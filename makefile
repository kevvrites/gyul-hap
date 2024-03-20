CC = gcc
INCLUDE_PATH = -I/opt/homebrew/opt/raylib/include
LIBRARY_PATH = -L/opt/homebrew/opt/raylib/lib
LIBS = -lraylib -framework IOKit -framework Cocoa -framework OpenGL
SRC = gyulhap.c 
OUT = gyulhap 
CFLAGS = -Wall -std=c99

all: $(OUT)

$(OUT): $(SRC)
	$(CC) $(CFLAGS) $(INCLUDE_PATH) $(LIBRARY_PATH) $(SRC) -o $(OUT) $(LIBS)

clean:
	rm -f $(OUT)

