from PIL import Image
import numpy as np

# other python files
from date_time import getDateTime
from globalconst import *
from utils.getCurrentName import getCurrentName


def exportImg(pictureFormat):
    array = []
    name = ""
    width = 0
    height = 0
    with open(IMAGEDATA_PATH, "r") as data:
        for row, line in enumerate(data.readlines()):
            if row == 1:
                # get width and height of ppm
                width, height = map(int, line.split(' '))

            if width != 0 and (row - 3) % width == 0:
                # new row
                array.insert(0, [])

            if row > 2:
                r, g, b = line.split(' ')
                array[0].append((r, g, b))

    # reset image_data.ppm to 0 bytes
    with open(IMAGEDATA_PATH, "w") as data:
        data.write("")

    pixel_data = np.array(array, dtype=np.uint8)
    name = getCurrentName()

    #with open(f"../Data/image_data.ppm", 'rb') as f:
        #img = Image.open(f)
        #img = img.convert('RGB')
        #img.save(f"../Images/{name}.{pictureFormat}")
    #image.convert("RGB")

    image = Image.fromarray(pixel_data)
    image.save(f"{IMAGES_PATH}{name}.{pictureFormat}")