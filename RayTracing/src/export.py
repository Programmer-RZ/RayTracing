from PIL import Image
import numpy as np

array = []
with open("image_data.txt", "r") as data:
    
    for row, line in enumerate(data.readlines()):
        if line == "row":
            array.append([])
        
        else:
            r, g, b = line.split(',')
            array[-1].append((r, g, b))

pixel_data = np.array(array, dtype=np.uint8)

image = Image.fromarray(pixel_data)
image.save("Scene.png")