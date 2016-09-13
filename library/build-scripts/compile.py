import shutil
import subprocess
import sys
from pathlib import Path

from xml.etree import ElementTree
from xml.etree.ElementTree import Element

import os

from common import extract_properties, import_project_to_workspace, build_project


def extract_cdt_project_exclusions(cproject_file_path: str, build_configuration: str):
    compiled_exclusions = list()
    """:type compiled_exclusions: list[str]"""
    # Load XML
    tree = ElementTree.parse(cproject_file_path)
    root = tree.getroot()  # type: Element
    # Get storage modules
    for storage_module in root.findall("./storageModule/cconfiguration/storageModule"):  # type: Element
        # Make sure this is a CDT storage module
        if storage_module.attrib.get("moduleId") != "cdtBuildSystem":
            continue
        # Make sure we have the correct build configuration
        configuration = storage_module.find("./configuration")  # type: Element
        if configuration.attrib.get("name") != build_configuration:
            continue
        # Extract source entries
        source_entries = configuration.findall("./sourceEntries/entry")
        """:type source_entries: list[Element]"""
        # Find all pure exclusions paths (must have an empty name attribute and an exclude value)
        pure_exclusions = list()
        """:type pure_exclusions: list[str]"""
        for source_entry in source_entries:
            if source_entry.attrib.get("name") != "" or source_entry.attrib.get("excluding") is None:
                continue
            exclusions = source_entry.attrib.get("excluding").split("|")
            for exclusion in exclusions:
                if exclusion:
                    pure_exclusions.append(exclusion)
        # Find entries that remove pure exclusion paths (must have a name that is in the pure exclusion paths and must
        # have a null exclusion)
        for source_entry in source_entries:
            # Check if the name is in the pure exclusions
            exclusion_found = False
            if source_entry.attrib.get("name") is None:
                continue
            for pure_exclusion in pure_exclusions:
                if source_entry.attrib.get("name") == pure_exclusion:
                    exclusion_found = True
                    break
            if exclusion_found is False:
                continue
            # Make sure excluding is not an attribute for this entry
            if source_entry.attrib.get("excluding") is not None:
                continue
            # Remove pure exclusion
            pure_exclusions.remove(source_entry.attrib.get("name"))

        # Find entries that specify a subset of a pure path (must have a name that is the pure exclusion path and must
        # have a valid exclusion)
        for source_entry in source_entries:
            # Check if the name is in the pure exclusions
            exclusion_found = False
            if source_entry.attrib.get("name") is None:
                continue
            for pure_exclusion in pure_exclusions:
                if source_entry.attrib.get("name") == pure_exclusion:
                    exclusion_found = True
                    break
            if exclusion_found is False:
                continue
            # Make sure excluding is an attribute for this entry
            if source_entry.attrib.get("excluding") is None:
                continue
            # Remove pure exclusion
            pure_exclusions.remove(source_entry.attrib.get("name"))
            # Add paths to compiled exclusions
            exclusions = source_entry.attrib.get("excluding").split("|")
            for exclusion in exclusions:
                if exclusion:
                    compiled_exclusions.append(exclusion)
        # Transfer remaining pure paths in the compiled list
        compiled_exclusions.extend(pure_exclusions)
        # Exclusion found stopping
        break
    return compiled_exclusions


def move_headers_in_target(build_exclusions, base_dir_path, build_directory, build_configuration):
    """
    :type build_exclusions: list[str]
    :type base_dir_path: str
    :type build_directory: str
    :type build_configuration: str
    """
    output_directory_path = Path(build_directory + "/library/" + build_configuration + "/headers/")
    # Extract header files
    headers = list()
    for root, dirs, files in os.walk(base_dir_path):
        for file in files:
            if file.endswith(".h"):
                headers.append(os.path.join(root, file))
    # Copy files
    for header in headers:  # type: str
        # We use Path to compare since we can abstract OS
        header_path = Path(header)
        # Make sure it isn't excluded
        excluded = False
        for exclusion in build_exclusions:
            if exclusion in header_path.as_posix():
                # When a specific file is excluded, only the name is specified. Make sure it is not a partial name match
                if exclusion in header_path.name and exclusion != header_path.name and \
                                exclusion not in header_path.as_posix().replace(header_path.name, ""):
                    continue
                excluded = True
                break
        if excluded:
            continue
        # Copy file to build directory headers with same relative path from base directory
        output_file_path = Path(output_directory_path.as_posix() + header.replace(base_dir_path, ""))
        os.makedirs(os.path.dirname(output_file_path.as_posix()), exist_ok=True)
        shutil.copy(header, output_file_path.as_posix())


def set_optimization_level(cproject_file_path, level, build_configurations):
    """
    :type cproject_file_path: str
    :type level: str
    :type build_configurations: list[str]
    """
    # Load XML
    tree = ElementTree.parse(cproject_file_path)
    root = tree.getroot()  # type: Element
    # Get storage modules
    for storage_module in root.findall("./storageModule/cconfiguration/storageModule"):  # type: Element
        # Make sure this is a CDT storage module
        if storage_module.attrib.get("moduleId") != "cdtBuildSystem":
            continue
        # Make sure we have the correct build configuration
        configuration = storage_module.find("./configuration")  # type: Element
        if configuration.attrib.get("name") not in build_configurations:
            continue
        # Change level
        options = configuration.findall("./folderInfo/toolChain/option")
        for option in options:  # type: Element
            if option.attrib.get("name") == "Optimization Level":
                option.attrib["value"] = "ilg.gnuarmeclipse.managedbuild.cross.option.optimization.level." + level
                break
    # Save XML
    with open(cproject_file_path, mode="wb") as cproject_file:
        cproject_file.write(b"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n")
        cproject_file.write(b"<?fileVersion 4.0.0?>")
        cproject_file.write(ElementTree.tostring(root))


if __name__ == '__main__':
    properties = extract_properties(sys.argv[1])
    cproject_file_path = properties["basedir"] + "\.cproject"
    # Extract requested build configurations
    build_configurations = properties["build.configurations"].split(",")
    # Extract exclusions and copy headers
    global_exlusions = properties["build.excludes"].split(",")
    for bc in build_configurations:
        build_exclusions = extract_cdt_project_exclusions(cproject_file_path, bc)
        # Add global exclusions
        build_exclusions.extend(global_exlusions)
        # Add target to exclusions
        build_exclusions.append(Path(properties["project.build.directory"]).as_posix())
        # Copy headers
        move_headers_in_target(build_exclusions, properties["basedir"], properties["project.build.directory"], bc)
    # Import project to new workspace
    print("Importing project to {}...".format(properties["path.to.workspace"]))
    result = import_project_to_workspace(properties["path.to.workspace"], properties["basedir"])
    if result != 0:
        exit(result)
    # Build for each optimization level
    optimization_levels = properties["optimization.levels"].split(",")
    for optimization_level in optimization_levels:
        # Set correct optimization level
        set_optimization_level(cproject_file_path, optimization_level, build_configurations)
        # Build library
        print("Building library with optimization level {}...".format(optimization_level))
        result = build_project(properties["path.to.workspace"], properties["project.name"], build_configurations)
        if result != 0:
            exit(result)
        # Copy libraries to build directory
        for bc in build_configurations:
            binary_name = "lib{}.a".format(properties["project.name"])
            input_file_path = os.path.join(properties["basedir"], bc, binary_name)
            output_file_path = os.path.join(properties["project.build.directory"], "library", bc, optimization_level)
            output_file_path = os.path.join(output_file_path, binary_name)
            os.makedirs(os.path.dirname(output_file_path), exist_ok=True)
            shutil.copy(input_file_path, output_file_path)
            # Delete build config outputs. For some reason, cleanBuild doesn't rebuild in some circumstances. I want to
            # force rebuild.
            shutil.rmtree(os.path.join(properties["basedir"], bc))
