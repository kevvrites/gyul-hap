#include "raylib.h"
#include <stdlib.h>
#include <time.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800
#define NUM_TILES 9
#define TOTAL_COMBINATIONS 27
#define TILE_SIDE_LENGTH 150
#define MARGIN 125
#define SPACING 50
#define MAX_SELECTED_TILES 3
#define SELECTION_BORDER_WIDTH 5
#define SELECTION_BORDER_COLOR GOLD

typedef enum { BG_WHITE, BG_GREY, BG_BLACK } BackgroundColor;
typedef enum { S_CIRCLE, S_SQUARE, S_TRIANGLE } Shape;
typedef enum { C_RED, C_YELLOW, C_BLUE } ShapeColor;

typedef struct {
    BackgroundColor backgroundColor;
    Shape shape;
    ShapeColor shapeColor;
} Tile;

void shuffleArray(Tile *array, int size) {
    for (int i = size - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Tile temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}

void drawTile(Tile tile, int x, int y, int size) {
    // Tile and background
    Color bgColor;
    switch (tile.backgroundColor) {
        case BG_WHITE:
            bgColor = WHITE;
            break;
        case BG_GREY:
            bgColor = GRAY;
            break;
        case BG_BLACK:
            bgColor = BLACK;
            break;
    }
    DrawRectangle(x, y, TILE_SIDE_LENGTH, TILE_SIDE_LENGTH, bgColor);

    // Shape and color
    Color shapeColor;
    switch (tile.shapeColor) {
        case C_RED:
            shapeColor = RED;
            break;
        case C_YELLOW:
            shapeColor = YELLOW;
            break;
        case C_BLUE:
            shapeColor = BLUE;
            break;
    }
    int shapeSize = TILE_SIDE_LENGTH * 0.7;
    int shapeX = x + (TILE_SIDE_LENGTH - shapeSize) / 2;
    int shapeY = y + (TILE_SIDE_LENGTH - shapeSize) / 2;

    switch (tile.shape) {
        case S_CIRCLE:
            DrawCircle(shapeX + shapeSize / 2, shapeY + shapeSize / 2, shapeSize / 2, shapeColor);
            break;
        case S_SQUARE:
            DrawRectangle(shapeX, shapeY, shapeSize, shapeSize, shapeColor);
            break;
        case S_TRIANGLE:
            DrawTriangle((Vector2){shapeX + shapeSize / 2, shapeY},
                         (Vector2){shapeX, shapeY + shapeSize},
                         (Vector2){shapeX + shapeSize, shapeY+shapeSize}, shapeColor);      
            break;
    }

}

bool isTileSelected(Tile tile, Tile *selectedTiles, int numSelectedTiles) {
    for (int i = 0; i < numSelectedTiles; i++) {
        if (selectedTiles[i].backgroundColor == tile.backgroundColor &&
            selectedTiles[i].shape == tile.shape &&
            selectedTiles[i].shapeColor == tile.shapeColor) {
            return true;
        }
    }
    return false;
}

void handleTileSelection(Tile *tiles, int numTiles, Tile *selectedTiles, int *numSelectedTiles) {
    Vector2 mousePosition = GetMousePosition();

    for (int i = 0; i < numTiles; i++) {
        int tileX = MARGIN + ((i % 3) * SPACING) + (i % 3) * TILE_SIDE_LENGTH;
        int tileY = MARGIN + ((i / 3) * SPACING) + (i / 3) * TILE_SIDE_LENGTH;

        if (mousePosition.x >= tileX && mousePosition.x <= tileX + TILE_SIDE_LENGTH &&
            mousePosition.y >= tileY && mousePosition.y <= tileY + TILE_SIDE_LENGTH) {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                if (isTileSelected(tiles[i], selectedTiles, *numSelectedTiles)) {
                    for (int j = 0; j < *numSelectedTiles; j++) {
                        if (selectedTiles[j].backgroundColor == tiles[i].backgroundColor &&
                            selectedTiles[j].shape == tiles[i].shape &&
                            selectedTiles[j].shapeColor == tiles[i].shapeColor) {
                            for (int k = j; k < *numSelectedTiles - 1; k++) {
                                selectedTiles[k] = selectedTiles[k + 1];
                            }
                            (*numSelectedTiles)--;
                            break;
                        }

                    }
                } else if (*numSelectedTiles < MAX_SELECTED_TILES) {
                    selectedTiles[*numSelectedTiles] = tiles[i];
                    (*numSelectedTiles)++;
                }
            }
        }
    }
}

void drawTileWithBorder(Tile tile, int x, int y, int size, Color borderColor, int borderWidth) {
    drawTile(tile, x, y, size);
    DrawRectangleLinesEx((Rectangle){x, y, size, size}, borderWidth, borderColor);
}

int main(void) {
    unsigned int seed = 42;
    srand(seed);

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "GYUL HAP"); 
    SetTargetFPS(60);
    
    // Make an array of the 27 unique tiles
    Tile tilesArray[TOTAL_COMBINATIONS];
    int index = 0;
    for (int bg = 0; bg < 3; bg++) {
        for (int shape = 0; shape < 3; shape++) {
            for (int color = 0; color < 3; color++) {
                tilesArray[index].backgroundColor = (BackgroundColor)bg;
                tilesArray[index].shape = (Shape)shape;
                tilesArray[index].shapeColor = (ShapeColor)color;
                index++;
            }
        }
    }

    shuffleArray(tilesArray, TOTAL_COMBINATIONS);

    // Randomly shuffle then select the first 9
    Tile boardTiles[NUM_TILES];
    for (int i = 0; i < NUM_TILES; i++) {
        boardTiles[i] = tilesArray[i];
    }

    Tile selectedTiles[MAX_SELECTED_TILES];
    int numSelectedTiles = 0;

    while (!WindowShouldClose())
    {
        // Update
        handleTileSelection(boardTiles, NUM_TILES, selectedTiles, &numSelectedTiles);

        // Draw Loop 
        BeginDrawing();
        ClearBackground(BEIGE);
        
        for (int i = 0; i < NUM_TILES; i++) {
            int tileX = MARGIN + ((i % 3) * SPACING) + (i % 3) * TILE_SIDE_LENGTH;
            int tileY = MARGIN + ((i / 3) * SPACING) + (i / 3) * TILE_SIDE_LENGTH;
            
            if (isTileSelected(boardTiles[i], selectedTiles, numSelectedTiles)) {
                drawTileWithBorder(boardTiles[i], tileX, tileY, TILE_SIDE_LENGTH, SELECTION_BORDER_COLOR, SELECTION_BORDER_WIDTH);
            } else {
                drawTile(boardTiles[i], tileX, tileY, TILE_SIDE_LENGTH);
            }
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
