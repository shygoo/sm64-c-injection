#
# mips64-elf-gcc
#  https://cen64.com/uploads/n64chain-win64-tools.zip
#  https://cen64.com/uploads/n64chain-linux64-tools.tgz
#
# armips
#  https://buildbot.orphis.net/armips/
#
# n64crc
#  http://n64dev.org/n64crc.html
#

ROM_IN=sm64.z64
ROM_OUT=sm64.mod.z64

CC=mips64-elf-gcc
CFLAGS=-O3 -mtune=vr4300 -march=vr4300 -mabi=32 -fomit-frame-pointer -G0 -I./include

AR=mips64-elf-ar

AS=armips
CRCFIX=n64crc

OBJ_DIR=obj
SRC_DIR=src
TMP_DIR=tmp
ASM_DIR=asm
LIB_DIR=lib

ASM_N64_WRAP=$(TMP_DIR)/armips_n64_wrapper.asm
ASM_LINK=link.asm

SRC_FILES=$(wildcard *.c)
OBJ_FILES=$(patsubst %.c,%.o,$(wildcard *.c))

C_FILES=$(wildcard $(SRC_DIR)/*.c)
O_FILES=$(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(C_FILES))

LIB_SEC_CUSTOM=$(LIB_DIR)/libsec_custom.a

############

.PHONY: rom
rom: $(ROM_OUT)

.PHONY: clean
clean:
	rm -rf $(OBJ_DIR)
	rm -rf $(TMP_DIR)
	rm -rf $(LIB_DIR)
	rm -f $(ROM_OUT)
	rm -f $(N64_WRAP)

############

$(ROM_OUT): $(LIB_SEC_CUSTOM) $(ROM_IN) $(ASM_N64_WRAP) $(ASM_LINK)
	$(AS) $(ASM_N64_WRAP) -root . -sym map.txt
	$(CRCFIX) $(ROM_OUT)

$(LIB_SEC_CUSTOM): $(O_FILES) | $(LIB_DIR)
	$(AR) rcs $(LIB_SEC_CUSTOM) $(O_FILES)

############

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $^ -o $@

############

# Generate a wrapper asm file for armips

$(ASM_N64_WRAP): makefile | $(TMP_DIR)
	printf ".n64\n" > $(ASM_N64_WRAP)
	printf ".open \"$(ROM_IN)\", \"$(ROM_OUT)\", 0\n" >> $(ASM_N64_WRAP)
	printf ".include \"$(ASM_LINK)\"\n" >> $(ASM_N64_WRAP)
	printf ".close\n" >> $(ASM_N64_WRAP)

############

$(OBJ_DIR):
	mkdir $(OBJ_DIR)

$(TMP_DIR):
	mkdir $(TMP_DIR)

$(LIB_DIR):
	mkdir $(LIB_DIR)

############
