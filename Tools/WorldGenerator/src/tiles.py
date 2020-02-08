import numpy as np

TILE_NONE = 0
TILE_GRASS = 1
TILE_SAND = 2
TILE_WATER = 3
TILE_STONE = 4

TILE_COLOR_LOOKUP = {
    TILE_NONE : np.array([0, 0, 0], dtype=np.uint8),
    TILE_GRASS : np.array([0, 255, 0], dtype=np.uint8),
    TILE_SAND : np.array([255, 255, 0], dtype=np.uint8),
    TILE_WATER : np.array([0, 0, 255], dtype=np.uint8),
    TILE_STONE : np.array([150, 150, 150], dtype=np.uint8),
}

TILE_COLOR_ARRAY = []
for i in range(len(TILE_COLOR_LOOKUP)):
    TILE_COLOR_ARRAY.append(TILE_COLOR_LOOKUP[i])
TILE_COLOR_ARRAY = np.array(TILE_COLOR_ARRAY, dtype=np.uint8)