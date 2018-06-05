import shutil
import subprocess
import sys
from pathlib import Path

import os

from common import extract_properties


def get_headers(build_configuration: str, cmake_file: str):
    pass


if __name__ == '__main__':
    properties = extract_properties(sys.argv[1])
    # Load requested build configurations
    build_configurations = properties["build.configurations"].split(",")
    # Load requested optimizations
    build_optimize_levels = properties["build.optimization.levels"].split(",")
    # Load lib path
    lib_path = Path(os.path.join(properties["basedir"], "library")).as_posix()
    # Load build paths
    build_path = Path(properties["build.folder"]).as_posix()
    binary_name = "lib{}.a".format(properties["project.name"])
    binary_path = os.path.join(build_path, binary_name)
    # Load default toolchain
    default_tc = properties["build.toolchain.default"]

    for bc in build_configurations:
        bc_sources = None
        bc_path = os.path.join(properties["project.build.directory"], "library", bc)
        for ol in build_optimize_levels:
            # Clean build directory
            shutil.rmtree(properties["build.folder"], ignore_errors=True)
            # Create build folder
            os.makedirs(properties["build.folder"])
            # Load build configuration toolchain
            tc_key = "build.toolchain.{}".format(bc.lower())
            tc_path = Path(os.path.join(lib_path, "{}.cmake".format(properties.get(tc_key, default_tc)))).as_posix()
            # Prepare build
            process_arguments = list()
            process_arguments.append("cmake")
            process_arguments.append("-DCMAKE_BUILD_TYPE=Release")
            process_arguments.append("-DCMAKE_TOOLCHAIN_FILE={}".format(tc_path))
            process_arguments.append("-DSET_OPTIMIZATION={}".format(ol))
            process_arguments.append("-H{}".format(lib_path))
            process_arguments.append("-B{}".format(build_path))
            process_arguments.append("-GCodeBlocks - Unix Makefiles")
            console_output = None
            try:
                console_output = subprocess.check_output(process_arguments, shell=True)
            except subprocess.CalledProcessError:
                exit(1)
            # Save sources
            if bc_sources is None:
                lines = str(console_output).split("\\n")
                marker_index = lines.index("~~~~~{}".format(bc))
                file_paths = lines[marker_index + 1].split(";")
                bc_sources = file_paths
            # Execute build
            process_arguments = list()
            process_arguments.append("cmake")
            process_arguments.append("--build")
            process_arguments.append(build_path)
            process_arguments.append("--clean-first")
            process_arguments.append("--target")
            process_arguments.append(bc)
            result = subprocess.call(process_arguments)
            if result != 0:
                exit(result)
            # Copy library to build directory
            output_file_path = os.path.join(bc_path, ol)
            output_file_path = os.path.join(output_file_path, binary_name)
            os.makedirs(os.path.dirname(output_file_path), exist_ok=True)
            shutil.copy(binary_path, output_file_path)
        # Extract headers
        headers_path = os.path.join(bc_path, "headers")
        for bc_source in bc_sources:
            if bc_source.endswith(".h"):
                # Clean path
                if lib_path in bc_source:
                    bc_source = bc_source.replace(lib_path, "")
                    # Remove dangling slash
                    bc_source = bc_source[1:]
                # Copy to output
                source_file = os.path.join(lib_path, bc_source)
                output_file = os.path.join(headers_path, bc_source)
                os.makedirs(os.path.dirname(output_file), exist_ok=True)
                shutil.copy(source_file, output_file)

    # # Build for each optimization level
    # optimization_levels = properties["optimization.levels"].split(",")
    # for optimization_level in optimization_levels:
    #     # Set correct optimization level
    #     set_optimization_level(cproject_file_path, optimization_level, build_configurations)
    #     # Build library
    #     print("Building library with optimization level {}...".format(optimization_level))
    #     result = build_project(properties["path.to.workspace"], properties["project.name"], build_configurations)
    #     if result != 0:
    #         exit(result)
    #     # Copy libraries to build directory
    #     for bc in build_configurations:
    #         binary_name = "lib{}.a".format(properties["project.name"])
    #         input_file_path = os.path.join(properties["basedir"], bc, binary_name)
    #         output_file_path = os.path.join(properties["project.build.directory"], "library", bc, optimization_level)
    #         output_file_path = os.path.join(output_file_path, binary_name)
    #         os.makedirs(os.path.dirname(output_file_path), exist_ok=True)
    #         shutil.copy(input_file_path, output_file_path)
    #         # Delete build config outputs. For some reason, cleanBuild doesn't rebuild in some circumstances. I want to
    #         # force rebuild.
    #         shutil.rmtree(os.path.join(properties["basedir"], bc))
