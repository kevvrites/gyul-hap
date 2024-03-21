#include "raylib.h"
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

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

bool areAllSameOrAllDifferent(int value1, int value2, int value3);
bool compareTiles(Tile *tiles1, Tile *tiles2);
bool isValidHap(Tile tile1, Tile tile2, Tile tile3);
int countAllHaps(Tile *boardTiles, int numTiles);
void findAllHaps(Tile *boardTiles, int numTiles, Tile (*haps)[3]);
void shuffleArray(Tile *array, int size);
void drawTile(Tile tile, int x, int y, int size);
bool isTileSelected(Tile tile, Tile *selectedTiles, int numSelectedTiles);
void handleTileSelection(Tile *tiles, int numTiles, Tile *selectedTiles, int *numSelectedTiles, Tile (*haps)[3], Tile (*duplicates)[3], int *numHaps, int *numDuplicates, int *remainingHaps, int *score);
void drawTileWithBorder(Tile tile, int x, int y, int size, Color borderColor, int borderWidth);

bool areAllSameOrAllDifferent(int value1, int value2, int value3) {
    return (value1 == value2 && value2 == value3) || (value1 != value2 && value2 != value3 && value1 != value3);
}

bool compareTiles(Tile *tiles1, Tile *tiles2) {
    for (int i = 0; i < 3; i++) {
        if (tiles1[i].backgroundColor != tiles2[i].backgroundColor ||
            tiles1[i].shape != tiles2[i].shape ||
            tiles1[i].shapeColor != tiles2[i].shapeColor) {
            return false;
        }
    }
    return true;
}

bool isValidHap(Tile tile1, Tile tile2, Tile tile3) {
    bool backgroundColorCondition = areAllSameOrAllDifferent(tile1.backgroundColor, tile2.backgroundColor, tile3.backgroundColor);
    bool shapeCondition = areAllSameOrAllDifferent(tile1.shape, tile2.shape, tile3.shape);
    bool shapeColorCondition = areAllSameOrAllDifferent(tile1.shapeColor, tile2.shapeColor, tile3.shapeColor);
    return backgroundColorCondition && shapeCondition && shapeColorCondition;
}

int countAllHaps(Tile *boardTiles, int numTiles) {
    int numHaps = 0;

    for (int i = 0; i < numTiles - 2; i++) {
        for (int j = i + 1; j < numTiles - 1; j++) {
            for (int k = j + 1; k < numTiles; k++) {
                if (isValidHap(boardTiles[i], boardTiles[j], boardTiles[k])) {
                    numHaps++;
                }
            }
        }
    }

    return numHaps;
}

void findAllHaps(Tile *boardTiles, int numTiles, Tile (*haps)[3]) {
    int numHaps = 0;

    for (int i = 0;i < numTiles - 2; i++) {
        for (int j = i + 1; j < numTiles - 1; j++) {
            for (int k = j + 1; k < numTiles; k++) {
                if (isValidHap(boardTiles[i], boardTiles[j], boardTiles[k])) {
                    haps[numHaps][0] = boardTiles[i];
                    haps[numHaps][1] = boardTiles[j];
                    haps[numHaps][2] = boardTiles[k];
                    numHaps++;
                }
            }
        }
    }
}

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

void handleTileSelection(Tile *tiles, int numTiles, Tile *selectedTiles, int *numSelectedTiles, Tile (*haps)[3], Tile (*duplicates)[3], int *numHaps, int *numDuplicates, int *remainingHaps, int *score) {
    Vector2 mousePosition = GetMousePosition();

    // Tile Selection
    for (int i = 0; i < numTiles; i++) {
        int tileX = MARGIN + ((i % 3) * SPACING) + (i % 3) * TILE_SIDE_LENGTH;
        int tileY = MARGIN + ((i / 3) * SPACING) + (i / 3) * TILE_SIDE_LENGTH;

        // Mouse Boundaries
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

    // Checking Puzzle Submission
    if (*numSelectedTiles == 3) {
        bool isValid = isValidHap(selectedTiles[0], selectedTiles[1], selectedTiles[2]);
        bool isDuplicate = false;

        // If Valid, check if already found
        if (isValid) {
            for (int i = 0; i < *numDuplicates; i++) {
                if (compareTiles(selectedTiles, duplicates[i])) {
                    isDuplicate = true;
                    (*score)--; // Duplicate Answer
                    break;
                }
            }
            
            // New Solution, move to found list
            if (!isDuplicate) {
                for (int i = 0; i < *numHaps; i++) {
                    if (compareTiles(selectedTiles, haps[i]) ||
                        compareTiles(selectedTiles, (Tile[3]){haps[i][0], haps[i][2], haps[i][1]}) ||
                        compareTiles(selectedTiles, (Tile[3]){haps[i][1], haps[i][0], haps[i][2]}) ||
                        compareTiles(selectedTiles, (Tile[3]){haps[i][1], haps[i][2], haps[i][0]}) ||
                        compareTiles(selectedTiles, (Tile[3]){haps[i][2], haps[i][0], haps[i][1]}) ||
                        compareTiles(selectedTiles, (Tile[3]){haps[i][2], haps[i][1], haps[i][0]})) {
                        duplicates[*numDuplicates][0] = haps[i][0];
                        duplicates[*numDuplicates][1] = haps[i][1];
                        duplicates[*numDuplicates][2] = haps[i][2];
                        (*numDuplicates)++;


                        for (int j = i; j < *numHaps - 1; j++) {
                            haps[j][0] = haps[j + 1][0];
                            haps[j][1] = haps[j + 1][1];
                            haps[j][2] = haps[j + 1][2];
                        }

                        (*numHaps)--;
                        (*remainingHaps)--;
                        (*score)++; // Right Answer
                        break;
                    }
                }
            }
        } else {
            (*score)--; // Wrong Answer
        }
        *numSelectedTiles = 0; // Reset selection
    }
}

// Selected tiles drawn with gold borders
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

    // Make empty array of selected tiles
    Tile selectedTiles[MAX_SELECTED_TILES];
    int numSelectedTiles = 0;

    // Find all valid Haps
    int numHaps = countAllHaps(boardTiles, NUM_TILES);
    int numDuplicates = 0;
    Tile (*haps)[3] = malloc(numHaps * sizeof(Tile[3]));
    findAllHaps(boardTiles, NUM_TILES, haps);
    Tile (*duplicates)[3] = malloc(numHaps * sizeof(Tile[3]));

    // Score and tracking
    int remainingHaps = numHaps;
    int score = 0;

    while (!WindowShouldClose())
    {
        // Update
        handleTileSelection(boardTiles, NUM_TILES, selectedTiles, &numSelectedTiles, haps, duplicates, &numHaps, &numDuplicates, &remainingHaps, &score);

        // Draw Loop 
        BeginDrawing();
        ClearBackground(BEIGE);
        
        DrawText(TextFormat("Remaining Haps: %d", remainingHaps), 10, 10, 20, BLACK);
        DrawText(TextFormat("Score: %d", score), 10, 40, 20, BLACK);

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

    free(haps);
    free(duplicates);

    return 0;
}
