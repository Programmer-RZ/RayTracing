import zipfile
from globalconst import *
from utils.getCurrentName import getCurrentName

def unzip(name):
    with zipfile.ZipFile(f"{STORAGE_PATH}{getCurrentName()}.zip", 'r') as zip_ref:
        zip_ref.extractall(TEMP_STORAGE_PATH + "scene")