# will change zip file to a rayt file
from pathlib import Path
from globalconst import *

def zip_to_rayt(name):
	p = Path(f"{STORAGE_PATH}{name}.zip")
	p.rename(p.with_suffix('.rayt'))