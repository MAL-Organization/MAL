import shutil
import subprocess
from collections import OrderedDict
from typing import Tuple, List


def extract_properties(property_file_path: str) -> dict:
    props = OrderedDict()
    with open(property_file_path) as property_file:
        for line in property_file.readlines():
            # Clean up line
            line = line.strip()
            line = line.replace("\\:", ":")
            # Skip comments
            if line.startswith("#"):
                continue
            # Split key value
            components = line.split("=")
            if len(components) != 2:
                continue
            # Append property
            props[components[0]] = components[1]
    return props


def clean_build_folder(build_folder_path: str):
    shutil.rmtree(build_folder_path, ignore_errors=True)


def prepare_cmake_build(toolchain_path: str, optimization_level: str, source_path: str, build_path: str,
                        build_type: str = "Release", definitions: List[str] = None) -> str:
    process_arguments = list()
    process_arguments.append("cmake")
    process_arguments.append("-DCMAKE_BUILD_TYPE={}".format(build_type))
    process_arguments.append("-DCMAKE_TOOLCHAIN_FILE={}".format(toolchain_path))
    process_arguments.append("-DSET_OPTIMIZATION={}".format(optimization_level))
    if definitions is not None:
        for definition in definitions:
            process_arguments.append("-D{}".format(definition))
    process_arguments.append("-H{}".format(source_path))
    process_arguments.append("-B{}".format(build_path))
    process_arguments.append("-GCodeBlocks - Unix Makefiles")
    console_output = None
    try:
        console_output = subprocess.check_output(process_arguments, shell=True)
    except subprocess.CalledProcessError:
        exit(1)
    return console_output


def build_cmake_project(build_path: str, target: str):
    process_arguments = list()
    process_arguments.append("cmake")
    process_arguments.append("--build")
    process_arguments.append(build_path)
    process_arguments.append("--clean-first")
    process_arguments.append("--target")
    process_arguments.append(target)
    result = subprocess.call(process_arguments)
    if result != 0:
        exit(result)
