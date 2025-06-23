import yaml
import os
from glob import glob
from PIL import Image, ImageOps
from math import ceil, sqrt


NES_PALETTE = open("sheets/nes.pal", "rb").read()
bg_color = (0xff,0,0xff)
#create palette image
p_img = Image.new('P', (1, 1))
p_img.putpalette(NES_PALETTE+bytearray(bg_color))
alpha_color = (0,0,0,0)

class SheetIO(object):
    def getSplitData(split):
        class Split(object):
            def __init__(self, start:int, paths):
                self.start = start
                if type(paths) == str:
                    self.paths = [paths]
                elif type(paths) == list:
                    self.paths = paths

        #single file
        if type(split) == list:
            return Split(split[0], split[1])
        #multifile
        elif type(split) == dict:
            key = list(split.keys())[0]
            return Split(key, split[key])


    def to_png(byte_data, out_path, is_sprite=False):
        out_path_no_file = os.path.dirname(out_path)
        #setup filesystem
        if not os.path.exists(out_path_no_file):
            os.makedirs(out_path_no_file)

        print(f"ripping {os.path.basename(out_path)}...")

        #####Convert
        tiles = []
        i = 0
        while i < len(byte_data):
            #the new tile
            tile = Image.new("RGBA", (8,8))

            #2bpp to palette ids
            lo = byte_data[i:i+8]
            hi = byte_data[i+8:i+0x10]
            ids = []
            for byte in range(8):
                for bit in range(8):
                    hib = (hi[byte] >> (7 - bit)) & 1
                    lob = (lo[byte] >> (7 - bit)) & 1
                    ids.append((hib << 1) | lob)

            #place down colors
            for y in range(8):
                for x in range(8):
                    #get the value of this pixel
                    getId = ids[x+(y*8)]

                    #get the palette of this tile
                    this_palette = [-1, 0xF, 0x00, 0x30] if is_sprite else [0xF, 0x00, 0x10, 0x30]

                    #get the color out of the palette
                    color = this_palette[getId]
                    #if color is alpha
                    if color == -1:
                        tile.putpixel((x, y), alpha_color)
                        continue
                    #else, put palette file color
                    tile.putpixel((x, y), tuple(NES_PALETTE[color*3: (color+1)*3]))
            #add tile to array
            tiles.append(tile)
            i += 0x10

        #construct the image with the tiles created

        #generic padding tile
        padTile = Image.new("RGBA", (8,8))
        padTile.paste(alpha_color, (0, 0, 8, 8))

        #construct a tilemap image from only unique tiles
        #square image
        newWidth = 0x10
        newHeight = (len(byte_data)//newWidth)//0x10
        newSize = (newWidth, newHeight)
        newImage = Image.new("RGBA", (newSize[0]*8, newSize[1]*8))

        #iteerate through all the tiles
        for y in range(newHeight):
            for x in range(newWidth):
                i = (y*newWidth)+x
                tiles
                #make rect
                a = (x*8, y*8, (x+1)*8, (y+1)*8)
                #paste :D
                newImage.paste(tiles[i], a)

        newImage.convert("RGBA")
        newImage.save(f"{out_path}")

    def to_binary(png, out_path, is_sprite=False):
        out_path_no_file = os.path.dirname(out_path)
        #setup filesystem
        if not os.path.exists(out_path_no_file):
            os.makedirs(out_path_no_file)

        #make a generic bytearray
        print(f"compiling file {png}...")
        #get the split data for this file

        #get the image path
        image_path = png

        #make a new image to compile
        newImage = SheetIO.QuantitizeImage(Image.open(image_path))

        newImage = SheetIO.PngToTilePng_real(newImage, 'sprite' if is_sprite else '')

        #get the bytes from an image
        ####PngTo2bpp
        file_bytes = bytearray()
        this_palette = [-1, 0xF, 0x00, 0x30] if is_sprite else [0xF, 0x00, 0x10, 0x30]

        for y in range(newImage.size[1] // 8):
            for x in range(newImage.size[0] // 8):
                #get 8x8 slice
                a = (x*8, y*8, (x+1)*8, (y+1)*8)
                cropped_img:Image = newImage.crop(a)

                #check if alpha
                #skip if so
                lohi = cropped_img.getcolors(maxcolors=4)
                if len(lohi) == 1 and lohi[0][1] == (0,0,0,0):
                    #pad with lo
                    for i in range(16):
                        file_bytes.append(0)
                    continue

                #get all unique colors and make a similar palette to ram
                unique_colors = cropped_img.getcolors(maxcolors=4)
                paletted_colors = []
                for color in unique_colors:
                    paletted_colors.append(SheetIO.pixel_to_id(color[1]))
                paletted_colors.sort()

                #check if that palette exists
                #if not, throw it in the assumed_palettes list
                assumed_set = this_palette

                lo = bytearray() #lo bitplane
                hi = bytearray() #hi bitplane
                for pY in range(8):
                    #get row
                    colorIds = []
                    for pX in range(8):
                        color = cropped_img.getpixel((pX, pY))
                        id = SheetIO.pixel_to_id(color)
                        colorIds.append(assumed_set.index(id))

                    #lo plane
                    byte = 0
                    for i in range(8):
                        byte |= (colorIds[i] & 0b00000001) << (7 - i)
                    lo.append(byte)

                    #hi plane
                    byte = 0
                    for i in range(8):
                        byte |= ((colorIds[i] & 0b00000010) >> 1) << (7 - i)
                    hi.append(byte)
                #write row
                file_bytes += lo+hi

        open(out_path, "wb").write(file_bytes)

    #function to optimize an image as much as possible.
    def PngToTilePng_real(in_image:Image, type):
        #PIL.Images
        image_tiles = []

        for y in range(in_image.size[1] // 8):
            for x in range(in_image.size[0] // 8):
                #get 8x8 slice
                a = (x*8, y*8, (x+1)*8, (y+1)*8)
                cropped_img = in_image.crop(a)

                image_tiles.append(cropped_img)

        #construct a tilemap image from only unique tiles
        newSize = (in_image.size[0]//8,in_image.size[1]//8)
        newWidth, newHeight = newSize
        newImage = Image.new("RGBA", in_image.size)

        #iterate all images
        for y in range(newHeight):
            for x in range(newWidth):
                i = x + (y * newWidth)
                tile = image_tiles[i]

                a = (x*8, y*8, (x+1)*8, (y+1)*8)
                newImage.paste(tile, a)

                i += 1
        return newImage



    def decompile(byte_data, data_loaded, out_path):
        types = data_loaded["type"]
        assumed_palettes = data_loaded["assumed_palettes"]
        splits = data_loaded["data"]
        binary_files = {}

        #get image binaries
        for i in range(len(splits)):
            split = SheetIO.getSplitData(splits[i])
            start = split.start
            end = 0
            if i < len(splits)-1:
                end = SheetIO.getSplitData(splits[i+1]).start
            else:
                end = len(byte_data)
            split_bytes = byte_data[start:end]
            for file in split.paths:
                if not file in list(binary_files.keys()):
                    binary_files[file] = split_bytes
                else:
                    binary_files[file] += split_bytes

        #setup filesystem
        if not os.path.exists(out_path):
            os.makedirs(out_path)

        #do the thang
        for file in list(binary_files.keys()):
            #get bytedata
            data = binary_files[file]
            use_style = {}
            for style in data_loaded["styling"]:
                if style["name"] == file:
                    use_style = style
                    break

            print(f"ripping {file}...")

            #####Convert
            tiles = []
            tile_order = []
            if "tile_order" in list(use_style.keys()):
                tile_order = use_style["tile_order"]
            use_palettes = None
            if "use_palettes" in list(use_style.keys()):
                use_palettes = use_style["use_palettes"]

            i = 0
            while i < len(data):
                #the new tile
                tile = Image.new("RGBA", (8,8))

                #2bpp to palette ids
                lo = data[i:i+8]
                hi = data[i+8:i+0x10]
                ids = []
                for byte in range(8):
                    for bit in range(8):
                        hib = (hi[byte] >> (7 - bit)) & 1
                        lob = (lo[byte] >> (7 - bit)) & 1
                        ids.append((hib << 1) | lob)

                #place down colors
                for y in range(8):
                    for x in range(8):
                        #get the value of this pixel
                        getId = ids[x+(y*8)]

                        #get the palette of this tile
                        this_palette = []
                        ##multi entry - different palettes per tile
                        if type(use_palettes) == list:
                            this_palette = assumed_palettes[use_palettes[i // 0x10]]
                        ##single entry - the same palette every tile
                        elif type(use_palettes) == int:
                            this_palette = assumed_palettes[use_palettes]

                        #get the color out of the palette
                        color = this_palette[getId]
                        #if color is alpha
                        if color == -1:
                            tile.putpixel((x, y), alpha_color)
                            continue
                        #else, put palette file color
                        tile.putpixel((x, y), tuple(NES_PALETTE[color*3: (color+1)*3]))
                #add tile to array
                tiles.append(tile)
                i += 0x10

            #construct the image with the tiles created

            #generic padding tile
            padTile = Image.new("RGBA", (8,8))
            padTile.paste(alpha_color, (0, 0, 8, 8))

            #if no tile order specified
            if tile_order == []:
                #construct a tilemap image from only unique tiles
                #square image
                newWidth = ceil(sqrt(len(tiles)))
                newHeight = newWidth
                newSize = (newWidth, newHeight)
                newImage = Image.new("RGBA", (newSize[0]*8, newSize[1]*8))

                #iteerate through all the tiles
                i = 0
                while i < newSize[0]*newSize[1]:
                    #if outside unique tiles, pad until finished
                    isPadding = i >= len(tiles)
                    if not isPadding:
                        tile = tiles[i]
                    else:
                        tile = padTile.copy()

                    #get coords
                    x = i % newSize[0]
                    y = i // newSize[0]
                    #make rect
                    a = (x*8, y*8, (x+1)*8, (y+1)*8)
                    #paste :D
                    newImage.paste(tile, a)

                    #end early if uhhhhh
                    if x == newSize[0]-1 and isPadding:
                        a = (0, 0, a[2], a[3])
                        newImage = newImage.crop(a)
                        break
                    i += 1
            #else, construct image from tilemap
            else:
                #get size based on tile contents
                newWidth = len(tile_order[0])
                newHeight = len(tile_order)
                newSize = (newWidth, newHeight)
                newImage = Image.new("RGBA", (newSize[0]*8, newSize[1]*8))

                #iterate through all ids
                i = 0
                while i < newSize[0]*newSize[1]:
                    #get coords
                    x = i % newSize[0]
                    y = i // newSize[0]
                    #make rect
                    a = [x*8, y*8, (x+1)*8, (y+1)*8]

                    #get the tile id
                    tileId = tile_order[y][x]
                    #'b' is the designated blank tile. used for padding
                    isPadding = tileId == 'b'

                    #boolean to flip a tile horizontally or not
                    isFlipped = False
                    if tileId == "-":
                        #because 0 doesn't have a negative, this is an override.
                        tileId = 0
                        isFlipped = True
                    elif tileId != "b":
                        isFlipped = tileId < 0

                    #get proper tile contents
                    if isPadding:
                        tile = padTile.copy()
                    else:
                        tile = tiles[abs(tileId)]
                        if isFlipped:
                            tile = ImageOps.mirror(tile)

                    #get image contents in that position
                    #composite over that, then paste
                    #this allows for alpha layering. shouldnt be needed
                    #but JUST IN CASE!!!
                    newImage.paste(Image.alpha_composite(newImage.crop(a), tile), a)
                    i += 1

            newImage.convert("RGBA")
            newImage.save(f"{out_path}{file}.png")

    def QuantitizeImage(image:Image):
        #make all alpha the rgb bg color
        fg = image.convert("RGBA")
        for y in range(fg.size[1]):
            for x in range(fg.size[0]):
                pixel = fg.getpixel((x, y))
                if pixel == (0,0,0,0):
                    fg.putpixel((x, y), bg_color)

        #convert image to nes palette (plus arbitrary bg)
        fg = fg.convert("RGB").quantize(palette=p_img, dither=0)
        fg.info["transparency"] = len(NES_PALETTE) // 3

        #convert back with arbitrary color back to alpha
        fg = image.convert("RGBA")
        #for y in range(fg.size[1]):
        #    for x in range(fg.size[0]):
        #        pixel = fg.getpixel((x, y))
        #        if pixel == bg_color:
        #            fg.putpixel((x, y), (0,0,0,0))
        return fg

    #assumes rgba
    def pixel_to_id(color):
        #0xf is also the default transparent
        #lets use -1 for a paletted alpha
        if color in [bg_color, (0,0,0,0)]:
            return -1
        #get only rgb
        b = bytes(color[:-1])
        #get id
        id = NES_PALETTE.find(b) // 3
        #0xf is the default black
        if b == bytes([0, 0, 0]):
            id = 0xf
        #0x30 is the default white. there are multiple
        elif b == bytes([0xff, 0xfe, 0xff]):
            id = 0x30
        return id

    #function to optimize an image as much as possible.
    def PngToTilePng(in_image:Image, type):
        #bytes
        tiles = []
        #PIL.Images
        image_tiles = []

        for y in range(in_image.size[1] // 8):
            for x in range(in_image.size[0] // 8):
                #get 8x8 slice
                a = (x*8, y*8, (x+1)*8, (y+1)*8)
                cropped_img = in_image.crop(a)

                #get pixel data
                data = cropped_img.tobytes()

                #if data verbatim already exists (optimization)
                exists = data in tiles

                #sprite only
                exists_flipped_h = False
                if type == "sprite":
                    data2 = ImageOps.mirror(cropped_img).tobytes()
                    exists_flipped_h = data2 in tiles

                #check if already exists
                if not (exists or exists_flipped_h):
                    tiles.append(data)
                    image_tiles.append(cropped_img)

        #generic padding tile
        padTile = Image.new("RGBA", (8,8))
        padTile.paste( (0,0,0,0), (0, 0, 8, 8))

        #construct a tilemap image from only unique tiles
        newWidth = ceil(sqrt(len(image_tiles)))
        newHeight = newWidth
        newSize = (newWidth, newHeight)
        newImage = Image.new("RGBA", (newSize[0]*8, newSize[1]*8))
        print(len(image_tiles))

        #iterate all images
        i = 0
        while i < newSize[0]*newSize[1]:
            isPadding = i >= len(image_tiles)
            tile = None
            if not isPadding:
                tile = image_tiles[i]
            else:
                tile = padTile.copy()

            x = i % newSize[0]
            y = i // newSize[0]
            a = (x*8, y*8, (x+1)*8, (y+1)*8)
            newImage.paste(tile, a)

            if x == newSize[0]-1 and isPadding:
                a = (0, 0, a[2], a[3])
                newImage = newImage.crop(a)
                break
            i += 1
        return newImage

    def PngToTileMappedPng(image, tile_order):
        #use this info to get the 'unique' tiles defined
        #this way, no tile can be interpreted as a duplicate of the other

        #get all unique tiles
        unique_tiles = {}
        for y in range(len(tile_order)):
            for x in range(len(tile_order[y])):
                isflipped = False

                #get tile
                tile = tile_order[y][x]
                if tile == "b":
                    #is blank
                    continue
                else:
                    #EDGE CASE
                    #if the first entry of this tile is flipped, note that as a boolean
                    #this way when the image is retrieved it is flipped to turn it into the normal version

                    if tile == "-":
                        #is 0 flipped
                        tile = 0
                        isflipped = True
                    #is negative
                    elif tile < 0:
                        isflipped = True

                    tile = abs(tile)

                if tile in list(unique_tiles.keys()):
                    continue

                a = (x*8, y*8, (x+1)*8, (y+1)*8)
                unique_tiles[tile] = image.crop(a)
                if isflipped:
                    unique_tiles[tile] = ImageOps.mirror(unique_tiles[tile])

        #sort them back into their "original order"
        #or, if this is homebrew, just their intended chr order
        unique_tiles_sorted = list(unique_tiles.keys())
        unique_tiles_sorted.sort()

        #square image
        newWidth = ceil(sqrt(len(unique_tiles_sorted)))
        newHeight = newWidth
        newImage = Image.new("RGBA", (newWidth*8, newHeight*8))

        #paste them into a new image that are just those
        for i in range(len(unique_tiles_sorted)):
            s = unique_tiles[unique_tiles_sorted[i]]
            x = i % newWidth
            y = i // newWidth
            a = (x*8, y*8, (x+1)*8, (y+1)*8)
            newImage.paste(s, a)
        return newImage

    def compile(data_loaded, in_folder):
        #make sure sheet_dump/ exists
        if not os.path.exists(in_folder):
            print("you have not ripped any files!")
            exit(hash("bad"))

        splits = data_loaded["data"]
        split_s = {}

        #get all split data
        for i in range(len(splits)):
            split = SheetIO.getSplitData(splits[i])
            start = split.start
            end = 0

            if i < len(splits)-1:
                end = SheetIO.getSplitData(splits[i+1]).start
            else:
                end = data_loaded["size"]

            for path in split.paths:
                if not path in (split_s.keys()):
                    split_s[path] = [[start, end]]
                else:
                    split_s[path].append([start, end])

        #make a generic bytearray
        sheet_bytes = bytearray(data_loaded["size"])

        #lets do this
        for file in list(split_s.keys()):
            print(f"compiling file {file}...")
            #get the split data for this file
            data = split_s[file]

            use_style = {}
            for style in data_loaded["styling"]:
                if style["name"] == file:
                    use_style = style
                    break
            if use_style == {}:
                continue

            #get the image path
            image_path = f"{in_folder}{file}.png"

            #make a new image to compile
            newImage = SheetIO.QuantitizeImage(Image.open(image_path))

            #if tile_order defined
            if "tile_order" in list(use_style.keys()):
                #congrats! we now have what is basically just an unclean rip from the game
                newImage = SheetIO.PngToTileMappedPng(newImage, use_style["tile_order"])
            else:
                #procedurally make an optimized tile image
                newImage = SheetIO.PngToTilePng(newImage, data_loaded["type"])

            #get the bytes from an image
            ####PngTo2bpp
            file_bytes = bytearray()

            for y in range(newImage.size[1] // 8):
                for x in range(newImage.size[0] // 8):
                    #get 8x8 slice
                    a = (x*8, y*8, (x+1)*8, (y+1)*8)
                    cropped_img:Image = newImage.crop(a)

                    #check if alpha
                    #skip if so
                    lohi = cropped_img.getcolors(maxcolors=4)
                    if len(lohi) == 1 and lohi[0][1] == (0,0,0,0):
                        #pad with lo
                        for i in range(16):
                            file_bytes.append(0)
                        continue

                    #get all unique colors and make a similar palette to ram
                    unique_colors = cropped_img.getcolors(maxcolors=4)
                    paletted_colors = []
                    for color in unique_colors:
                        paletted_colors.append(SheetIO.pixel_to_id(color[1]))
                    paletted_colors.sort()

                    #check if that palette exists
                    #if not, throw it in the assumed_palettes list
                    assumed_set = []
                    if type(style["use_palettes"]) == list:
                        ab = (newImage.size[0] // 8) * y
                        ab += x
                        get = style["use_palettes"][ab]
                        assumed_set = data_loaded["assumed_palettes"][get]
                    elif type(style["use_palettes"]) == int:
                        assumed_set = data_loaded["assumed_palettes"][style["use_palettes"]]

                    lo = bytearray() #lo bitplane
                    hi = bytearray() #hi bitplane
                    for pY in range(8):
                        #get row
                        colorIds = []
                        for pX in range(8):
                            color = cropped_img.getpixel((pX, pY))
                            id = SheetIO.pixel_to_id(color)
                            colorIds.append(assumed_set.index(id))

                        #lo plane
                        byte = 0
                        for i in range(8):
                            byte |= (colorIds[i] & 0b00000001) << (7 - i)
                        lo.append(byte)

                        #hi plane
                        byte = 0
                        for i in range(8):
                            byte |= ((colorIds[i] & 0b00000010) >> 1) << (7 - i)
                        hi.append(byte)
                    #write row
                    file_bytes += lo+hi

            #####
            at_byte = 0
            for split in data:
                length = split[1]-split[0]
                sheet_bytes[split[0]:split[1]] = file_bytes[at_byte:at_byte+length]
                at_byte += length

        return sheet_bytes


def Extract_o():
    for sheet_yaml in glob("sheets/*.yaml"):
        general_data = yaml.safe_load(open(sheet_yaml, "r"))
        general_name = general_data["path"]
        base_name = os.path.basename(sheet_yaml).removesuffix(".yaml")

        byte_data = open(f"split/{general_name}/{base_name}.bin", "rb").read()
        SheetIO.decompile(byte_data, general_data, f"extract/{general_name}/")

def Extract():
    sprite = [
        "characters1",
        "characters2",
        "characters3",
        "characters4",
        "characters5",
        "characters6",
        "characters7",
        "characters8",
        "characters9",
        "characters10",
        "characters11",
        "characters12",
        "credits_characters1",
        "credits_characters2",
        "credits_characters3",
        "title_earth",
    ]
    for sheet in glob("split/graphics/*.bin"):
        base_name = os.path.basename(sheet).replace(".bin", "", 1)
        general_name = "graphics"
        is_sprite = base_name in sprite
        byte_data = open(sheet, "rb").read()
        SheetIO.to_png(byte_data, f"extract/{general_name}/{base_name}.png", is_sprite)

    for sheet in glob("split/battle/graphics/*.bin"):
        base_name = os.path.basename(sheet).replace(".bin", "", 1)
        general_name = "battle/graphics"
        byte_data = open(sheet, "rb").read()
        SheetIO.to_png(byte_data, f"extract/{general_name}/{base_name}.png", False)

def Recompile():
    sprite = [
        "characters1",
        "characters2",
        "characters3",
        "characters4",
        "characters5",
        "characters6",
        "characters7",
        "characters8",
        "characters9",
        "characters10",
        "characters11",
        "characters12",
        "credits_characters1",
        "credits_characters2",
        "credits_characters3",
    ]
    for sheet in glob("extract/graphics/*.png"):
        base_name = os.path.basename(sheet).removesuffix(".png")
        general_name = "graphics"
        is_sprite = base_name in sprite
        SheetIO.to_binary(sheet, f"recompile/{general_name}/{base_name}.bin", is_sprite)

    for sheet in glob("extract/battle/graphics/*.png"):
        base_name = os.path.basename(sheet).removesuffix(".png")
        general_name = "battle/graphics"
        SheetIO.to_binary(sheet, f"recompile/{general_name}/{base_name}.bin", False)
