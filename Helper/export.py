from PIL import Image
import numpy as np

array = []
name = ""
with open("../Data/image_data.txt", "r") as data:
    for row, line in enumerate(data.readlines()):
        if "row" in line:
            array.insert(0, [])
        
        elif ',' in line:
            r, g, b, _ = line.split(',')
            array[0].append((r, g, b))
        
        elif "." in line:
            name = line[1:-2]

pixel_data = np.array(array, dtype=np.uint8)

image = Image.fromarray(pixel_data)
image.save(f"../Images/{name}.png")