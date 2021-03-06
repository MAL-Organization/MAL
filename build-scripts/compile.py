import shutil
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
    compare_lib_path = lib_path
    # Drive letter capitalization is inconsistent, make it lower case so it matches all the time
    if compare_lib_path.startswith("C:"):
        compare_lib_path = compare_lib_path.replace("C:", "c:", 1)
    # Load build paths
    build_path = Path(properties["build.folder"]).as_posix()
    binary_name = "lib{}.a".format(properties["project.name"])
    binary_path = os.path.join(build_path, binary_name)
    # Load default toolchain
    default_tc = properties["build.toolchain.default"]
    # Load default make
    default_make = properties["build.make.default"]
    # Load default cmake
    default_cmake = properties["build.cmake.default"]

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
            # Load build configuration make
            make_key = "build.make.{}".format(bc.lower())
            make_path = properties.get(make_key, default_make)
            # Load build configuration cmake
            cmake_key = "build.cmake.{}".format(bc.lower())
            cmake_path = properties.get(cmake_key, default_cmake)
            # Load build configuration path additions
            path_additions_key = "build.path.{}".format(bc.lower())
            path_additions_str: str = properties.get(path_additions_key)
            path_additions = None
            if path_additions_str is not None:
                path_additions = path_additions_str.split(",")
            # Prepare build
            console_output = prepare_cmake_build(tc_path, ol, lib_path, build_path, make_path=make_path,
                                                 cmake_path=cmake_path, path_additions=path_additions)
            # Save sourcesbuild_path
            if bc_sources is None:
                lines = str(console_output).split("\\n")
                marker_index = lines.index("~~~~~{}".format(bc))
                file_paths = lines[marker_index + 1].split(";")
                bc_sources = file_paths
            # Execute build
            build_cmake_project(build_path, target=bc, cmake_path=cmake_path, path_additions=path_additions)
            # Copy library to build directory
            output_file_path = os.path.join(bc_path, ol)
            output_file_path = os.path.join(output_file_path, binary_name)
            os.makedirs(os.path.dirname(output_file_path), exist_ok=True)
            shutil.copy(binary_path, output_file_path)
        # Extract headers
        headers_path = os.path.join(bc_path, "headers")
        for bc_source in bc_sources:
            # Filter path so it matches syntax of lib path
            bc_source = Path(bc_source).as_posix()
            # Drive letter capitalization is inconsistent, make it lower case so it matches all the time
            if bc_source.startswith("C:"):
                bc_source = bc_source.replace("C:", "c:", 1)
            if bc_source.endswith(".h"):
                # Clean path
                if compare_lib_path in bc_source:
                    bc_source = bc_source.replace(compare_lib_path, "")
                    # Remove dangling slash
                    bc_source = bc_source[1:]
                # Copy to output
                source_file = os.path.join(lib_path, bc_source)
                output_file = os.path.join(headers_path, bc_source)
                os.makedirs(os.path.dirname(output_file), exist_ok=True)
                try:
                    shutil.copy(source_file, output_file)
                except shutil.SameFileError:
                    print("Error copying {} to {}".format(bc_source, output_file))
                    raise
