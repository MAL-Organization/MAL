import subprocess
import sys

import os
from pathlib import Path

from common import extract_properties, clean_build_folder, prepare_cmake_build, build_cmake_project, get_wrapped_command

if __name__ == '__main__':
    properties = extract_properties(sys.argv[1])
    source_path = Path(os.path.join(properties["basedir"], "test")).as_posix()
    build_path = Path(properties["build.test.folder"]).as_posix()
    # Clean
    clean_build_folder(properties["build.test.folder"])
    # Prepare build
    optimization_level = "O0"
    tc_name = properties["build.test.toolchain"]
    tc_path = Path(os.path.join(source_path, "{}.cmake".format(tc_name))).as_posix()
    lib_name = "lib{}.a".format(properties["project.name"])
    lib_path = os.path.join(properties["project.build.directory"], "library", "GNU", optimization_level, lib_name)
    make_path = properties["build.make.gnu"]
    cmake_path = properties["build.cmake.gnu"]
    definitions = ["MAL_LIB_PATH={}".format(lib_path)]
    path_additions_str: str = properties["build.path.gnu"]
    path_additions = path_additions_str.split(",")
    prepare_cmake_build(tc_path, optimization_level=optimization_level, source_path=source_path, build_path=build_path,
                        definitions=definitions, make_path=make_path, cmake_path=cmake_path,
                        path_additions=path_additions)
    # Build test project
    build_cmake_project(build_path, target="{}-test".format(properties["project.name"]), cmake_path=cmake_path,
                        path_additions=path_additions)
    # Execute test
    command = list()
    command.append(os.path.join(build_path, "{}-test".format(properties["project.name"])))
    string_command = get_wrapped_command(path_additions, command)
    result = subprocess.call(string_command, shell=True)
    exit(result)
