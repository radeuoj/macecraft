import sys
from PIL import Image

image = Image.open(sys.argv[1])
image = image.transpose(Image.Transpose.FLIP_TOP_BOTTOM)
image.save(sys.argv[2])