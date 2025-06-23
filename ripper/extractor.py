import os
import shutil
import yaml
import sys
from glob import glob
import sheets.SheetIO
import yamlSplit

extract = "extract/"

mem_data = yaml.safe_load(open("split.yaml", 'r'))

#$41 - $5A == A - Z (bold)
#$61 - $7A == a - z (bold)
#$b0 - $b9 == 0 - 9
#$C1 - $DA == A - Z
#$E1 - $FA == a - z

table = {
    #symbols
    0xA0: " ",
    0xA1: "!",
    0xA2: "?",
    0xA3: "..",
    0xA4: "$",
    0xA5: "•",
    0xA6: '"',
    0xA7: "'",
    0xA8: "(",
    0xA9: ")",
    0xAA: ":",
    0xAB: ";",
    0xAC: ",",
    0xAD: "-",
    0xAE: ".",
    0xAF: "/",
    0xC0: "*",
    0xFF: ">",
    0xBB: "α",
    0xBC: "β",
    0xBD: "γ",
    0xBE: "π",
    0xBF: "Ω",
}

alphabet_C_B = range(0x41, 0x5A+1) #capital (bold)
alphabet_l_B = range(0x61, 0x7A+1) #lowercase (bold)
numbers = range(0xb0, 0xb9+1)
alphabet_C = range(0xc1, 0xdA+1) #capital
alphabet_l = range(0xe1, 0xfA+1) #lowercase

#control codes
stopText = 0
newLine = 1
waitThenOverwrite = 2
pauseText = 3
t_nop = 5

SMAAAAASH = b"\x97\x98\x99\x9A\x9B\x9C\x9D\x9E\x9F"

goto = [4,"ADDR"]
#tx,ty
set_pos = [0x20,"byte","byte"]
print_string = [0x21,"ADDR"]
#ta,tb
repeatTile = [0x22,"byte","byte"]
#tb,tc
print_number = [0x23,"ADDR",'byte',"byte"]

class DialogueIO(object):
    def decompile(input:bytearray, asm=False, english=True):
        lines = []
        string = ""

        i = 0
        while i < len(input):
            val = input[i]
            if val in [stopText, newLine, waitThenOverwrite, pauseText, t_nop, goto[0], set_pos[0], print_string[0], repeatTile[0], print_number[0], SMAAAAASH[0]]:
                if val in [newLine, stopText]:
                    if val == newLine:
                        string += "\n"
                        lines.append(string)
                    #stopTexts dont depend on text and are usually singled if null
                    elif val == stopText:
                        lines.append(string+"|STOP|\n")
                        lines.append("\n")

                    string = ""
                elif val == pauseText:
                    lines.append("|PAUSE|\n")
                elif val == waitThenOverwrite:
                    lines.append("|WAIT_THEN_OVERWRITE|\n")
                elif val == t_nop:
                    lines.append("|T_NOP|\n")
                elif val == goto[0]:
                    i += 1
                    pointer = int.from_bytes(input[i:i+2], "little")
                    string += f"|goto({hex(pointer)})|"
                    i += 2-1
                elif val == set_pos[0]:
                    i += 1
                    string += f"|set_pos({input[i]},{input[i+1]})|"
                    i += 2-1
                elif val == print_string[0]:
                    i += 1
                    pointer = int.from_bytes(input[i:i+2], "little")
                    string += f"|print_string({hex(pointer)})|"
                    i += 2-1
                elif val == repeatTile[0]:
                    i += 1
                    string += f"|repeatTile({input[i]},{input[i+1]})|"
                    i += 2-1
                elif val == print_number[0]:
                    i += 1
                    pointer = int.from_bytes(input[i:i+2], "little")
                    string += f"|print_number({hex(pointer)},{input[i+2]},{input[i+3]})|"
                    i += 4-1
                elif input[i:i+9] == SMAAAAASH:
                    string+="|SMAAAAASH|"
                    i += 9-1
            elif val in alphabet_C_B:
                string += chr(ord("A")+(val-0x41))
            elif val in alphabet_C:
                string += chr(ord("A")+(val-0xc1))
            elif val in alphabet_l_B:
                string += chr(ord("a")+(val-0x61))
            elif val in alphabet_l:
                string += chr(ord("a")+(val-0xe1))
            elif val in numbers:
                string += chr(ord("0")+(val-0xb0))



            elif val in list(table.keys()):
                string += table[val]
            else:
                #unsupported
                print(hex(val))
            i += 1
        return lines
    def compile(stream):
        string = stream.readlines()
        output = bytearray()
        x = 0
        while x < len(string):
            line = string[x]
            i = 0
            while i < len(line):
                char = line[i]
                if char == "|":
                    #inline control code
                    if line[i+1:].startswith("goto"):
                        args = line[i+1:].split("goto(")[1].split(")")[0]
                        output.append(4)
                        output += int.to_bytes(int(args, 16), 2, "little")
                    elif line[i+1:].startswith("set_pos"):
                        args = line[i+1:].split("set_pos(")[1].split(")")[0].split(",")
                        output.append(0x20)
                        output.append(int(args[0]))
                        output.append(int(args[1]))
                    elif line[i+1:].startswith("print_string"):
                        args = line[i+1:].split("print_string(")[1].split(")")[0]
                        output.append(0x21)
                        output += int.to_bytes(int(args, 16), 2, "little")
                    elif line[i+1:].startswith("repeatTile"):
                        args = line[i+1:].split("repeatTile(")[1].split(")")[0].split(",")
                        output.append(0x22)
                        output.append(int(args[0]))
                        output.append(int(args[1]))
                    elif line[i+1:].startswith("print_number"):
                        args = line[i+1:].split("print_number(")[1].split(")")[0].split(",")
                        output.append(0x23)
                        output += int.to_bytes(int(args[0], 16), 2, "little")
                        output.append(int(args[1]))
                        output.append(int(args[2]))
                    elif line[i+1:].startswith("SMAAAAASH"):
                        output += SMAAAAASH
                    elif line[i+1:].startswith("STOP"):
                        output.append(stopText)
                        x += 1
                        break
                    elif line[i+1:].startswith("PAUSE"):
                        output.append(pauseText)
                        break
                    elif line[i+1:].startswith("WAIT_THEN_OVERWRITE"):
                        output.append(waitThenOverwrite)
                        break
                    elif line[i+1:].startswith("T_NOP"):
                        output.append(t_nop)
                        break


                    i = line[i+1:].find("|")+i+1
                else:
                    if char == "\n":
                        output.append(newLine)
                    else:
                        if char in list(table.values()):
                            for key in list(table.keys()):
                                if table[key] == char:
                                    output.append(key)
                                    break
                        else:
                            output.append(ord(char) + 0x80)
                i += 1
            x += 1
        return output

#basically DialogueIO but ONLY for singular strings
class NameEntryIO(object):
    def decompile(input:bytearray):
        string = ""
        i = 0
        while i < len(input):
            val = input[i]
            if val in alphabet_C_B:
                string += chr(ord("A")+(val-0x41))
            elif val in alphabet_C:
                string += chr(ord("A")+(val-0xc1))
            elif val in alphabet_l_B:
                string += chr(ord("a")+(val-0x61))
            elif val in alphabet_l:
                string += chr(ord("a")+(val-0xe1))
            elif val in numbers:
                string += chr(ord("0")+(val-0xb0))

            elif val in list(table.keys()):
                string += table[val]
            elif val == stopText:
                if i < len(input) - 1:
                    string+="|STOP|"
                else:
                    break
            else:
                #unsupported
                print(hex(val))
            i += 1
        return [string]
    def compile(stream):
        string = stream.readlines()
        if type(string) == list:
            if len(string) == 0:
                return bytearray([0])
            else:
                string = string[0]
        output = bytearray()
        i = 0
        while i < len(string):
            char = string[i]
            if char in list(table.values()):
                for key in list(table.keys()):
                    if table[key] == char:
                        output.append(key)
                        break
            elif char == "|":
                if string[i:].startswith("|STOP|"):
                    output.append(0)
                    i += len("|STOP|")-1
            elif char == "\n":
                break
            else:
                output.append(ord(char) + 0x80)
            i += 1
        output.append(0)
        return output

class TextPointersIO(object):
    def decompile(bytes):
        #TODO: THIS REQUIRES LINKING!!!
        byte_data = bytes
        pointers : list[int] = []
        i = 0
        while i < len(byte_data):
            pointers.append(int.from_bytes(byte_data[i:i+3], "little"))
            i += 3
        text_banks = mem_data["splits"]["chr"][0x10:0x1a]
        pointer_files = []
        for i in range(len(pointers)):
            use_name = ""
            if pointers[i] == 0:
                use_name = '0x{:06X}'.format(0)
            else:
                bank = pointers[i] // 0x2000
                bank_addr = pointers[i] - (bank * 0x2000)
                for entry in text_banks[bank]["splits"]:
                    if len(entry) > 1:
                        if entry[0] == bank_addr:
                            use_name = entry[1]
                            break
                if use_name == "":
                    use_name = '0x{:06X}'.format(pointers[i])
            pointer_files.append(use_name+"\n")
        return pointer_files
    def compile(stream):
        lines = stream.readlines()
        out_bytes = bytearray()
        #TODO: THIS REQUIRES LINKING!!!
        #this is a temporary bytematching solution, but the addresses would be retrived by:
        #1. compile each page
        #2. get position in page
        #3. get page mem
        #4. add
        #this currently does everything but 1. fix that!!!
        text_banks = mem_data["splits"]["chr"][0x10:0x1a]
        for line in lines:
            if line.startswith("0x"):
                out_bytes += int.to_bytes(int(line.strip(), 16), 3, "little")
                continue
            bank = int(line.replace("dialogue/", "").split("/")[0])-1
            mem = text_banks[bank]["mem"]
            for entry in text_banks[bank]["splits"]:
                if len(entry) > 1:
                    if entry[1] == line.strip():
                        mem += entry[0]
                        break
            out_bytes += int.to_bytes(mem, 3, "little")
        return out_bytes


ENEMY_FLAGS = {
    "NONE": 0,
    "SPRAYS": 1 << 0,
    "R_PK_BEAM": 1 << 1,
    "R_DEFDOWN_CONFSLEEP": 1 << 2,
    "UNK": 1 << 3,
    "R_PK_THUNDER": 1 << 4,
    "R_PK_FREEZE": 1 << 5,
    "R_PK_FIRE": 1 << 6,
    "GOD": 1 << 7
}

class BattleStatIO(object):
    def decompile(input:bytearray):
        #TODO: THIS REQUIRES LINKING!!!
        lines = []
        if len(input) != 0x20:
            print("WHAT????")
            return None

        flags = []
        for key in list(ENEMY_FLAGS.keys()):
            if input[2] & ENEMY_FLAGS[key]:
                flags.append(key)

        ething1 = int.from_bytes(input[3:5], "little")
        health =  ething1 & 0b0000001111111111
        subpal = (ething1 & 0b0000110000000000) >> 10
        gtile = (ething1 & 0b1111000000000000) >> 12
        ething2 = int.from_bytes(input[5:7], "little")
        pp =  ething2 & 0b0000001111111111
        gfx = (ething2 & 0b1111110000000000) >> 10
        ething3 = int.from_bytes(input[7:9], "little")
        offense =  ething3 & 0b0000001111111111
        final_action = (ething3 & 0b0001110000000000) >> 10
        altitude = (ething3 & 0b1110000000000000) >> 13
        ething4 = int.from_bytes(input[9:11], "little")
        defense =  ething4 & 0b0000001111111111
        message_defeat = (ething4 & 0b0001110000000000) >> 10
        unkParam = (ething4 & 0b1110000000000000) >> 13

        name_bank = mem_data["splits"]["prg"][0]
        name_addr = int.from_bytes(input[24:26], "little")
        name_file = ""
        for entry in name_bank["splits"]:
            if len(entry) > 1:
                if entry[0] == name_addr-name_bank["mem"]:
                    name_file = entry[1]
        if name_file == "":
            name_file = '0x{:04X}'.format(name_addr)

        dict = {
        "unk": input[0],
        "init_status": input[1],
        "flags": flags,

        "health": health,
        "subpal": subpal,
        "gtile": gtile,

        "pp": pp,
        "gfx": gfx,

        "offense": offense,
        "final_action": final_action,
        "altitude": altitude,

        "defense": defense,
        "message_defeat": message_defeat,
        "unkParam": unkParam,

        "fight": input[11],
        "speed": input[12],
        "wisdom": input[13],
        "strength": input[14],
        "force": input[15],

        "battle_actions": [input[16],input[17],input[18],input[19],input[20],input[21],input[22],input[23]],
        "name_pointer": name_file,
        "exp": int.from_bytes(input[26:28], "little"),
        "money": int.from_bytes(input[28:30], "little"),
        "item": input[30],
        "graphic_page": input[31]
        }

        lines = yaml.dump(dict, sort_keys=False)
        return lines
    def compile(stream):
        #TODO: THIS REQUIRES LINKING!!!
        data_loaded = yaml.safe_load(stream)

        output = bytearray()
        output.append(data_loaded["unk"])
        output.append(data_loaded["init_status"])

        flag_bit = 0
        flags = data_loaded["flags"]
        for key in flags:
            flag_bit |= ENEMY_FLAGS[key]
        output.append(flag_bit)

        output += int.to_bytes((data_loaded["gtile"] << 12) | (data_loaded["subpal"] << 10) | data_loaded["health"], 2, 'little')
        output += int.to_bytes((data_loaded["gfx"] << 10) | data_loaded["pp"], 2, "little")
        output += int.to_bytes((data_loaded["altitude"] << 13) | (data_loaded["final_action"] << 10) | data_loaded["offense"], 2, "little")
        output += int.to_bytes((data_loaded["unkParam"] << 13) | (data_loaded["message_defeat"] << 10) | data_loaded["defense"], 2, "little")

        output.append(data_loaded["fight"])
        output.append(data_loaded["speed"])
        output.append(data_loaded["wisdom"])
        output.append(data_loaded["strength"])
        output.append(data_loaded["force"])

        for entry in data_loaded["battle_actions"]:
            output.append(entry)

        name_bank = mem_data["splits"]["prg"][0]
        name_file = data_loaded["name_pointer"]
        name_addr = -1
        for entry in name_bank["splits"]:
            if len(entry) > 1:
                if entry[1] == name_file:
                    name_addr = entry[0]+name_bank["mem"]
        if name_addr == -1:
            name_addr = 0xFFFF
        output += int.to_bytes(name_addr, 2, "little")

        output += int.to_bytes(data_loaded["exp"], 2, "little")
        output += int.to_bytes(data_loaded["money"], 2, "little")
        output.append(data_loaded["item"])
        output.append(data_loaded["graphic_page"])
        return output

class ItemIO(object):
    def decompile(input:bytearray):
        #TODO: THIS REQUIRES LINKING!!!
        lines = []
        if len(input) != 8:
            print("WHAT????")
            return None

        usableBy = input[2] & 0b00111111
        edible = bool((input[2] & 0b01000000) >> 6)
        keyitem = bool((input[2] & 0b10000000) >> 7)
        effectValue = input[3] & 0b00111111
        armorType = (input[3] & 0b11000000) >> 6

        name_bank = mem_data["splits"]["prg"][0]
        name_addr = int.from_bytes(input[:2], "little")
        name_file = ""
        for entry in name_bank["splits"]:
            if len(entry) > 1:
                if entry[0] == name_addr-name_bank["mem"]:
                    name_file = entry[1]
        if name_file == "":
            name_file = '0x{:04X}'.format(name_addr)

        dict = {
        "name": name_file,
        "usableBy": usableBy,
        "edible": edible,
        "keyitem": keyitem,

        "effectValue": effectValue,
        "armorType": armorType,

        "overworldAction": input[4],
        "battleAction": input[5],

        "msgPrice": int.from_bytes(input[6:], "little")
        }

        lines = yaml.dump(dict, sort_keys=False)
        return lines
    def compile(stream):
        #TODO: THIS REQUIRES LINKING!!!
        data_loaded = yaml.safe_load(stream)

        output = bytearray()

        name_bank = mem_data["splits"]["prg"][0]
        name_file = data_loaded["name"]
        name_addr = -1
        for entry in name_bank["splits"]:
            if len(entry) > 1:
                if entry[1] == name_file:
                    name_addr = entry[0]+name_bank["mem"]
        if name_addr == -1:
            name_addr = 0xFFFF
        output += int.to_bytes(name_addr, 2, "little")

        output.append((int(data_loaded["keyitem"]) << 7) | (int(data_loaded["edible"]) << 6) | data_loaded["usableBy"])
        output.append((data_loaded["armorType"] << 6) | data_loaded["effectValue"])

        output.append(data_loaded["overworldAction"])
        output.append(data_loaded["battleAction"])

        output += int.to_bytes(data_loaded["msgPrice"], 2, "little")
        return output

class PsiIO(object):
    def decompile(input:bytearray):
        #TODO: THIS REQUIRES LINKING!!!
        lines = []
        if len(input) != 8:
            print("WHAT????")
            return None

        name_bank = mem_data["splits"]["prg"][0]
        name_addr = int.from_bytes(input[:2], "little")
        name_file = ""
        for entry in name_bank["splits"]:
            if len(entry) > 1:
                if entry[0] == name_addr-name_bank["mem"]:
                    name_file = entry[1]
        if name_file == "":
            name_file = '0x{:04X}'.format(name_addr)

        dict = {
        "name": name_file,
        "nintenLevel": input[2],
        "anaLevel": input[3],
        "overworldEffect": input[4],
        "battleEffect": input[5],
        "arg5": input[6],
        "cost": input[7],
        }

        lines = yaml.dump(dict, sort_keys=False)
        return lines
    def compile(stream):
        #TODO: THIS REQUIRES LINKING!!!
        data_loaded = yaml.safe_load(stream)

        output = bytearray()

        name_bank = mem_data["splits"]["prg"][0]
        name_file = data_loaded["name"]
        name_addr = -1
        for entry in name_bank["splits"]:
            if len(entry) > 1:
                if entry[1] == name_file:
                    name_addr = entry[0]+name_bank["mem"]
        if name_addr == -1:
            name_addr = 0xFFFF
        output += int.to_bytes(name_addr, 2, "little")

        output.append(data_loaded["nintenLevel"])
        output.append(data_loaded["anaLevel"])
        output.append(data_loaded["overworldEffect"])
        output.append(data_loaded["battleEffect"])
        output.append(data_loaded["arg5"])
        output.append(data_loaded["cost"])

        return output

class ChoicerIO(object):
    def decompile(input:bytearray):
        #TODO: THIS REQUIRES LINKING!!!
        lines = []
        if len(input) != 8:
            print("WHAT????")
            return None


        name_bank = mem_data["splits"]["prg"][0]
        name_addr = int.from_bytes(input[:2], "little")
        name_file = ""
        for entry in name_bank["splits"]:
            if len(entry) > 1:
                if entry[0] == name_addr-name_bank["mem"]:
                    name_file = entry[1]
        if name_file == "":
            name_file = '0x{:04X}'.format(name_addr)

        ething1 = int.from_bytes(input[2:4], "little")
        music = ething1 & 0b0000000000111111
        posX = (ething1 & 0b1111111111000000) >> 6
        ething2 = int.from_bytes(input[4:6], "little")
        direction = ething2 & 0b0000000000111111
        posY = (ething2 & 0b1111111111000000) >> 6

        dict = {
        "name": name_file,
        "music": music,
        "posX": posX,
        "direction": direction,
        "posY": posY,
        "msgPrice": int.from_bytes(input[6:], "little")
        }

        lines = yaml.dump(dict, sort_keys=False)
        return lines
    def compile(stream):
        #TODO: THIS REQUIRES LINKING!!!
        data_loaded = yaml.safe_load(stream)

        output = bytearray()

        name_bank = mem_data["splits"]["prg"][0]
        name_file = data_loaded["name"]
        name_addr = -1
        for entry in name_bank["splits"]:
            if len(entry) > 1:
                if entry[1] == name_file:
                    name_addr = entry[0]+name_bank["mem"]
        if name_addr == -1:
            name_addr = 0xFFFF
        output += int.to_bytes(name_addr, 2, "little")

        output += int.to_bytes((int(data_loaded["posX"]) << 6) | data_loaded["music"], 2, 'little')
        output += int.to_bytes((int(data_loaded["posY"]) << 6) | data_loaded["direction"], 2, 'little')

        output += int.to_bytes(data_loaded["msgPrice"], 2, "little")
        return output

class BattleIO(object):
    def decompile(input:bytearray):
        #TODO: THIS REQUIRES LINKING!!!
        if len(input) != 0xA:
            print("WHAT????")
            return None

        enemies = []
        i = 0
        while i < 8:
            enemy_bank = mem_data["splits"]["prg"][0x16]
            enemy_id = input[i]
            enemy_label = input[i+1]
            file = ""
            if enemy_id != 0xFF:
                entry = enemy_bank["splits"][enemy_id]
                if len(entry) > 1:
                    file = entry[1]
                if file == "":
                    file = '0x{:01X}'.format(enemy_id)
            else:
                file = "NONE"
            enemies.append({"enemy": file, "label": enemy_label})
            i += 2

        position = input[8] & 0b00011111
        encounter = (input[8] & 0b11100000) >> 5
        palette = input[9] & 0b00001111
        music = (input[9] & 0b11110000) >> 4

        dict = {
        "enemies": enemies,
        "position": position,
        "encounter": encounter,
        "palette": palette,
        "music": music,
        }

        lines = yaml.dump(dict, sort_keys=False)
        return lines
    def compile(stream):
        #TODO: THIS REQUIRES LINKING!!!
        data_loaded = yaml.safe_load(stream)

        output = bytearray()

        enemies = data_loaded["enemies"]
        for enemy in enemies:
            enemy_file = enemy["enemy"]
            enemy_label = enemy["label"]
            enemy_bank = mem_data["splits"]["prg"][0x16]
            enemy_id = -1
            if enemy_file != "NONE":
                for i in range(len(enemy_bank["splits"])):
                    entry = enemy_bank["splits"][i]
                    if len(entry) > 1:
                        if entry[1] == enemy_file:
                            enemy_id = i
                            break
            if enemy_id == -1:
                enemy_id = 0xFF
            output.append(enemy_id)
            output.append(enemy_label)

        output.append((int(data_loaded["encounter"]) << 5) | data_loaded["position"])
        output.append((int(data_loaded["music"]) << 4) | data_loaded["palette"])
        return output

class SpriteTileIO(object):
    def decompile(input:bytearray):
        if len(input) < 4:
            print("WHAT????")
            return None
        if len(input) % 4 != 0:
            print("WHAT????")
            return None

        tiles = []
        i = 0
        while i < len(input):
            tile_data = input[i:i+4]

            palette = tile_data[2] & 0b00000011
            unkArg1 = (tile_data[2] & 0b00011100) >> 2
            order = (tile_data[2] & 0b00100000) >> 5
            flipX = (tile_data[2] & 0b01000000) >> 6
            flipY = (tile_data[2] & 0b10000000) >> 7

            dict = {
            "posX": tile_data[0],
            "posY": tile_data[1],
            "palette": palette,
            "unkArg1": unkArg1,
            "order": order,
            "flipX": flipX,
            "flipY": flipY,
            "index": tile_data[3],
            }

            i += 4
            tiles.append(dict)

        lines = yaml.dump(tiles, sort_keys=False)
        return lines
    def compile(stream):
        data_loaded = yaml.safe_load(stream)
        output = bytearray()
        for tile in data_loaded:
            output.append(tile["posX"])
            output.append(tile["posY"])
            output.append((tile["flipY"] << 7) | (tile["flipX"] << 6) | (tile["order"] << 5) | (tile["unkArg1"] << 2) | tile["palette"])
            output.append(tile["index"])
        return output

class SpriteDefinitionIO(object):
    def decompile(input:bytearray):
        #TODO: THIS REQUIRES LINKING!!!
        lines = []
        if len(input) % 4 != 0:
            print("WHAT????")
            return None

        defs = []
        i = 0
        while i < len(input):
            def_data = input[i:i+4]

            sprite_bank = mem_data["splits"]["prg"][0x15]
            tile_addr = int.from_bytes(def_data[:2], "little")
            tile_file = ""
            for entry in sprite_bank["splits"]:
                if len(entry) > 1:
                    if entry[0] == tile_addr-sprite_bank["mem"]:
                        tile_file = entry[1]
            if tile_file == "":
                tile_file = '0x{:04X}'.format(tile_addr)

            p1 = def_data[3] & 0b00000011
            p2 = (def_data[3] & 0b00001100) >> 2
            unkArg1 = (def_data[3] & 0b00010000) >> 4
            unkArg2 = (def_data[3] & 0b11100000) >> 5

            dict = {
            "tiles": tile_file,
            "ppu": def_data[2],
            "p1": p1,
            "p2": p2,
            "unkArg1": unkArg1,
            "unkArg2": unkArg2
            }

            i += 4
            defs.append(dict)

        return yaml.dump(defs, sort_keys=False)
    def compile(stream):
        #TODO: THIS REQUIRES LINKING!!!
        data_loaded = yaml.safe_load(stream)
        output = bytearray()
        for definition in data_loaded:
            sprite_bank = mem_data["splits"]["prg"][0x15]
            tile_file = definition["tiles"]
            tile_addr = -1
            for entry in sprite_bank["splits"]:
                if len(entry) > 1:
                    if entry[1] == tile_file:
                        tile_addr = entry[0]+sprite_bank["mem"]
            if tile_addr == -1:
                tile_addr = 0xFFFF
            output += int.to_bytes(tile_addr, 2, "little")
            output.append(definition["ppu"])
            output.append((definition["unkArg2"] << 5) | (definition["unkArg1"] << 4) | (definition["p2"] << 2) | definition["p1"])
        return output


class MapIO(object):

    color_map = {
        (0,0,0,255): 0,
        (102,102,102,255): 1,
        (173,173,173,255): 2,
        (255,254,255,255): 3,
    }

    def decompile(byte_data):
        #Map height * width in terms of 64x64 chunks
        mapTiles = []
        mapTileset = [] #second tileset
        mapEvent = []

        #Same as above, divided by 4
        sectorPalette = []
        sectorArea = []
        sectorTileset1 = []
        sectorTileset2 = []

        banksMap = [0x2000, 0x6000, 0xA000, 0xE000, 0x12000, 0x16000, 0x1A000]
        curBank = 0
        for offset in banksMap:
            for i in range(0x2000):
                tileOffs = offset + i #Offset of current 64x64 map data
                currentByte = byte_data[tileOffs]
                mapTiles.append(currentByte & 0b00111111); #Get lower 6 bits only of map data, store in array
                mapTileset.append((currentByte & 0b01000000) != 0)
                mapEvent.append((currentByte & 0b10000000) != 0)

            curBank += 1

        #Lower 6 bits of 3800-3FFF (skip 1st bank)
        banksSector = [0x5800, 0x9800, 0xD800, 0x11800, 0x15800, 0x19800, 0x1D800]
        curBank = 0
        for offset in banksSector:
            for i in range(0x200):
                tileOffs = offset + (i * 4) #Offset of current 256x256 sector data
                #First 6 bits of each byte
                sectorPalette.append(byte_data[tileOffs] & 0b00111111)
                sectorArea.append(byte_data[tileOffs+1] & 0b00111111)
                sectorTileset1.append(byte_data[tileOffs+2] & 0b00111111)
                sectorTileset2.append(byte_data[tileOffs+3] & 0b00111111)
            curBank += 1

        #Lower 6 bits of 3000-37FF
        graphics16 = []
        banks16 = [0x1000, 0x5000, 0x9000, 0xD000, 0x11000, 0x15000, 0x19000, 0x1D000]
        palettes64 = []
        curBank = 0
        for offset in banks16:
            for i in range(0x200):
                tileset = (curBank * 4) + (i // 0x80) #4 tilesets for each bank
                tileset *= 64
                tileOffs = offset + (i * 4) #Offset of current 16x16 tile data
                graphics16.append([
                    tileset + (  byte_data[tileOffs] & 0b00111111),
                    tileset + (byte_data[tileOffs+1] & 0b00111111),
                    tileset + (byte_data[tileOffs+2] & 0b00111111),
                    tileset + (byte_data[tileOffs+3] & 0b00111111),
                    i & 0b01111111
                ])
            for i in range(0x100):
                paletteOffs = offset + (i * 0x10); #Offset of current 64x64 palette data
                new_palette = []
                for j in range(0x10):
                    value = (byte_data[paletteOffs + j] & 0b11000000) >> 6
                    new_palette.append(value)
                palettes64.append(new_palette)
            curBank += 1

        #2000-2FFF
        banks64 = [0, 0x4000, 0x8000, 0xC000, 0x10000, 0x14000, 0x18000, 0x1C000]
        graphics64 = []
        curBank = 0
        for offset in banks64:
            for i in range(0x100):
                tileset = (curBank * 4) + (i // 64); #4 tilesets for each bank
                tileOffs = offset + (i * 16); #Offset of current 64x64 palette data
                curTiles = [] #Tile16's
                altTileset = []
                tileNums = []

                #Iterate through the 64x64 tile data & decode it
                for j in range(0x10):
                    subOffs = tileOffs + j
                    tileNum = byte_data[subOffs] & 0b01111111
                    #This gets the correct 16x16 tile from the loaded list (ignoring all that alternate tileset crap)
                    curTiles.append((tileset * 128) + tileNum)
                    tileNums.append(tileNum)

                    if (byte_data[subOffs] & 0b10000000): #If last bit is set, use alternate tileset
                        altTileset.append(j)

                graphics64.append({
                    "curTiles": curTiles,
                    "palette64": (curBank*0x100)+i,
                    "altTileset": altTileset,
                    "tileNums": tileNums
                })
            curBank += 1

        dict = {
            "graphics64": graphics64,
            "palettes64": palettes64,
            "graphics16": graphics16,
            "mapTiles": mapTiles,
            "mapTileset": mapTileset,
            "mapEvent": mapEvent,
            "sectorPalette": sectorPalette,
            "sectorArea": sectorArea,
            "sectorTileset1": sectorTileset1,
            "sectorTileset2": sectorTileset2
        }

        return yaml.dump(dict, sort_keys=False, default_flow_style=True)

    def compile(stream):
        data_loaded = yaml.safe_load(stream)
        #TODO: dont make it a set size. hard!
        out_bytes = bytearray(0x1e000)


        #Map height * width in terms of 64x64 chunks
        mapTiles = data_loaded["mapTiles"]
        mapTileset = data_loaded["mapTileset"] #second tileset
        mapEvent = data_loaded["mapEvent"]

        #Same as above, divided by 4
        sectorPalette = data_loaded["sectorPalette"]
        sectorArea = data_loaded["sectorArea"]
        sectorTileset1 = data_loaded["sectorTileset1"]
        sectorTileset2 = data_loaded["sectorTileset2"]

        graphics16 = data_loaded["graphics16"]
        graphics64 = data_loaded["graphics64"]
        palettes64 = data_loaded["palettes64"]

        banksMap = [0x2000, 0x6000, 0xA000, 0xE000, 0x12000, 0x16000, 0x1A000]
        curBank = 0
        array_i = 0
        for offset in banksMap:
            for i in range(0x2000):
                tileOffs = offset + i #Offset of current 64x64 map data
                currentByte = mapTiles[array_i] & 0b00111111
                currentByte |= (int(mapTileset[array_i]) & 0b1) << 6
                currentByte |= (int(mapEvent[array_i]) & 0b1) << 7
                out_bytes[tileOffs] |= currentByte

                array_i += 1

            curBank += 1

        #Lower 6 bits of 3800-3FFF (skip 1st bank)
        banksSector = [0x5800, 0x9800, 0xD800, 0x11800, 0x15800, 0x19800, 0x1D800]
        curBank = 0
        array_i = 0
        for offset in banksSector:
            for i in range(0x200):
                tileOffs = offset + (i * 4) #Offset of current 256x256 sector data
                #First 6 bits of each byte
                bytees = bytearray(4)
                bytees[0] = sectorPalette[array_i] & 0b00111111
                bytees[1] = sectorArea[array_i] & 0b00111111
                bytees[2] = sectorTileset1[array_i] & 0b00111111
                bytees[3] = sectorTileset2[array_i] & 0b00111111

                for i in range(len(bytees)):
                    out_bytes[tileOffs+i] |= bytees[i]
                array_i += 1
            curBank += 1

        #Lower 6 bits of 3000-37FF
        banks16 = [0x1000, 0x5000, 0x9000, 0xD000, 0x11000, 0x15000, 0x19000, 0x1D000]
        curBank = 0
        array_i = 0
        array_p = 0
        for offset in banks16:
            for i in range(0x200):
                tileset = (curBank * 4) + (i // 0x80) #4 tilesets for each bank
                tileset *= 64
                tileOffs = offset + (i * 4) #Offset of current 16x16 tile data

                tile1 = graphics16[array_i][0]-tileset
                tile2 = graphics16[array_i][1]-tileset
                tile3 = graphics16[array_i][2]-tileset
                tile4 = graphics16[array_i][3]-tileset
                byte_data = bytearray([tile1, tile2, tile3, tile4])

                for i in range(len(byte_data)):
                    out_bytes[tileOffs+i] |= byte_data[i]

                array_i += 1
            for i in range(0x100):
                paletteOffs = offset + (i * 0x10); #Offset of current 64x64 palette data
                data = palettes64[array_p]
                for j in range(0x10):
                    value = (data[j] & 0b11) << 6
                    out_bytes[paletteOffs+j] |= value
                array_p += 1
            curBank += 1

        #2000-2FFF
        banks64 = [0, 0x4000, 0x8000, 0xC000, 0x10000, 0x14000, 0x18000, 0x1C000]
        curBank = 0
        array_i = 0
        for offset in banks64:
            for i in range(0x100):
                tileset = (curBank * 4) + (i // 64); #4 tilesets for each bank
                tileOffs = offset + (i * 16); #Offset of current 64x64 palette data
                curTiles = [] #Tile16's

                #Iterate through the 64x64 tile data & decode it
                for j in range(0x10):
                    subOffs = tileOffs + j

                    tileNum = graphics64[array_i]["tileNums"][j]
                    out_bytes[subOffs] |= tileNum
                    #This gets the correct 16x16 tile from the loaded list (ignoring all that alternate tileset crap)
                    curTiles.append((tileset * 128) + tileNum)

                    if j in graphics64[array_i]["altTileset"]:
                        out_bytes[subOffs] |= 0b10000000

                array_i += 1
            curBank += 1

        return out_bytes

class Extractor(object):
    def Extract():
        yamlSplit.doSplit("split")
        data_loaded = yaml.safe_load(open("extract.yaml", "r"))

        out_dir = data_loaded["output"]
        if os.path.exists(out_dir):
            shutil.rmtree(out_dir)
        os.makedirs(out_dir)

        bad = []
        #get files and use their class to decompile
        for types in data_loaded["types"]:
            for to_glob in types["glob"]:
                files = glob(to_glob)
                for file in files:
                    get_class = getattr(sys.modules[__name__], types["class"])
                    print(f"decompiling {file} with {get_class.__name__}")
                    original_data = open(file, "rb").read()
                    text = get_class.decompile(original_data)
                    out_file = file.replace("\\", "/").replace(".bin", types["extension"]).replace("split/", out_dir)
                    if not os.path.exists(os.path.dirname(out_file)):
                        os.makedirs(os.path.dirname(out_file))
                    open(out_file, "w", encoding="utf-8").writelines(text)
        sheets.SheetIO.Extract()

    def Recompile():
        if os.path.exists("recompile/"):
            shutil.rmtree("recompile/")
        os.makedirs("recompile/")

        data_loaded = yaml.safe_load(open("extract.yaml", "r"))

        #get a list of recompilable files and how to recompile them
        recompilableFiles = {}
        for types in data_loaded["types"]:
            for to_glob in types["glob"]:
                files = glob(to_glob)
                for file in files:
                    get_class = getattr(sys.modules[__name__], types["class"])
                    in_file = file.replace("\\", "/").replace(".bin", types["extension"]).replace("split/", "extract/")
                    out_file = file.replace("\\", "/").replace("split/", "recompile/")
                    recompilableFiles[out_file] = {"in_file": in_file, "class": get_class}

        #images
        sheets.SheetIO.Recompile()

        #assembled output
        rom_output = bytearray()


        def get_file(file_path):
            recomp = "recompile"+file_path
            split = "split"+file_path

            recomp_results = [file for file in list(recompilableFiles.keys()) if file.startswith(recomp)]
            recomp_results += glob(f"{recomp}.*")
            recomp_exists = len(recomp_results) > 0
            output_results = glob(f"{split}.*")
            output_exists = len(output_results) > 0

            if not recomp_exists:
                if not output_exists:
                    return
                return output_results[0]
            else:
                use_thing = recomp_results[0]
                if not os.path.exists(use_thing):
                    #do
                    out_file = use_thing
                    in_file = recompilableFiles[out_file]["in_file"]
                    get_class = recompilableFiles[out_file]["class"]
                    print(f"recompiling {file} with {get_class.__name__}")
                    out_bytes = get_class.compile(open(in_file, "r", encoding="utf-8"))
                    if not os.path.exists(os.path.dirname(out_file)):
                        os.makedirs(os.path.dirname(out_file))
                    open(out_file, "wb").write(out_bytes)

                    #remove entry
                    del recompilableFiles[out_file]

                    return out_file
                elif type(use_thing) == str:
                    return use_thing.replace("\\", "/")


        structure_yaml = yaml.safe_load(open("split.yaml", 'r'))
        assembled_files = []

        for side in structure_yaml["splits"]:
            skip_banks = 0
            for bank in structure_yaml["splits"][side]:
                bank_bytes = bytearray()
                bank_number = hex(bank["bank"]).replace("0x", "")
                if "splits" in list(bank.keys()):
                    for split in bank["splits"]:
                        if len(split) > 1:
                            file = split[1]
                            gotten_file = get_file(f"/{file}")
                            if gotten_file in assembled_files:
                                continue
                            if not gotten_file in assembled_files:
                                print(f"assembling {gotten_file}....")
                                assembled_files.append(gotten_file)
                                bank_bytes += open(gotten_file, "rb").read()
                        else:
                            fakeaddr = hex(split[0]).replace("0x", "")
                            usename = f"/{side}/bank{bank_number}/unk{fakeaddr}"
                            gotten_file = get_file(usename)
                            if gotten_file in assembled_files:
                                continue
                            if not gotten_file in assembled_files:
                                print(f"assembling {gotten_file}....")
                                assembled_files.append(gotten_file)
                                bank_bytes += open(gotten_file, "rb").read()
                else:
                    usename = f"/{side}/bank{bank_number}"
                    gotten_file = get_file(usename)
                    if not gotten_file in assembled_files:
                        print(f"assembling {gotten_file}....")
                        assembled_files.append(gotten_file)
                        bank_bytes += open(gotten_file, "rb").read()
                if "end" in list(bank.keys()):
                    i_should_be_at = bank["bank"] * 0x2000
                    if side == "chr":
                        i_should_be_at += len(structure_yaml["splits"]["prg"]) * 0x2000
                    passed = len(rom_output) - i_should_be_at
                    if passed < 0:
                        passed = 0
                    #bank_count = len(bank_bytes) +
                    #if rom_output > i_should_be_at:
                    while len(bank_bytes) < 0x2000 - passed:
                        bank_bytes.append(0xFF)


                rom_output += bank_bytes

        INES_NES20 = b'\x4E\x45\x53\x1A\x10\x20\x42\x00\x00\x00\x00\x00\x00\x00\x00\x00'

        with open("mother_reconstruct.nes", "wb") as file:
            file.write(INES_NES20)
            file.write(rom_output)


if __name__ == "__main__":
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument('mode', type=str)
    args = parser.parse_args()

    if args.mode == "e":
        Extractor.Extract()
    elif args.mode == "r":
        Extractor.Recompile()