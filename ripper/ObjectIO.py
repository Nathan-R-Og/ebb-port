import yaml

obj_typeinfo = open("split/obj_typeinfo.bin", "rb").read()
obj_bank2 = open("split/objbank_2.bin", "rb").read()

class EbbTypeDef(object):
    def __init__(self, data):
        if len(data) != 4:
            return
        self.addr = int.from_bytes(data[:2], "little")
        self.tiles = data[2]
        self.is_solid = (data[3] & 0b10000000) >> 7
        self.is_interactable = (data[3] & 0b01000000) >> 6
        self.can_face = (data[3] & 0b00100000) >> 5
        self.high_priority = (data[3] & 0b00010000) >> 4
        self.offset = data[3] & 0b00001111

parse = {}
for i in range(0x2e):
    parse[i] = EbbTypeDef(obj_typeinfo[i*4:(i+1)*4])

class EbbObject(object):
    lines = []
    addr_start = -1
    def __init__(self, data, addr_start):
        self.byte_data = data
        self.addr_start = addr_start

        header = int.from_bytes(data[:2], "little")
        self.obtype = header & 0b0000000000111111
        self.posX = (header & 0b1111111111000000) >> 6

        header2 = int.from_bytes(data[2:4], "little")
        self.direction = header2 & 0b0000000000111111
        self.posY = (header2 & 0b1111111111000000) >> 6

        self.script = data[parse[self.obtype].offset:]

    def to_macros(self):
        self.lines = [f"objectDef {self.obtype}, {self.posX}, {self.direction}, {self.posY}\n"]


# .define moveDef(direction, cmd, tiles) .word (tiles << 8) | (cmd << 3) | direction

# .define objectDef(type, posX, direction, posY) .word (posX << 6) | type, (posY << 6) | direction
# ;top left of the map is 0, $80. why?
# .define doorArgDef(music, targetPosX, targetDirection, targetPosY) .word (targetPosX << 6) | music, (targetPosY << 6) | targetDirection

# ;APPARENTLY they made the teleport gaining object based??????
# ;bbbbbfff
# .define teleportFlagDef(flag, byte) .byte (byte << 3) | flag

class DoorArgDef(object):
    def __init__(self, data):
        split1 = int.from_bytes(data[:2], 'little')
        self.tele_music = split1 & 0b0000000000111111
        self.tele_x = (split1 & 0b1111111111000000) >> 6
        split2 = int.from_bytes(data[2:4], 'little')
        self.tele_dir = split2 & 0b0000000000111111
        self.tele_y = (split2 & 0b1111111111000000) >> 6

    def __str__(self):
        args_val = ', '.join([str(i) for i in [self.tele_music, self.tele_x, self.tele_dir, self.tele_y]])
        return f"{args_val}"
    def __repr__(self):
        return str(self)

#generic warp class
class EbbWarp(EbbObject):
    def __init__(self, data, addr_start):
        super().__init__(data, addr_start)
        self.warp_data = DoorArgDef(data[4:parse[self.obtype].offset])

    def to_macros(self):
        super().to_macros()
        self.lines.append(f"doorArgDef {self.warp_data.tele_music}, {self.warp_data.tele_x}, {self.warp_data.tele_dir}, {self.warp_data.tele_y}\n")

#yeah
class EbbDoor(EbbWarp):
    def __init__(self, data, addr_start):
        super().__init__(data, addr_start)
class EbbStairs(EbbWarp):
    def __init__(self, data, addr_start):
        super().__init__(data, addr_start)
class EbbHole(EbbWarp):
    def __init__(self, data, addr_start):
        super().__init__(data, addr_start)


mem_data = yaml.safe_load(open("split.yaml"))
sprite_list = mem_data["splits"]["prg"][0x15]["splits"]
sprite_files = {}
for i in range(len(sprite_list)):
    if not sprite_list[i][1].startswith("sprites/defs/"):
        continue
    sprite_files[sprite_list[i][0]+0x8000] = sprite_list[i][1]

#generic 'with sprite' class
class EbbSprite(EbbObject):
    def __init__(self, data, addr_start):
        super().__init__(data, addr_start)
        self.sprite_addr = int.from_bytes(data[4:parse[self.obtype].offset], "little")
        self.sprite = sprite_files[self.sprite_addr]

    def to_macros(self):
        super().to_macros()
        self.lines.append(f".addr {self.sprite}\n")


class EbbScript(object):
    def __init__(self, byte_data):
        #optionals
        if not hasattr(self, "macro_args"):
            self.macro_args = []
        if not hasattr(self, "args"):
            self.args = []

        #do evaluate
        i = 0
        x = 0
        while i < len(byte_data):
            if i == 0: #control code
                i += 1
                continue
            if x > len(self.macro_args):
                break

            arg = self.macro_args[x]
            if arg in ['oaddr', 'byte']: #object address (zp (basically))
                self.args.append(byte_data[i])
                i += 1
            elif arg in ['addr', 'word']: #ram address
                word = int.from_bytes(byte_data[i:i+2], 'little')
                self.args.append(word)
                i += 2
            elif arg in ['doorArgDef']: #ram address
                self.args.append(DoorArgDef(byte_data[i:i+4]))
                i += 4


            x += 1

    def __str__(self):
        args_val = ', '.join([str(i) for i in self.args])
        return f"{self.legible_name}({args_val})"
    def __repr__(self):
        return str(self)

class FUNC_STOP(EbbScript):
    id = 0
    scr_len = 1
    macro = "OBJ_STOP"
    legible_name = "stop"
class FUNC_JUMP(EbbScript):
    id = 1
    scr_len = 1+1
    macro = "OBJ_JUMP"
    macro_args = ['oaddr']
    legible_name = "jump"
class FUNC_SUBROUTINE(EbbScript):
    id = 2
    scr_len = 1+2+1
    macro = "OBJ_SUBROUTINE"
    macro_args = ['addr', 'byte']
    legible_name = "run_subroutine"
class FUNC_RETURN(EbbScript):
    id = 3
    scr_len = 1
    macro = "OBJ_RETURN"
    legible_name = "escape_subroutine"
class FUNC_DELAY(EbbScript):
    id = 4
    scr_len = 1+1
    macro = "OBJ_DELAY"
    macro_args = ['byte']
    legible_name = "wait"
class FUNC_FLAG_DISAPPEAR(EbbScript):
    id = 5
    scr_len = 1+1
    macro = "OBJ_FLAG_DISAPPEAR"
    macro_args = ['byte']
    legible_name = "disappear_if_flag"
class FUNC_FLAG_APPEAR(EbbScript):
    id = 6
    scr_len = 1+1
    macro = "OBJ_FLAG_APPEAR"
    macro_args = ['byte']
    legible_name = "appear_if_flag"
class FUNC_DIALOGUE(EbbScript):
    id = 8
    scr_len = 1+2
    macro = "OBJ_DIALOGUE"
    macro_args = ['word']
    legible_name = "show_text"
class FUNC_YESNO_IS_NO(EbbScript):
    id = 9
    scr_len = 1+1
    macro = "OBJ_YESNO_IS_NO"
    macro_args = ['oaddr']
    legible_name = "jump_if_say_no"
class FUNC_IS_NOT_TALKING(EbbScript):
    id = 0xA
    scr_len = 1+1
    macro = "OBJ_IS_NOT_TALKING"
    macro_args = ['oaddr']
    legible_name = "jump_if_not_talking"
class FUNC_IS_NOT_CHECKING(EbbScript):
    id = 0xB
    scr_len = 1+1
    macro = "OBJ_IS_NOT_CHECKING"
    macro_args = ['oaddr']
    legible_name = "jump_if_not_checking"
class FUNC_IS_NOT_CASTING(EbbScript):
    id = 0xC
    scr_len = 1+1+1
    macro = "OBJ_IS_NOT_CASTING"
    macro_args = ['byte', 'oaddr']
    legible_name = "jump_if_not_casting"
class FUNC_IS_NOT_USING(EbbScript):
    id = 0xD
    scr_len = 1+1+1
    macro = "OBJ_IS_NOT_USING"
    macro_args = ['byte', 'oaddr']
    legible_name = "jump_if_not_using"
class FUNC_RESET(EbbScript):
    id = 0xF
    scr_len = 1
    macro = "OBJ_RESET"
    legible_name = "reset"
class FUNC_SET_FLAG(EbbScript):
    id = 0x10
    scr_len = 1+1
    macro = "OBJ_SET_FLAG"
    macro_args = ['byte']
    legible_name = "set_flag"
class FUNC_CLEAR_FLAG(EbbScript):
    id = 0x11
    scr_len = 1+1
    macro = "OBJ_CLEAR_FLAG"
    macro_args = ['byte']
    legible_name = "reset_flag"
class FUNC_IS_NOT_FLAG(EbbScript):
    id = 0x12
    scr_len = 1+1+1
    macro = "OBJ_IS_NOT_FLAG"
    macro_args = ['byte', 'oaddr']
    legible_name = "jump_if_not_flag"
class FUNC_DECREMENT_COUNTER(EbbScript):
    id = 0x13
    scr_len = 1+1
    macro = "OBJ_DECREMENT_COUNTER"
    macro_args = ['byte']
    legible_name = "decrement_counter"
class FUNC_INCREMENT_COUNTER(EbbScript):
    id = 0x14
    scr_len = 1+1
    macro = "OBJ_INCREMENT_COUNTER"
    macro_args = ['byte']
    legible_name = "increment_counter"
class FUNC_RESET_COUNTER(EbbScript):
    id = 0x15
    scr_len = 1+1
    macro = "OBJ_RESET_COUNTER"
    macro_args = ['byte']
    legible_name = "reset_counter"
class FUNC_COUNTERLESSTHAN(EbbScript):
    id = 0x16
    scr_len = 1+1+1+1
    macro = "OBJ_COUNTERLESSTHAN"
    macro_args = ['byte', 'byte', 'oaddr']
    legible_name = "jump_if_counter_less_than"
class FUNC_CHOOSE_CHARACTER(EbbScript):
    id = 0x18
    scr_len = 1+1
    macro = "OBJ_CHOOSE_CHARACTER"
    macro_args = ['oaddr']
    legible_name = "jump_if_declined_choosing_character"
class FUNC_PICK_CHARACTER(EbbScript):
    id = 0x19
    scr_len = 1+1
    macro = "OBJ_PICK_CHARACTER"
    macro_args = ['byte']
    legible_name = "load_character"
class FUNC_NOT_CHARACTER_SELECTED(EbbScript):
    id = 0x1A
    scr_len = 1+1+1
    macro = "OBJ_NOT_CHARACTER_SELECTED"
    macro_args = ['byte', 'oaddr']
    legible_name = "jump_if_not_character_loaded"
class FUNC_NO_NEW_MONEY(EbbScript):
    id = 0x1B
    scr_len = 1+1
    macro = "OBJ_NO_NEW_MONEY"
    macro_args = ['oaddr']
    legible_name = "jump_if_no_new_money"
class FUNC_LOAD_NUMBER(EbbScript):
    id = 0x1D
    scr_len = 1+2
    macro = "OBJ_LOAD_NUMBER"
    macro_args = ['word']
    legible_name = "load_number"
class FUNC_SHOW_MONEY(EbbScript):
    id = 0x1F
    scr_len = 1
    macro = "OBJ_SHOW_MONEY"
    legible_name = "show_money"
class FUNC_CHOOSE_ITEM(EbbScript):
    id = 0x20
    scr_len = 1+1
    macro = "OBJ_CHOOSE_ITEM"
    macro_args = ['oaddr']
    legible_name = "jump_if_declined_choosing_item"
class FUNC_CHOOSE_ITEM_CLOSET(EbbScript):
    id = 0x21
    scr_len = 1+1
    macro = "OBJ_CHOOSE_ITEM_CLOSET"
    macro_args = ['oaddr']
    legible_name = "jump_if_declined_choosing_closet_item"
class FUNC_DISPLAY_ITEMS(EbbScript):
    id = 0x22
    scr_len = 1+1+1+1+1+1
    macro = "OBJ_DISPLAY_ITEMS"
    macro_args = ['byte', 'byte', 'byte', 'byte', 'oaddr']
    legible_name = "jump_if_declined_showing_shop"
class FUNC_PICK_ITEM(EbbScript):
    id = 0x25
    scr_len = 1+1
    macro = "OBJ_PICK_ITEM"
    macro_args = ['byte']
    legible_name = "load_item"
class FUNC_IS_NOT_SELECTED(EbbScript):
    id = 0x26
    scr_len = 1+1+1
    macro = "OBJ_IS_NOT_SELECTED"
    macro_args = ['byte', 'oaddr']
    legible_name = "jump_if_not_item_loaded"
class FUNC_NOT_HAS_ITEM(EbbScript):
    id = 0x27
    scr_len = 1+1+1
    macro = "OBJ_NOT_HAS_ITEM"
    macro_args = ['byte', 'oaddr']
    legible_name = "jump_if_doesnt_have"
class FUNC_GIVE_MONEY(EbbScript):
    id = 0x28
    scr_len = 1+1
    macro = "OBJ_GIVE_MONEY"
    macro_args = ['oaddr']
    legible_name = "jump_if_cant_hold_give_money"
class FUNC_TAKE_MONEY(EbbScript):
    id = 0x29
    scr_len = 1+1
    macro = "OBJ_TAKE_MONEY"
    macro_args = ['oaddr']
    legible_name = "jump_if_cant_give_take_money"
class FUNC_UNSELLABLE(EbbScript):
    id = 0x2C
    scr_len = 1+1
    macro = "OBJ_UNSELLABLE"
    macro_args = ['oaddr']
    legible_name = "jump_if_item_cant_be_sold"
class FUNC_GIVE_ITEM(EbbScript):
    id = 0x2D
    scr_len = 1+1
    macro = "OBJ_GIVE_ITEM"
    macro_args = ['oaddr']
    legible_name = "jump_if_cant_hold_give_item"
class FUNC_REMOVE_ITEM(EbbScript):
    id = 0x2E
    scr_len = 1+1
    macro = "OBJ_REMOVE_ITEM"
    macro_args = ['oaddr']
    legible_name = "jump_if_cant_give_take_item"
class FUNC_ADD_ITEM_TO_CLOSET(EbbScript):
    id = 0x2F
    scr_len = 1+1
    macro = "OBJ_ADD_ITEM_TO_CLOSET"
    macro_args = ['oaddr']
    legible_name = "jump_if_closet_cant_hold_give_item"
class FUNC_TAKE_ITEM_FROM_CLOSET(EbbScript):
    id = 0x30
    scr_len = 1+1
    macro = "OBJ_TAKE_ITEM_FROM_CLOSET"
    macro_args = ['oaddr']
    legible_name = "jump_if_closet_doesnt_have_take_item"
class FUNC_PICK_CHARACTER_ITEM(EbbScript):
    id = 0x31
    scr_len = 1+1+1
    macro = "OBJ_PICK_CHARACTER_ITEM"
    macro_args = ['byte', 'oaddr']
    legible_name = "jump_if_empty_load_character_item"
class FUNC_MULTIPLY_NUMBER(EbbScript):
    id = 0x32
    scr_len = 1+1
    macro = "OBJ_MULTIPLY_NUMBER"
    macro_args = ['byte']
    legible_name = "multiply_number"
class FUNC_NOT_HAS_CHARACTER(EbbScript):
    id = 0x33
    scr_len = 1+1+1
    macro = "OBJ_NOT_HAS_CHARACTER"
    macro_args = ['byte', 'oaddr']
    legible_name = "jump_if_not_character_in_party"
class FUNC_IS_NOT_TOUCHING(EbbScript):
    id = 0x35
    scr_len = 1+1
    macro = "OBJ_IS_NOT_TOUCHING"
    macro_args = ['oaddr']
    legible_name = "jump_if_not_touching"
class FUNC_J_UNK(EbbScript):
    id = 0x36
    scr_len = 1+1
    macro = "J_UNK"
    macro_args = ['oaddr']
    legible_name = "jump_if_unk"
class FUNC_MENU(EbbScript):
    id = 0x37
    scr_len = 1+2+1+1
    macro = "OBJ_MENU"
    macro_args = ['word', 'oaddr', 'oaddr']
    legible_name = "jump_j1_pick_2_jump_j2_if_declined_menu"
class FUNC_NO_ITEMS(EbbScript):
    id = 0x38
    scr_len = 1+1
    macro = "OBJ_NO_ITEMS"
    macro_args = ['oaddr']
    legible_name = "jump_if_no_items"
class FUNC_NO_ITEMS_CLOSET(EbbScript):
    id = 0x39
    scr_len = 1+1
    macro = "OBJ_NO_ITEMS_CLOSET"
    macro_args = ['oaddr']
    legible_name = "jump_if_no_closet_items"
class FUNC_PARTY_CHARACTER(EbbScript):
    id = 0x3a
    scr_len = 1+1+1
    macro = "OBJ_PARTY_CHARACTER"
    macro_args = ['byte', 'oaddr']
    legible_name = "jump_if_no_charater_loaded"
class FUNC_CHANGE_TYPE(EbbScript):
    id = 0x3B
    scr_len = 1+1
    macro = "OBJ_CHANGE_TYPE"
    macro_args = ['byte']
    legible_name = "change_type"
class FUNC_TELEPORT(EbbScript):
    id = 0x3d
    scr_len = 1+4
    macro = "OBJ_TELEPORT"
    macro_args = ['doorArgDef']
    legible_name = "teleport"
class FUNC_MOVE(EbbScript):
    id = 0x3e
    scr_len = 1+2
    macro = "OBJ_MOVE"
    macro_args = ['addr']
    legible_name = "move"
class FUNC_SIGNAL(EbbScript):
    id = 0x3f
    scr_len = 1+1
    macro = "OBJ_SIGNAL"
    macro_args = ['byte']
    legible_name = "signal"
class FUNC_IS_SIGNAL(EbbScript):
    id = 0x40
    scr_len = 1+1
    macro = "OBJ_IS_SIGNAL"
    macro_args = ['oaddr']
    legible_name = "jump_if_not_signaled"
class FUNC_TELEPORT_TO_SAVEGAME(EbbScript):
    id = 0x41
    scr_len = 1
    macro = "OBJ_TELEPORT_TO_SAVEGAME"
    legible_name = "warp_to_save_location"
class FUNC_ADD_CHARACTER(EbbScript):
    id = 0x42
    scr_len = 1+1+1
    macro = "OBJ_ADD_CHARACTER"
    macro_args = ['byte', 'oaddr']
    legible_name = "jump_if_cant_add_character"
class FUNC_REMOVE_CHARACTER(EbbScript):
    id = 0x43
    scr_len = 1+1+1
    macro = "OBJ_REMOVE_CHARACTER"
    macro_args = ['byte', 'oaddr']
    legible_name = "jump_if_cant_remove_character"
class FUNC_BATTLE(EbbScript):
    id = 0x44
    scr_len = 1+1
    macro = "OBJ_BATTLE"
    macro_args = ['byte']
    legible_name = "start_battle"
class FUNC_MULTIPLY_BY_PARTY(EbbScript):
    id = 0x45
    scr_len = 1
    macro = "OBJ_MULTIPLY_BY_PARTY"
    legible_name = "multiply_number_by_party"
class FUNC_ROCKET(EbbScript):
    id = 0x46
    scr_len = 1+1
    macro = "OBJ_ROCKET"
    macro_args = ['byte']
    legible_name = "spawn_rocket"
class FUNC_AIRPLANE(EbbScript):
    id = 0x47
    scr_len = 1+1
    macro = "OBJ_AIRPLANE"
    macro_args = ['byte']
    legible_name = "spawn_airplane"
class FUNC_TANK(EbbScript):
    id = 0x48
    scr_len = 1+1
    macro = "OBJ_TANK"
    macro_args = ['byte']
    legible_name = "spawn_tank"
class FUNC_NOVEC(EbbScript):
    id = 0x4C
    scr_len = 1+1
    macro = "OBJ_NOVEC"
    macro_args = ['byte']
    legible_name = "despawn_vehicle"
class FUNC_END_PLANE(EbbScript):
    id = 0x4D
    scr_len = 1
    macro = "OBJ_END_PLANE"
    legible_name = "end_plane_path"
class FUNC_J_UNK2(EbbScript):
    id = 0x4F
    scr_len = 1+1
    macro = "J_UNK2"
    macro_args = ['oaddr']
    legible_name = "jump_if_unk2"
class FUNC_NOT_MAX_HEALTH(EbbScript):
    id = 0x50
    scr_len = 1+1
    macro = "OBJ_NOT_MAX_HEALTH"
    macro_args = ['oaddr']
    legible_name = "jump_if_less_than_max_hp"
class FUNC_HEAL(EbbScript):
    id = 0x51
    scr_len = 1+1
    macro = "OBJ_HEAL"
    macro_args = ['byte']
    legible_name = "heal"
class FUNC_HAS_STATUS(EbbScript):
    id = 0x52
    scr_len = 1+1+1
    macro = "OBJ_HAS_STATUS"
    macro_args = ['byte', 'oaddr']
    legible_name = "jump_if_has_status"
class FUNC_AND_STATUS(EbbScript):
    id = 0x53
    scr_len = 1+1
    macro = "OBJ_AND_STATUS"
    macro_args = ['byte']
    legible_name = "keep_status"
class FUNC_BELOW_LEVEL(EbbScript):
    id = 0x54
    scr_len = 1+1+1
    macro = "OBJ_BELOW_LEVEL"
    macro_args = ['byte', 'oaddr']
    legible_name = "jump_if_more_than_level"
class FUNC_SLEEP(EbbScript):
    id = 0x55
    scr_len = 1
    macro = "OBJ_SLEEP"
    legible_name = "sleep"
class FUNC_SAVE(EbbScript):
    id = 0x56
    scr_len = 1
    macro = "OBJ_SAVE"
    legible_name = "save"
class FUNC_GET_NEXT_EXP(EbbScript):
    id = 0x57
    scr_len = 1
    macro = "OBJ_GET_NEXT_EXP"
    legible_name = "get_needed_exp"
class FUNC_GET_CASH(EbbScript):
    id = 0x58
    scr_len = 1
    macro = "OBJ_GET_CASH"
    legible_name = "get_current_cash"
class FUNC_GIVE_STATUS(EbbScript):
    id = 0x59
    scr_len = 1+1
    macro = "OBJ_GIVE_STATUS"
    macro_args = ['byte']
    legible_name = "give_status"
class FUNC_PLAY_MUSIC(EbbScript):
    id = 0x5A
    scr_len = 1+1
    macro = "OBJ_PLAY_MUSIC"
    macro_args = ['byte']
    legible_name = "play_music"
class FUNC_PLAY_SOUND2(EbbScript):
    id = 0x5B
    scr_len = 1+1
    macro = "OBJ_PLAY_SOUND2"
    macro_args = ['byte']
    legible_name = "play_sound2"
class FUNC_PLAY_SOUND(EbbScript):
    id = 0x5C
    scr_len = 1+1
    macro = "OBJ_PLAY_SOUND"
    macro_args = ['byte']
    legible_name = "play_sound"
class FUNC_NOT_MAX_PP(EbbScript):
    id = 0x60
    scr_len = 1+1
    macro = "OBJ_NOT_MAX_PP"
    macro_args = ['oaddr']
    legible_name = "jump_if_less_than_max_pp"
class FUNC_PPHEAL(EbbScript):
    id = 0x61
    scr_len = 1+1
    macro = "OBJ_PPHEAL"
    macro_args = ['byte']
    legible_name = "restore_pp"
class FUNC_TAKE_WEAPON(EbbScript):
    id = 0x62
    scr_len = 1+1
    macro = "OBJ_TAKE_WEAPON"
    macro_args = ['oaddr']
    legible_name = "jump_if_cant_confiscate_weapon"
class FUNC_SELECT_CONFWEAPON(EbbScript):
    id = 0x63
    scr_len = 1+1
    macro = "OBJ_SELECT_CONFWEAPON"
    macro_args = ['oaddr']
    legible_name = "jump_if_no_confiscated_weapon"
class FUNC_DO_LIVE_SHOW(EbbScript):
    id = 0x64
    scr_len = 1
    macro = "OBJ_DO_LIVE_SHOW"
    legible_name = "live_show"
class FUNCINCOMPLETE_MELODIES(EbbScript):
    id = 0x65
    scr_len = 1+1
    macro = "OBJ_INCOMPLETE_MELODIES"
    macro_args = ['oaddr']
    legible_name = "jump_if_not_have_all_melodies"
class FUNC_REGISTER_NAME(EbbScript):
    id = 0x66
    scr_len = 1
    macro = "OBJ_REGISTER_NAME"
    legible_name = "register_name"
class FUNC_LAND_MINE(EbbScript):
    id = 0x68
    scr_len = 1
    macro = "OBJ_LAND_MINE"
    legible_name = "trigger_landmine"

SCRIPT_TABLE = {
    0: FUNC_STOP,
    1: FUNC_JUMP, #just jump
    2: FUNC_SUBROUTINE, #call pointer j of object o
    3: FUNC_RETURN, #return from subroutine
    4: FUNC_DELAY, #delay for t frames
    5: FUNC_FLAG_DISAPPEAR, #disappear if f == 1
    6: FUNC_FLAG_APPEAR, #appear if f == 1
    8: FUNC_DIALOGUE, #display dialogue from msg_pointerlist
    9: FUNC_YESNO_IS_NO, #ask yes or no, jump if no/b
    0xA: FUNC_IS_NOT_TALKING, #jump to j if not talking
    0xB: FUNC_IS_NOT_CHECKING, #jump to j if not checking
    0xC: FUNC_IS_NOT_CASTING, #jump to j if not using psi p
    0xD: FUNC_IS_NOT_USING, #jump to j if not using this item
    0xF: FUNC_RESET, #reset the nes
    0x10: FUNC_SET_FLAG, #set flag f
    0x11: FUNC_CLEAR_FLAG, #clear flag f
    0x12: FUNC_IS_NOT_FLAG, #jump to j if f == 0
    0x13: FUNC_DECREMENT_COUNTER, #decrements counter c
    0x14: FUNC_INCREMENT_COUNTER, #increments counter c
    0x15: FUNC_RESET_COUNTER, #resets counter c
    0x16: FUNC_COUNTERLESSTHAN, #jumps to j if counter c < int n
    0x18: FUNC_CHOOSE_CHARACTER, #choose character, jump if b pressed
    0x19: FUNC_PICK_CHARACTER, #select character
    0x1A: FUNC_NOT_CHARACTER_SELECTED, #jump to j if chararacter c not selected
    0x1B: FUNC_NO_NEW_MONEY, #jump to j if no money has been gained since last call
    0x1D: FUNC_LOAD_NUMBER, #load number ????
    0x1F: FUNC_SHOW_MONEY, #yeah
    0x20: FUNC_CHOOSE_ITEM, #jump to j if declined
    0x21: FUNC_CHOOSE_ITEM_CLOSET, #jump to j if declined
    0x22: FUNC_DISPLAY_ITEMS, #jump if b pressed
    0x25: FUNC_PICK_ITEM, #load i into selected item
    0x26: FUNC_IS_NOT_SELECTED, #jump to j if i isnt selected
    0x27: FUNC_NOT_HAS_ITEM, #jump to j if i not in inventory
    0x28: FUNC_GIVE_MONEY, #jump to j if cant hold money
    0x29: FUNC_TAKE_MONEY, #jump to j if not enough money
    0x2C: FUNC_UNSELLABLE, #jump to j if item cannot be sold
    0x2D: FUNC_GIVE_ITEM, #jump to j if inventory full. else give selected item
    0x2E: FUNC_REMOVE_ITEM, #remove item, jump if doesn't have
    0x2F: FUNC_ADD_ITEM_TO_CLOSET, #add item to closet, jump to j if closet full
    0x30: FUNC_TAKE_ITEM_FROM_CLOSET, #remove item to closet, jump to j if not available
    0x31: FUNC_PICK_CHARACTER_ITEM, #pick character's I'th item, jump if empty (0)
    0x32: FUNC_MULTIPLY_NUMBER, #multiply number by n / 100
    0x33: FUNC_NOT_HAS_CHARACTER, #jump to j if character is not in party
    0x35: FUNC_IS_NOT_TOUCHING, #jump to j if not touching
    0x36: FUNC_J_UNK, #jump to j if ??????
    0x37: FUNC_MENU, #display menu pointer, jump to j1 if option 2, jump to j2 if b pressed
    0x38: FUNC_NO_ITEMS, #jump to j if no items
    0x39: FUNC_NO_ITEMS_CLOSET, #jump to j if no items in closet
    0x3A: FUNC_PARTY_CHARACTER, #select character c in party, jump to j if not present
    0x3B: FUNC_CHANGE_TYPE, #change object type to t
    0x3D: FUNC_TELEPORT, #teleport player to doorArgDef (basically, runs a door command)
    0x3E: FUNC_MOVE, #move using m pointer (word)
    0x3F: FUNC_SIGNAL, #signal object o (index)
    0x40: FUNC_IS_SIGNAL, #jump to j if not signaled
    0x41: FUNC_TELEPORT_TO_SAVEGAME, #teleport to saved game location
    0x42: FUNC_ADD_CHARACTER, #add char c from party, jump to j if full
    0x43: FUNC_REMOVE_CHARACTER, #remove char c from party, jump to j if not in
    0x44: FUNC_BATTLE, #start battle b in battles list
    0x45: FUNC_MULTIPLY_BY_PARTY, #multiply by number of characters
    0x46: FUNC_ROCKET, #spawn rocket in direction (?)
    0x47: FUNC_AIRPLANE, #spawn airplane in direction (?)
    0x48: FUNC_TANK, #spawn tank in direction (?)
    0x4C: FUNC_NOVEC, #spawn players in direction (?)
    0x4D: FUNC_END_PLANE, #end plane path
    0x4F: FUNC_J_UNK2, #jump to j if ?????
    0x50: FUNC_NOT_MAX_HEALTH, #jump to j if < max hp
    0x51: FUNC_HEAL, #heal hp n
    0x52: FUNC_HAS_STATUS, #jump to j if character has status s
    0x53: FUNC_AND_STATUS, #remove all statuses but s
    0x54: FUNC_BELOW_LEVEL, #jump to j if character < n
    0x55: FUNC_SLEEP, #sleep
    0x56: FUNC_SAVE, #self explanatory
    0x57: FUNC_GET_NEXT_EXP, #get selected characters' needed exp
    0x58: FUNC_GET_CASH, #get money
    0x59: FUNC_GIVE_STATUS, #inflict s status
    0x5A: FUNC_PLAY_MUSIC, #play m song
    0x5B: FUNC_PLAY_SOUND2, #play s
    0x5C: FUNC_PLAY_SOUND, #play s
    0x60: FUNC_NOT_MAX_PP, #jump to j if < max pp
    0x61: FUNC_PPHEAL, #heal pp n
    0x62: FUNC_TAKE_WEAPON, #jump to j if no weapon, else take
    0x63: FUNC_SELECT_CONFWEAPON, #get confiscated weapon, jump to j if none
    0x64: FUNC_DO_LIVE_SHOW, #in ellay
    0x65: FUNCINCOMPLETE_MELODIES, #jump to j if not all melodies learnt
    0x66: FUNC_REGISTER_NAME, #register name
    0x68: FUNC_LAND_MINE, #in yucca desert
}


class EbbProgrammable(EbbSprite):
    script_output = []
    movement_data = None
    def __init__(self, data, addr_start):
        super().__init__(data, addr_start)

        end = len(self.script)

        i = 0
        while i < end:
            func = self.script[i]
            get_class = SCRIPT_TABLE[func]
            if type(get_class) == list:
                break
            byte_data = self.script[i:i+get_class.scr_len]
            new_script = get_class(byte_data)
            self.script_output.append(new_script)

            #MOVE pointers usually indicate the end of the script.
            if get_class == FUNC_MOVE:
                result = new_script.args[0] - (self.addr_start+parse[self.obtype].offset)
                if end > result:
                    end = result
            i += len(byte_data)

        if end < len(self.script):
            self.movement_data = self.script[end:]

    def to_macros(self):
        super().to_macros()
        for script in self.script_output:
            self.lines.append(f"{script}\n")

class EbbStationaryNPC(EbbProgrammable):
    def __init__(self, data, addr_start):
        super().__init__(data, addr_start)
class EbbWanderingNPC(EbbStationaryNPC):
    def __init__(self, data, addr_start):
        super().__init__(data, addr_start)
class EbbWanderingNPCFast(EbbWanderingNPC):
    def __init__(self, data, addr_start):
        super().__init__(data, addr_start)
class EbbSpinningNPC(EbbProgrammable):
    def __init__(self, data, addr_start):
        super().__init__(data, addr_start)

class EbbStationaryNPC_FlagCheck(EbbStationaryNPC):
    def __init__(self, data, addr_start):
        super().__init__(data, addr_start)
class EbbWanderingNPC_FlagCheck(EbbWanderingNPC):
    def __init__(self, data, addr_start):
        super().__init__(data, addr_start)
class EbbWanderingNPCFast_FlagCheck(EbbWanderingNPCFast):
    def __init__(self, data, addr_start):
        super().__init__(data, addr_start)
class EbbSpinningNPC_FlagCheck(EbbSpinningNPC):
    def __init__(self, data, addr_start):
        super().__init__(data, addr_start)

class EbbFlagDo(EbbObject):
    def __init__(self, data, addr_start):
        super().__init__(data, addr_start)
        self.byte = (data[4] & 0b11111000) >> 3
        self.flag = 0x80 >> (data[4] & 0b00000111)

class EbbFlagsetSee(EbbFlagDo):
    def __init__(self, data, addr_start):
        super().__init__(data, addr_start)
class EbbFlagresetSee(EbbFlagDo):
    def __init__(self, data, addr_start):
        super().__init__(data, addr_start)

type_map = {
    0: EbbObject, # Dummy
    1: EbbDoor,
    2: EbbDoor,
    3: EbbStairs,
    4: EbbHole,
    8: EbbObject, # player
    0x10: EbbStationaryNPC,
    0x11: EbbWanderingNPC,
    0x12: EbbWanderingNPCFast,
    0x13: EbbSpinningNPC,
    0x14: EbbStationaryNPC_FlagCheck,
    0x15: EbbWanderingNPC_FlagCheck,
    0x16: EbbWanderingNPCFast_FlagCheck,
    0x17: EbbSpinningNPC_FlagCheck,
    0x29: EbbFlagsetSee,
    0x2A: EbbFlagresetSee,
}

mem_data = yaml.safe_load(open("split.yaml"))
object_list = [mem_data["splits"]["prg"][0x10]]

objects = {}
for bank in object_list:
    addr = 0x8000
    for split in bank["splits"]:
        file = ""
        if len(split) > 1:
            file = f"split/{split[1]}.bin"

        if file.find("OBJ_") == -1:
            addr = split[0] + 0x8000
            continue
        data = open(file, 'rb').read()
        addr = split[0] + 0x8000
        new_object = EbbObject(data, addr)
        if new_object.obtype in list(type_map.keys()):
            new_object = type_map[new_object.obtype](data, addr)

        objects[split[1]] = new_object

for object in list(objects.keys()):
    if issubclass(type(objects[object]), EbbProgrammable):
        objects[object].to_macros()
        print(objects[object].lines)
