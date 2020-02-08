import sys
import os
import argparse
import numpy as np
import utils
import reader
import tiles

def clean_directory(directory):
    for file_object in os.listdir(directory):
        file_object_path = os.path.join(directory, file_object)
        if os.path.isfile(file_object_path) or os.path.islink(file_object_path):
            os.unlink(file_object_path)
        else:
            clean_directory(file_object_path)

def ensure_directory_exists(directory):
    if not os.path.exists(directory):
        os.makedirs(directory)

def pad_filename(filename):
    length = len(filename)
    delta = utils.FILENAME_LENGTH - length
    if delta < 0:
        raise Exception("Filename too long, {}".format(length))
    return filename + '\0' * delta

def create_index_file(root_dir, width_tiles, height_tiles, xchunks, ychunks, xtiles_per_chunk, ytiles_per_chunk):
    filename = os.path.join(root_dir, "world.dat")
    with open(filename, "wb") as f:
        tile_size = np.array([width_tiles, height_tiles], dtype=np.uint32)
        other_data = np.array([xchunks, ychunks, xtiles_per_chunk, ytiles_per_chunk], dtype=np.uint16)
        fname_length = np.array([utils.FILENAME_LENGTH], dtype=np.uint8)
        
        f.write(tile_size.tobytes())
        f.write(other_data.tobytes())
        f.write(fname_length.tobytes())

        f.write(bytes(pad_filename(utils.CHUNK_DIRECTORY), encoding="utf-8"))
        for y in range(ychunks):
            for x in range(xchunks):
                f.write(bytes(pad_filename(utils.create_filename(x, y, xchunks)), encoding="utf-8"))

def write_chunk(chunkdata, file_object):
    file_object.write(chunkdata.tobytes())

def create_empty(directory, width_tiles, height_tiles, xtiles_per_chunk, ytiles_per_chunk):
    xchunks = int(width_tiles / xtiles_per_chunk) + (1 if width_tiles % xtiles_per_chunk != 0 else 0)
    ychunks = int(height_tiles / ytiles_per_chunk) + (1 if height_tiles % ytiles_per_chunk != 0 else 0)

    ensure_directory_exists(directory)
    clean_directory(directory)
    ensure_directory_exists(os.path.join(directory, utils.CHUNK_DIRECTORY))
    create_index_file(directory, width_tiles, height_tiles, xchunks, ychunks, xtiles_per_chunk, ytiles_per_chunk)

    data = np.ones(xtiles_per_chunk * ytiles_per_chunk, dtype=utils.TILE_DTYPE) * tiles.TILE_NONE
    for y in range(ychunks):
        for x in range(xchunks):
            filename = os.path.join(directory, utils.CHUNK_DIRECTORY, utils.create_filename(x, y, xchunks))
            with open(filename, "wb") as f:
                write_chunk(data, f)

# Splits world of width x height tiles into some number of chunks separated into files
def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--width", type=int, required=True, help="Width of world in number of tiles")
    parser.add_argument("--height", type=int, required=True, help="Height of world in number of tiles")
    parser.add_argument("--xtiles-per-chunk", type=int, required=False, default=256, help="Number of tiles per chunk in width direction")
    parser.add_argument("--ytiles-per-chunk", type=int, required=False, default=256, help="Number of tiles per chunk in height direction")
    parser.add_argument("-o", type=str, required=False, default="./", help="Directory for root of the world")

    args = parser.parse_args()

    width_tiles = args.width
    height_tiles = args.height
    xtiles_per_chunk = args.xtiles_per_chunk
    ytiles_per_chunk = args.ytiles_per_chunk
    output_dir = args.o

    create_empty(output_dir, width_tiles, height_tiles, xtiles_per_chunk, ytiles_per_chunk)

if __name__ == "__main__":
    main()