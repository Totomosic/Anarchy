import numpy
from PIL import Image
import tiles

def create_image(tile_data):
    colors = tiles.TILE_COLOR_ARRAY[tile_data.T]
    image = Image.fromarray(colors)
    return image

def write_image(filename, image):
    with open(filename, "wb") as f:
        image.save(f, format="png")