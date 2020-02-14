import os
import utils
import numpy as np

def clean_string(string):
    last_char = string.find('\0')
    if last_char >= 0:
        return string[:last_char]
    return string

class TileIndex:
    def __init__(self, xchunk, ychunk, xoffset, yoffset):
        self.xchunk = xchunk
        self.ychunk = ychunk
        self.xoffset = xoffset
        self.yoffset = yoffset

class WorldReader:
    def __init__(self, directory, max_space=1 * 1024 * 1024 * 1024):
        self.root_directory = directory
        self.world_file = os.path.join(self.root_directory, utils.INDEX_FILENAME)
        self.filenames = []
        with open(self.world_file, "rb") as f:
            file_data = f.read()
            size_data = np.frombuffer(file_data[:8], dtype=np.uint32)
            other_data = np.frombuffer(file_data[8:16], dtype=np.uint16)
            filename_len_data = np.frombuffer(file_data[16:], dtype=np.uint8, count=1)

            self.width_tiles = size_data[0]
            self.height_tiles = size_data[1]
            self.xchunks = other_data[0]
            self.ychunks = other_data[1]
            self.xtiles_per_chunk = other_data[2]
            self.ytiles_per_chunk = other_data[3]
            self.filename_length = filename_len_data[0]

            start_index = 17
            end_index = start_index + self.filename_length
            self.chunk_directory = os.path.join(self.root_directory, clean_string(str(file_data[start_index : end_index], encoding="utf-8")))
            for i in range(self.xchunks * self.ychunks):
                start_index = 17 + (i + 1) * self.filename_length
                end_index = start_index + self.filename_length
                fname = os.path.join(self.chunk_directory, clean_string(str(file_data[start_index : end_index], encoding="utf-8")))
                self.filenames.append(fname)

        self.max_space_allowed = max_space
        self.loaded_chunks = {}
        self.recently_used = []

    def commit(self):
        for index in self.loaded_chunks:
            self._update_chunk_index(index, self.loaded_chunks[index])
        self.loaded_chunks.clear()
        self.recently_used.clear()

    def get_tile(self, x, y):
        index = self.create_tile_index(x, y)
        data = self.load_chunk(index.xchunk, index.ychunk)
        data_index = self.get_tile_offset_index(index.xoffset, index.yoffset)
        return data[data_index]

    def get_tiles(self, x, y, width, height):
        min_index = self.create_tile_index(x, y)
        max_index = self.create_tile_index(x + width - 1, y + height - 1)

        result = np.zeros((width, height), dtype=utils.TILE_DTYPE)
        current_x = 0
        current_y = 0
        for ychunk in range(min_index.ychunk, max_index.ychunk + 1):
            base_y = current_y
            current_x = 0
            for xchunk in range(min_index.xchunk, max_index.xchunk + 1):
                xoffset = 0
                yoffset = 0
                xend = self.xtiles_per_chunk - 1
                yend = self.ytiles_per_chunk - 1
                if xchunk == min_index.xchunk:
                    xoffset = min_index.xoffset
                if xchunk == max_index.xchunk:
                    xend = max_index.xoffset
                if ychunk == min_index.ychunk:
                    yoffset = min_index.yoffset
                if ychunk == max_index.ychunk:
                    yend = max_index.yoffset
                data = self.load_chunk(xchunk, ychunk)

                base_x = current_x
                row_index = 0
                for row in range(yoffset, yend + 1):
                    index = utils.get_index(xoffset, row, self.xtiles_per_chunk)
                    row_width = xend - xoffset + 1
                    row_data = data[index : index + row_width]
                    result[base_x : base_x + row_width, base_y + row_index] = row_data
                    current_x = row_width + base_x
                    row_index += 1
                current_y = base_y + row_index
        return result.T

    def set_tile(self, x, y, tile):
        index = self.create_tile_index(x, y)
        data = self.load_chunk(index.xchunk, index.ychunk)
        data_index = self.get_tile_offset_index(index.xoffset, index.yoffset)
        data[data_index] = utils.TILE_DTYPE(tile)

    def set_tiles(self, x, y, width, height, data):
        min_index = self.create_tile_index(x, y)
        max_index = self.create_tile_index(x + width - 1, y + height - 1)

        current_x = 0
        current_y = 0
        for ychunk in range(min_index.ychunk, max_index.ychunk + 1):
            base_y = current_y
            current_x = 0
            for xchunk in range(min_index.xchunk, max_index.xchunk + 1):
                xoffset = 0
                yoffset = 0
                xend = self.xtiles_per_chunk - 1
                yend = self.ytiles_per_chunk - 1
                if xchunk == min_index.xchunk:
                    xoffset = min_index.xoffset
                if xchunk == max_index.xchunk:
                    xend = max_index.xoffset
                if ychunk == min_index.ychunk:
                    yoffset = min_index.yoffset
                if ychunk == max_index.ychunk:
                    yend = max_index.yoffset
                chunk_data = self.load_chunk(xchunk, ychunk)

                base_x = current_x
                row_index = 0
                for row in range(yoffset, yend + 1):
                    index = utils.get_index(xoffset, row, self.xtiles_per_chunk)
                    row_width = xend - xoffset + 1

                    data_index = base_x + (base_y + row_index) * width

                    chunk_data[index : index + row_width] = data[data_index : data_index + row_width]
                    current_x = row_width + base_x
                    row_index += 1
                current_y = base_y + row_index

    def calculate_chunk_size(self):
        return int(self.xtiles_per_chunk) * int(self.ytiles_per_chunk) * utils.TILE_BYTE_COUNT

    def calculate_space_used(self):
        return len(self.loaded_chunks) * self.calculate_chunk_size()

    def get_tile_offset_index(self, tile_x, tile_y):
        return utils.get_index(tile_x, tile_y, self.xtiles_per_chunk)

    def get_chunk_index(self, chunk_x, chunk_y):
        return utils.get_index(chunk_x, chunk_y, self.xchunks)

    def create_tile_index(self, tile_x, tile_y):
        if tile_x < 0 or tile_x >= self.width_tiles or tile_y < 0 or tile_y >= self.height_tiles:
            raise Exception("Tile index out of range")
        chunkx = int(tile_x / self.xtiles_per_chunk)
        chunky = int(tile_y / self.ytiles_per_chunk)
        xoffset = tile_x - chunkx * self.xtiles_per_chunk
        yoffset = tile_y - chunky * self.ytiles_per_chunk
        return TileIndex(chunkx, chunky, xoffset, yoffset)

    def load_chunk(self, x, y):
        index = self.get_chunk_index(x, y)
        if index not in self.loaded_chunks:
            filename = self.filenames[index]
            with open(filename, "rb") as f:
                data = np.array(np.frombuffer(f.read(), dtype=utils.TILE_DTYPE))
                self.loaded_chunks[index] = data
        self._use_chunk(index)
        while self.calculate_space_used() > self.max_space_allowed:
            self._purge_chunk()
        return self.loaded_chunks[index]

    def update_chunk(self, x, y, data):
        if len(data) != self.xtiles_per_chunk * self.ytiles_per_chunk:
            raise Exception("Invalid data length")
        index = self.get_chunk_index(x, y)
        self._update_chunk_index(index)

    def _update_chunk_index(self, index, data):
        if index in self.loaded_chunks:
            self.loaded_chunks[index] = data
        filename = self.filenames[index]
        with open(filename, "wb") as f:
            f.write(data.tobytes())
        self._use_chunk(index)

    def _use_chunk(self, chunk_index):
        if chunk_index in self.loaded_chunks:
            if chunk_index in self.recently_used:
                self.recently_used.remove(chunk_index)
            self.recently_used.append(chunk_index)

    def _purge_chunk(self):
        if len(self.recently_used) > 0 and len(self.loaded_chunks) > 0:
            index = self.recently_used[0]
            self._update_chunk_index(index, self.loaded_chunks[index])
            del self.loaded_chunks[index]
            self.recently_used = self.recently_used[1:]