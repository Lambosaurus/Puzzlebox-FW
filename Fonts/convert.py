
from PIL import Image
import os


class Font():
    def __init__(self, name: str, path: str, width: int, height: int, char_count = 95):
        self.name = name
        self.src_char_height = height
        self.src_char_width = width + 1
        self.src_cols = 16
        self.src_rows = 6
        self.src_char_count = char_count
        self.offset_x = 77
        self.offset_y = 41
        self.ascii_offset = 32
        self.set_vcrop(0,0)
        self.img = Image.open(path)
        self.pad_rules: dict[str, tuple[int,int]] = {}
        self.char_start = 0
        self.char_count = char_count

    def set_vcrop(self, top: int, bot: int):
        self.top_crop = top
        self.bot_crop = bot
        self.height = self.src_char_height - (self.top_crop + self.bot_crop)

    def set_char_range(self, ch_start = ' ', ch_end = '~'):
        self.char_start = ord(ch_start) - self.ascii_offset
        self.char_count = ord(ch_end) + 1 - ord(ch_start)

    def get_char_img(self, index) -> Image.Image:
        row = int(index / self.src_cols)
        col = index - row * self.src_cols
        char_width = self.src_char_width
        char_height = self.src_char_height
        x = self.offset_x + (col * char_width)
        y = self.offset_y + (row * char_height)

        xlen = char_width
        ylen = self.height
        if x + xlen > self.img.width:
            xlen = self.img.width - x

        y += self.top_crop
        

        char_img = self.img.crop((x, y, x + xlen, y + ylen))

        return char_img
    
    def get_char_range(self) -> list[int]:
        for i in range(self.char_count):
            yield self.char_start + i
    
    def get_char_data(self, index) -> list[list[int]]:
        img = self.get_char_img(index)
        cols = img.width
        rows = img.height

        col_byte_count = int((rows + 7) / 8)

        col_data = []
        for x in range(cols):
            bytes = [0] * col_byte_count
            for y in range(rows):
                byte_row = int(y / 8)
                byte_pos = y - (byte_row * 8)

                pix = img.getpixel((x,y))
                if pix < 128:
                    bytes[byte_row] |= (1 << byte_pos)
            col_data.append(bytes)
        return self.transpose(self.truncate_char(col_data))
    
    def truncate_char(self, col_data: list[list[int]]) -> list[list[int]]:
        def is_col_empty(col: list[int]):
            return all(b == 0 for b in col)
        
        xstart = 0
        xend = len(col_data) - 1
        while xstart <= xend and is_col_empty(col_data[xstart]):
            xstart += 1

        while xend >= xstart and is_col_empty(col_data[xend]):
            xend -= 1
        
        return col_data[xstart:xend + 1]
    
    def transpose(self, col_data: list[list[int]]):
        if len(col_data) == 0:
            return []
        
        row_data = []
        for r in range(len(col_data[0])):
            row_data.append([col[r] for col in col_data])
        return row_data
    
    def save_chars(self, dir: str):
        os.makedirs(dir, exist_ok=True)
        for i in self.get_char_range():
            img = self.get_char_img(i)
            dst = os.path.join(dir, f"{i}.png")
            img.save(dst)

    def get_padding(self, ch: str) -> tuple[int,int]:
        if ch in self.pad_rules:
            return self.pad_rules[ch]
        if ch == ' ':
            return self.get_space_width(), 0
        return 0,0
    
    def get_symbol_width(self, row_data):
        if len(row_data) == 0:
            return 0
        return len(row_data[0])
    
    def get_space_width(self) -> int:
        zero_char = ord('0') - self.ascii_offset
        zero_width = self.get_symbol_width(self.get_char_data(zero_char))
        return zero_width
        #return min((self.src_char_width // 2), 15), 0
    
    def set_pad(self, ch: str, prepad: int, postpad: int):
        self.pad_rules[ch] = (prepad, postpad)

    def create_c(self, path: str):
        
        os.makedirs(path, exist_ok=True)

        font_template_h = f"FONT_{self.name.upper()}_H"
        font_name = self.name
        source_name = self.name

        font_chars = ""
        font_bytes = []
        
        for i in self.get_char_range():
            data = self.get_char_data(i)
            character = chr(i + self.ascii_offset)
            pre_pad, post_pad = self.get_padding(character)
            display_character = '?' if ord(character) < 32 else character
            pad = pre_pad | (post_pad << 4)
            font_chars += f"    {'{'} .width = {self.get_symbol_width(data)}, .data_offset = {len(font_bytes)}, .pad = 0x{pad:02X} {'}'}, // '{display_character}'\n"
            for col in data:
                font_bytes += col

        font_data = ""
        for i,b in enumerate(font_bytes):
            if i % 16 == 0:
                font_data += "    "
            font_data += f"0x{b:02X}, "
            if i % 16 == 15:
                font_data += "\n"

        with open("font_template.h", 'r') as header_file:
            header_text = header_file.read()

        header_text = header_text.replace("FONT_NAME", font_name)
        header_text = header_text.replace("FONT_TEMPLATE_H", font_template_h)

        with open( os.path.join(path, f"{source_name}.h"), 'w') as header_file:
            header_file.write(header_text)
        
        with open("font_template.c", 'r') as source_file:
            source_text = source_file.read()

        source_text = source_text.replace("font_template.h", f"{source_name}.h")
        source_text = source_text.replace("FONT_NAME", font_name)
        source_text = source_text.replace("FONT_ASCII_START", str(self.ascii_offset + self.char_start))
        source_text = source_text.replace("FONT_HEIGHT", str(self.height))
        source_text = source_text.replace("FONT_DATA_INSERT", font_data)
        source_text = source_text.replace("FONT_CHAR_INSERT", font_chars)

        with open(os.path.join(path, f"{source_name}.c"), 'w') as source_file:
            source_file.write(source_text)


if __name__ == "__main__":

    DST_SRC_DIR = "../Core/fonts"

    crox4h = Font("crox4h", "sources/u8g2_font_crox4h_tr.png", 18, 24)
    crox4h.set_vcrop(5, 0)
    crox4h.create_c(DST_SRC_DIR)
    #crox4h.save_chars("test")

    crox2h = Font("crox2h", "sources/u8g2_font_crox2h_tr.png", 13, 16)
    crox2h.set_vcrop(1, 2)
    crox2h.set_pad('1', 1, 1)
    crox2h.set_pad('i', 1, 1)
    crox2h.create_c(DST_SRC_DIR)
    #crox2h.save_chars("test")

    minicute = Font("minicute", "sources/u8g2_font_minicute_tr.png", 8, 13)
    minicute.set_vcrop(1, 2)
    minicute.set_pad('1', 2, 1)
    minicute.create_c(DST_SRC_DIR)
    #minicute.save_chars("test")