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
    lib_path = os.path.join(properties["basedir"], "library")
    # Load default toolchain
    default_tc = properties["build.toolchain.default"]

    for bc in build_configurations:
        bc_sources = None
        for ol in build_optimize_levels:
            # Clean build directory
            shutil.rmtree(properties["build.folder"], ignore_errors=True)
            # Create build folder
            os.makedirs(properties["build.folder"])
            # Load build configuration toolchain
            tc_key = "build.toolchain.{}".format(bc.lower())
            tc_path = os.path.join(lib_path, "{}.cmake".format(properties.get(tc_key, default_tc)))
            # Prepare build
            process_arguments = list()
            process_arguments.append("cmake")
            process_arguments.append("-DCMAKE_BUILD_TYPE=Release")
            process_arguments.append("-DCMAKE_TOOLCHAIN_FILE={}".format(tc_path))
            process_arguments.append("-DSET_OPTIMIZATION={}".format(ol))
            process_arguments.append("-H{}".format(lib_path))
            process_arguments.append("-B{}".format(properties["build.folder"]))
            process_arguments.append("-GCodeBlocks - Unix Makefiles")
            console_output = None
            try:
                console_output = subprocess.check_output(process_arguments, shell=True)
            except subprocess.CalledProcessError:
                exit(1)
            # Save sources
            if bc_sources is None:
                print(console_output)
                print("**************************************")

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
