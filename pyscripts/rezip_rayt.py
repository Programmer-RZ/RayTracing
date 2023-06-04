from utils.changeto_rayt import zip_to_rayt
from utils.getCurrentName import getCurrentName
from globalconst import *
import zipfile
import os

name = getCurrentName()

# delete old zip file
if os.path.isfile(f"{STORAGE_PATH}{name}.zip"):
    os.remove(f"{STORAGE_PATH}{name}.zip")

# full file path
# needed when writing the files to the zip file
FULL_INIFILE_PATH = os.path.abspath(INIFILE_PATH)
FULL_SPHERENAME_PATH = os.path.abspath(SPHERENAME_PATH)
FULL_MATERIALNAME_PATH = os.path.abspath(MATERIALNAME_PATH)

# create new empty zip file
with zipfile.ZipFile(f"{STORAGE_PATH}{name}.zip", 'w') as myzip:
    myzip.write(FULL_INIFILE_PATH, arcname="sceneInfo.ini")
    myzip.write(FULL_SPHERENAME_PATH, arcname="sphere_names.txt")
    myzip.write(FULL_MATERIALNAME_PATH, arcname="material_names.txt")

zip_to_rayt(name)