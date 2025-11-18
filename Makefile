
CC = gcc

VENDOR = vendor
INCLUDE_PATH = -Iinclude -I$(VENDOR)/raygui/src

CFLAGS = -g -O0 -fsanitize=address -fsanitize=undefined -std=c11 $(INCLUDE_PATH) 
LDFLAGS = -lraylib -lm -lportaudio

BUILD_DIR = build

SRC_FILES = $(wildcard src/*.c)
TEMP_SRC = $(notdir $(SRC_FILES))
OBJ_FILES = $(addprefix $(BUILD_DIR)/, $(TEMP_SRC:.c=.o))

TARGET = $(BUILD_DIR)/main

all: $(TARGET)

$(TARGET): $(OBJ_FILES)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(BUILD_DIR)/%.o: src/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

run: clean $(TARGET)
	./$(TARGET)

cloc:
	cloc . --exclude-dir=vendor,build,third_party,bin,compile_commands.json

clean:
	rm -rf $(BUILD_DIR)

