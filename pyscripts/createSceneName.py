import os
from utils.getNumImages import getNumImages
from globalconst import *

if os.stat(SCENENAME_PATH).st_size == 0:
    # empty
    numofimages = getNumImages()
    with open(SCENENAME_PATH, "w") as name:
        name.write(f"Untitled{numofimages}")