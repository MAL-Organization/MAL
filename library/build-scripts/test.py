import subprocess
import sys

import os

from common import extract_properties, import_project_to_workspace, build_project

if __name__ == '__main__':
    properties = extract_properties(sys.argv[1])
    # Import project to new workspace
    print("Importing project to {}...".format(properties["path.to.workspace"]))
    result = import_project_to_workspace(properties["path.to.workspace"],
                                         os.path.join(properties["basedir"], "../test"))
    if result != 0:
        exit(result)
    # Build test project
    print("Building test project...")
    result = build_project(properties["path.to.workspace"],
                           "{}-test".format(properties["project.name"]),
                           ["Debug"])
    if result != 0:
        exit(result)
    # Execute test
    process_arguments = list()
    process_arguments.append(os.path.join(properties["basedir"],
                                          "../test/Debug",
                                          "{}-test".format(properties["project.name"])))
    result = subprocess.call(process_arguments)
    exit(result)
