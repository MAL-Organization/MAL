import subprocess
import sys

import os
from pathlib import Path

from common import extract_properties, clean_build_folder, prepare_cmake_build, build_cmake_project

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
    definitions = ["MAL_LIB_PATH={}".format(lib_path)]
    prepare_cmake_build(tc_path, optimization_level=optimization_level, source_path=source_path, build_path=build_path,
                        definitions=definitions)
    # Build test project
    build_cmake_project(build_path, target="{}-test".format(properties["project.name"]))
    # Execute test
    process_arguments = list()
    process_arguments.append(os.path.join(build_path, "{}-test".format(properties["project.name"])))
    result = subprocess.call(process_arguments)
    exit(result)
