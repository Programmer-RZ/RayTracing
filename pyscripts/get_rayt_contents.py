from utils.changeto_zip import rayt_to_zip
from utils.unzip import unzip
from utils.getCurrentName import getCurrentName
from globalconst import *

rayt_to_zip(getCurrentName())
unzip(getCurrentName())