import os

from globalconst import *

def getNumImages():
	img_formats = ["jpg", "png"]
	numofimages = 0

	# iterate the directory
	for path in os.listdir(IMAGES_PATH):
		# check if current path is a file
		if os.path.isfile(os.path.join(IMAGES_PATH, path)):
			# check if file is png or jpg
			if os.path.splitext(os.path.join(IMAGES_PATH, path))[1] in img_formats:
				numofimages += 1

	return numofimages