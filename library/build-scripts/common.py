import subprocess
from collections import OrderedDict


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


def import_project_to_workspace(path_to_workspace: str, path_to_project: str) -> int:
    process_arguments = list()
    process_arguments.append("eclipsec")
    process_arguments.append("--launcher.suppressErrors")
    process_arguments.append("-nosplash")
    process_arguments.append("-application")
    process_arguments.append("org.eclipse.cdt.managedbuilder.core.headlessbuild")
    process_arguments.append("-data")
    process_arguments.append(path_to_workspace)
    process_arguments.append("-import")
    process_arguments.append(path_to_project)
    return subprocess.call(process_arguments)


def build_project(path_to_workspace: str, project: str, build_configurations: list) -> int:
    process_arguments = list()
    process_arguments.append("eclipsec")
    process_arguments.append("--launcher.suppressErrors")
    process_arguments.append("-nosplash")
    process_arguments.append("-application")
    process_arguments.append("org.eclipse.cdt.managedbuilder.core.headlessbuild")
    process_arguments.append("-data")
    process_arguments.append(path_to_workspace)
    for bc in build_configurations:
        process_arguments.append("-cleanBuild")
        process_arguments.append("{0}/{1}".format(project, bc))
    return subprocess.call(process_arguments)
