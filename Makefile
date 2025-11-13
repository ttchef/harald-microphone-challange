CC = gcc
CFLAGS = -g -O0
LDFLAGS = -lraylib -lm -lportaudio

BUILD_DIR = build

SRC_FILES = $(wildcard *.c)
OBJ_FILES = $(addprefix $(BUILD_DIR)/, $(SRC_FILES:.c=.o))

TARGET = $(BUILD_DIR)/main

all: $(TARGET)

$(TARGET): $(OBJ_FILES)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(BUILD_DIR)

