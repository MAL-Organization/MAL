import shutil
import subprocess
from collections import OrderedDict
from typing import List, Union


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


def get_wrapped_command(path_additions: Union[List[str], None], command: List[str]):
    command_wrapper = list()
    if path_additions is not None:
        command_wrapper.append("cmd")
        command_wrapper.append("/c")
        set_command = r'"set "PATH=%PATH%'
        for path_addition in path_additions:
            set_command += ";{}".format(path_addition)
        set_command += '" && {}"'.format(" ".join(command))
        command_wrapper.append(set_command)
    else:
        command_wrapper.append(" ".join(command))
    return " ".join(command_wrapper)


def prepare_cmake_build(toolchain_path: str, optimization_level: str, source_path: str, build_path: str,
                        build_type: str = "Release", definitions: List[str] = None, make_path: str = None,
                        path_additions: List[str] = None, cmake_path: str = None) -> str:
    command = list()
    # Choose cmake
    if cmake_path is not None:
        command.append(cmake_path)
    else:
        command.append("cmake")
    # Check if make should be specified
    if make_path is not None:
        command.append("-DCMAKE_MAKE_PROGRAM={}".format(make_path))
    command.append("-DCMAKE_BUILD_TYPE={}".format(build_type))
    command.append("-DCMAKE_TOOLCHAIN_FILE={}".format(toolchain_path))
    command.append("-DSET_OPTIMIZATION={}".format(optimization_level))
    if definitions is not None:
        for definition in definitions:
            command.append("-D{}".format(definition))
    command.append("-H{}".format(source_path))
    command.append("-B{}".format(build_path))
    command.append("-G\"CodeBlocks - Unix Makefiles\"")

    string_command = get_wrapped_command(path_additions, command)
    console_output = None
    try:
        console_output = subprocess.check_output(string_command, shell=True)
    except subprocess.CalledProcessError:
        exit(1)
    return console_output


def build_cmake_project(build_path: str, target: str, cmake_path: str = None, path_additions: List[str] = None):
    command = list()
    # Choose cmake
    if cmake_path is not None:
        command.append(cmake_path)
    else:
        command.append("cmake")
    command.append("--build")
    command.append(build_path)
    command.append("--clean-first")
    command.append("--target")
    command.append(target)
    command.append("--")
    command.append("-j")
    command.append("4")

    string_command = get_wrapped_command(path_additions, command)
    result = subprocess.call(string_command, shell=True)
    if result != 0:
        exit(result)
