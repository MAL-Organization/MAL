import shutil
import subprocess
import sys
from pathlib import Path

import os

from common import extract_properties, clean_build_folder, prepare_cmake_build, build_cmake_project


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
            clean_build_folder(properties["build.folder"])
            # Create build folder
            os.makedirs(properties["build.folder"])
            # Load build configuration toolchain
            tc_key = "build.toolchain.{}".format(bc.lower())
            tc_path = Path(os.path.join(lib_path, "{}.cmake".format(properties.get(tc_key, default_tc)))).as_posix()
            # Prepare build
            console_output = prepare_cmake_build(tc_path, ol, lib_path, build_path)
            # Save sourcesbuild_path
            if bc_sources is None:
                lines = str(console_output).split("\\n")
                marker_index = lines.index("~~~~~{}".format(bc))
                file_paths = lines[marker_index + 1].split(";")
                bc_sources = file_paths
            # Execute build
            build_cmake_project(build_path, target=bc)
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
