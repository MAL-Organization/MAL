import shutil
import subprocess
import sys

import os

from common import extract_properties

if __name__ == '__main__':
    properties = extract_properties(sys.argv[1])
    # Erase current documentation
    shutil.rmtree(properties["path.to.doc"], ignore_errors=True)
    # Generate documentation
    process_arguments = list()
    process_arguments.append("doxygen")
    process_arguments.append(os.path.join(properties["basedir"], "Doxyfile"))
    retcode = subprocess.call(process_arguments)
    if retcode != 0:
        exit(retcode)
    # Copy index
    index_name = "index.html"
    index_source_file = os.path.join(properties["path.to.doc.resources"], index_name)
    index_destination_file = os.path.join(properties["path.to.doc"], index_name)
    shutil.copy(index_source_file, index_destination_file)
