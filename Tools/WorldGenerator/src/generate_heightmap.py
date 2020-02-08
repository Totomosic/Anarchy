import generate_empty
import image
import utils
import tiles
from reader import WorldReader
from PIL import Image
import numpy as np
import argparse

class HeightCutoffs:
    def __init__(self):
        self.lookup = {}
        self.cutoffs = []

    def add_cutoff(self, height, tile):
        self.lookup[height] = tile
        self.cutoffs.append(height)
        self.cutoffs.sort(reverse=True)

    @staticmethod
    def default():
        cutoffs = HeightCutoffs()
        #cutoffs.add_cutoff(1.0, tiles.TILE_STONE)
        cutoffs.add_cutoff(1.0, tiles.TILE_GRASS)
        cutoffs.add_cutoff(0.0, tiles.TILE_SAND)
        cutoffs.add_cutoff(-0.5, tiles.TILE_WATER)
        return cutoffs

def create_height_array(image):
    return (image / 255 - 0.5) * 2.0

# Create from tilemap, heightmap_array_2d is a numpy 2d array of values ranging from 0->255
def create_from_heightmap(directory, heightmap_array_2d, xtiles_per_chunk, ytiles_per_chunk, cutoffs=HeightCutoffs.default()):
    width_tiles, height_tiles = heightmap_array_2d.shape
    generate_empty.create_empty(directory, width_tiles, height_tiles, xtiles_per_chunk, ytiles_per_chunk)
    heights = create_height_array(heightmap_array_2d.flatten())

    tile_data = np.zeros(width_tiles * height_tiles, dtype=utils.TILE_DTYPE)
    for height in cutoffs.cutoffs:
        tile_data[heights <= height] = cutoffs.lookup[height]

    reader = WorldReader(directory)
    reader.set_tiles(0, 0, width_tiles, height_tiles, tile_data)
    reader.commit()

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--xtiles-per-chunk", type=int, required=False, default=256, help="Number of tiles per chunk in width direction")
    parser.add_argument("--ytiles-per-chunk", type=int, required=False, default=256, help="Number of tiles per chunk in height direction")
    parser.add_argument("--heightmap", type=str, required=True, help="Heightmap filename")
    parser.add_argument("--scaling", type=float, required=False, default=1.0, help="Scaling for converting image pixels to tiles")
    parser.add_argument("-o", type=str, required=False, default="./", help="Directory for root of the world")
    parser.add_argument("-i", "--image", type=str, required=False, help="Generate an image of the tilemap")

    args = parser.parse_args()

    scaling = args.scaling
    img = Image.open(args.heightmap).convert("L")
    if scaling != 1.0:
        img = img.resize((int(img.size[0] * scaling), int(img.size[1] * scaling)), Image.BILINEAR)

    width_tiles, height_tiles = img.size
    xtiles_per_chunk = args.xtiles_per_chunk
    ytiles_per_chunk = args.ytiles_per_chunk
    output_directory = args.o
    
    create_from_heightmap(output_directory, np.array(img.getdata()).reshape(width_tiles, height_tiles), xtiles_per_chunk, ytiles_per_chunk)
    if args.image:
        image.write_image(args.image, image.create_image(WorldReader(output_directory).get_tiles(0, 0, width_tiles, height_tiles)))

if __name__ == "__main__":
    main()