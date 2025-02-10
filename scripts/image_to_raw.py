import sys
from PIL import Image
import numpy as np

image = Image.open(sys.argv[1]).convert("RGB")
image = image.transpose(Image.Transpose.FLIP_TOP_BOTTOM)

image = np.asarray(image)

image.tofile(sys.argv[2])