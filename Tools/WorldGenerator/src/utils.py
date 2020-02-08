import numpy as np

FILENAME_LENGTH = 16
BYTE_ORDER = "little"

TILE_BYTE_COUNT = 2
TILE_DTYPE = np.uint16

INDEX_FILENAME = "world.dat"
CHUNK_DIRECTORY = "chunks"

def get_index(x, y, width):
    return x + y * width

def create_filename(x, y, width):
    return str(get_index(x, y, width)) + ".dat"