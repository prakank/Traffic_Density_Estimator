CC = g++
TARGET = code
CFLAGS = -pthread -std=c++11 `pkg-config --cflags --libs opencv`

all: $(TARGET).cpp
	$(CC) $(TARGET).cpp -o $(TARGET) $(CFLAGS)


.PHONY:clean

clean:
	rm -rf $(TARGET)

help:
	@echo "usage: make <command>"
	@echo "\nFor execution on image file: ./code <image.jpg>"
	@echo "Options:"
	@echo "\n positional arguments:"
	@echo "  command"

	@echo "    all:\tcompile code.cpp and generate executable binary"
	@echo "    clean:\tremove executable binary"
	@echo "    help:\tdisplay help"
