from PIL import Image
import png
from glob import glob
import zlib
import subprocess

sprite_palette = bytearray([0x00,0x00,0x00, 0x00,0x00,0x00, 0x66,0x66,0x66, 0xFF,0xFE, 0xFF])

def fix_sprite_image(path):
    # Open the existing PNG file
    reader = png.Reader(open(path, 'rb'))
    chunks = list(reader.chunks())
    width = 0
    height = 0

    # Find and modify the tRNS chunk
    pre_plt = None
    plt = None
    for i, (chunk_type, chunk_data) in enumerate(chunks):
        #set up palettes
        if chunk_type == b'PLTE':
            # Assuming a paletted image (color type 3)
            # Modify the alpha values in chunk_data (e.g., make the first palette entry fully transparent)
            modified_trns_data = bytearray(chunk_data)
            to_tuple = []
            s = 0
            while s < len(modified_trns_data):
                to_tuple.append((modified_trns_data[s], modified_trns_data[s+1], modified_trns_data[s+2]))
                s += 3

            pre_plt = to_tuple.copy()

            to_tuple = []
            s = 0
            while s < len(sprite_palette):
                to_tuple.append((sprite_palette[s], sprite_palette[s+1], sprite_palette[s+2]))
                s += 3

            plt = to_tuple
            chunks[i] = (b'PLTE', bytes(sprite_palette))
        if chunk_type == b'tRNS':
            chunks[i] = (b'tRNS', bytes(bytearray([0,0xff,0xff,0xff])))
        if chunk_type == b"IHDR":
            width = int.from_bytes(bytearray(chunk_data[0:4]), 'big')
            height = int.from_bytes(bytearray(chunk_data[4:8]), 'big')
        if chunk_type == b'IDAT':
            is_this_real = bytearray(zlib.decompress(chunk_data))
            pc = 1
            for row in range(height):
                data = is_this_real[pc:pc+(width//2)]
                #print(data.hex())
                for x in range(len(data)):
                    value = (data[x] & 0xF0) >> 4
                    alphaskip = False
                    if value == 0:
                        alphaskip = True
                    if not alphaskip:
                        get_thing = pre_plt[value]
                        data[x] &= 0x0F
                        data[x] |= plt.index(get_thing, 1) << 4

                    value = data[x] & 0x0F
                    if value == 0:
                        continue
                    get_thing = pre_plt[value]
                    data[x] &= 0xF0
                    data[x] |= plt.index(get_thing, 1)
                is_this_real[pc:pc+(width//2)] = data
                pc += (width//2) + 1
            chunks[i] = (b'IDAT', bytes(zlib.compress(is_this_real)))
            pass

    # Write the modified PNG to a new file
    with open(path, 'wb') as f:
        f.write(b'\x89PNG\r\n\x1a\n') #header
        for i, (chunk_type, chunk_data) in enumerate(chunks):
            png.write_chunk(f, chunk_type, chunk_data)


if __name__ == "__main__":
    import shutil
    import os
    if not os.path.exists("image/"):
        os.mkdir("image/")

    for image in glob("ripper/extract/graphics/**/*.png", recursive=True):
        shutil.copy(image, "image/"+os.path.basename(image))

    for image in glob("image/**/*.png", recursive=True):
        img = Image.open(image, 'r').convert("RGBA")
        alphas = [color[1] for color in img.getcolors(4)]
        if (0,0,0,0) in alphas:
            print(image+" has transparency! Modifying chunks...")
            #imagemagick can at least generate a transparency chunk AND have a palette. more than a lot can say!
            #
            #im looking at you, PIL.
            subprocess.run(f"convert {image} -colors 4 {image}", shell = True, executable="/bin/bash")
            fix_sprite_image(image)
            continue

        print(f"PIL'ing {image}...")

        paletted_img = img.convert("RGB").quantize(colors=4)
        current_palette = paletted_img.getpalette()
        current_palette.sort()

        p_img = Image.new('P', (1, 1))
        p_img.putpalette(current_palette)
        paletted_img = img.convert("RGB").quantize(colors=4, palette=p_img)
        paletted_img.save(image)