# will change rayt file to a zip file
from pathlib import Path
from globalconst import *

def rayt_to_zip(name):
	p = Path(f"{STORAGE_PATH}{name}.rayt")
	p.rename(p.with_suffix('.zip'))