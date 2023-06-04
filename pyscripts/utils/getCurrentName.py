from globalconst import *

def getCurrentName():
	with open(f"{TEMP_STORAGE_PATH}currentSceneName.txt", "r") as name:
		return name.read()