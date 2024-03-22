#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800
#define NUM_TILES 9
#define TOTAL_COMBINATIONS 27
#define TILE_SIDE_LENGTH 150
#define H_MARGIN 125
#define V_MARGIN 100
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

Color bgColor = BEIGE;
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
bool isValidGyul(int remainingHaps);
void handleGyul(int remainingHaps, int *score, bool *isGameOver);
void startScreen(bool *startGame, bool *showHelp, bool *showSettings);
void helpScreen(bool *showHelp);
void settingsScreen(bool *showSettings);
void endingScreen(int score, bool *playAgain, bool *quitToMenu);

bool areAllSameOrAllDifferent(int value1, int value2, int value3) {
    return (value1 == value2 && value2 == value3) || (value1 != value2 && value2 != value3 && value1 != value3);
}

bool compareTiles(Tile *tiles1, Tile *tiles2) {
    for (int i = 0; i < 3; i++) {
        bool found = false;
        for (int j = 0; j < 3; j++) {
            if (tiles1[i].backgroundColor == tiles2[j].backgroundColor &&
                tiles1[i].shape == tiles2[j].shape &&
                tiles1[i].shapeColor == tiles2[j].shapeColor) {
                found = true;
                break;
            }
        }
        if (!found) {
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
        int tileX = H_MARGIN + ((i % 3) * SPACING) + (i % 3) * TILE_SIDE_LENGTH;
        int tileY = V_MARGIN + ((i / 3) * SPACING) + (i / 3) * TILE_SIDE_LENGTH;

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
                    if (compareTiles(selectedTiles, haps[i])) {
                        duplicates[*numDuplicates][0] = selectedTiles[0];
                        duplicates[*numDuplicates][1] = selectedTiles[1];
                        duplicates[*numDuplicates][2] = selectedTiles[2];
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

bool isValidGyul(int remainingHaps) {
    return remainingHaps == 0;
}

void handleGyul(int remainingHaps, int *score, bool *isGameOver) {
    Vector2 mousePosition = GetMousePosition();

    // Mouse Boundaries
    if (mousePosition.x >= 300 && mousePosition.x <= 500 &&
        mousePosition.y >= 700 && mousePosition.y <= 750) {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (isValidGyul(remainingHaps)) {
                (*score) += 3;
                (*isGameOver) = true;
            } else {
                (*score) -= 1;
            }
        }
    }
}

void startScreen(bool *startGame, bool *showHelp, bool *showSettings) {
    Vector2 gameNameSize = MeasureTextEx(GetFontDefault(), "GYUL HAP", 120, 1);
    Vector2 buttonSize = {300, 60};
    Vector2 buttonSpacing = {0, 20};

    Rectangle playButton = {(WINDOW_WIDTH - buttonSize.x) / 2, WINDOW_HEIGHT / 2 - buttonSize.y - buttonSpacing.y, buttonSize.x, buttonSize.y};
    Rectangle helpButton = {(WINDOW_WIDTH - buttonSize.x) / 2, WINDOW_HEIGHT / 2, buttonSize.x, buttonSize.y};
    Rectangle settingsButton = {(WINDOW_WIDTH - buttonSize.x) / 2, WINDOW_HEIGHT / 2 + buttonSize.y + buttonSpacing.y, buttonSize.x, buttonSize.y};
    
    Vector2 playTextSize = MeasureTextEx(GetFontDefault(), "Play Game", 30, 1);
    Vector2 helpTextSize = MeasureTextEx(GetFontDefault(), "Help", 30, 1);
    Vector2 settingsTextSize = MeasureTextEx(GetFontDefault(), "Settings", 30, 1);
    
    char *creditText = "Made with raylib - Kevin Liu 2024";
    Vector2 creditTextSize = MeasureTextEx(GetFontDefault(), creditText, 20, 1);
    
    while (!WindowShouldClose() && !(*startGame) && !(*showHelp) && !(*showSettings)) {
        BeginDrawing();
        ClearBackground(bgColor);
        
        DrawTextEx(GetFontDefault(), "GYUL HAP", (Vector2){(WINDOW_WIDTH - gameNameSize.x) / 2, 120}, 120, 5, BLACK);
        
        DrawRectangleRec(playButton, LIGHTGRAY);
        DrawTextEx(GetFontDefault(), "Play Game", (Vector2){playButton.x + (playButton.width - playTextSize.x) / 2, playButton.y + (playButton.height - playTextSize.y) / 2}, 30, 1.5, BLACK);
        
        DrawRectangleRec(helpButton, LIGHTGRAY);
        DrawTextEx(GetFontDefault(), "Help", (Vector2){helpButton.x + (helpButton.width - helpTextSize.x) / 2, helpButton.y + (helpButton.height - helpTextSize.y) / 2}, 30, 1.5, BLACK);
        
        DrawRectangleRec(settingsButton, LIGHTGRAY);
        DrawTextEx(GetFontDefault(), "Settings", (Vector2){settingsButton.x + (settingsButton.width - settingsTextSize.x) / 2, settingsButton.y + (settingsButton.height - settingsTextSize.y) / 2}, 30, 1.5, BLACK);
        
        DrawTextEx(GetFontDefault(), creditText, (Vector2){(WINDOW_WIDTH - creditTextSize.x) / 2, WINDOW_HEIGHT - creditTextSize.y - 20}, 20, 1, GRAY);
        
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 mousePos = GetMousePosition();
            if (CheckCollisionPointRec(mousePos, playButton)) {
                *startGame = true;
            } else if (CheckCollisionPointRec(mousePos, helpButton)) {
                *showHelp = true;
            } else if (CheckCollisionPointRec(mousePos, settingsButton)) {
                *showSettings = true;
            }
        }
        
        EndDrawing();
    }
}

void helpScreen(bool *showHelp) {
    Vector2 titleSize = MeasureTextEx(GetFontDefault(), "Help", 60, 2);
    char *helpText[] = {
        "GYUL HAP is a puzzle game where you need to find HAPs and GYUL.",
        "A HAP is a set of 3 tiles that satisfies the following condition:",
        "- The tile properties must ALL match or ALL be different",
        "Once all HAPs are found, press GYUL",
        "",
        "Scoring: 1 point for successful HAP, 3 points for GYUL.",
        "-1 point for incorrect HAPs and GYULs",
        "Example HAPs:",
        ""
    };
    int numLines = sizeof(helpText) / sizeof(helpText[0]);
    Vector2 textSize = MeasureTextEx(GetFontDefault(), helpText[0], 20, 1.5);
    float textSpacing = 10;
    float exampleSpacing = 20;
    
    Tile exampleHap1[3] = {
        {BG_WHITE, S_CIRCLE, C_RED},
        {BG_WHITE, S_SQUARE, C_RED},
        {BG_WHITE, S_TRIANGLE, C_RED}
    };
    
    Tile exampleHap2[3] = {
        {BG_WHITE, S_CIRCLE, C_RED},
        {BG_GREY, S_SQUARE, C_YELLOW},
        {BG_BLACK, S_TRIANGLE, C_BLUE}
    };
    
    while (!WindowShouldClose() && *showHelp) {
        BeginDrawing();
        ClearBackground(bgColor);
        
        DrawTextEx(GetFontDefault(), "Help", (Vector2){(WINDOW_WIDTH - titleSize.x) / 2, 50}, 60, 2, BLACK);
        
        for (int i = 0; i < numLines; i++) {
            DrawTextEx(GetFontDefault(), helpText[i], (Vector2){50, 150 + i * (textSize.y + textSpacing)}, 20, 1.5, BLACK);
        }
        
        float exampleWidth = 3 * TILE_SIDE_LENGTH + 2 * exampleSpacing;
        float exampleY = 100 + (numLines + 1) * (textSize.y + textSpacing);
        float exampleX = (WINDOW_WIDTH - exampleWidth) / 2;
        
        for (int i = 0; i < 3; i++) {
            drawTile(exampleHap1[i], exampleX + i * (TILE_SIDE_LENGTH + exampleSpacing), exampleY, TILE_SIDE_LENGTH);
        }
        
        exampleY += TILE_SIDE_LENGTH + exampleSpacing;
        for (int i = 0; i < 3; i++) {
            drawTile(exampleHap2[i], exampleX + i * (TILE_SIDE_LENGTH + exampleSpacing), exampleY, TILE_SIDE_LENGTH);
        }
        
        DrawText("Press 'H' to go back", (WINDOW_WIDTH / 2) - MeasureTextEx(GetFontDefault(), "Press 'H' to go back", 20, 1.5).x / 2, WINDOW_HEIGHT - 50, 20, GRAY);
        
        if (IsKeyPressed(KEY_H)) {
            *showHelp = false;
        }
        
        EndDrawing();
    }
}

void settingsScreen(bool *showSettings) {
    Vector2 titleSize = MeasureTextEx(GetFontDefault(), "Settings", 60, 2);
    char *settingsText = "Change background color:";
    Vector2 settingsTextSize = MeasureTextEx(GetFontDefault(), settingsText, 30, 2);
    
    Color colorOptions[5] = {BEIGE, LIGHTGRAY, SKYBLUE, PINK, ORANGE};
    Rectangle colorRects[5];
    Vector2 colorRectsSize = {50, 50};
    Vector2 colorRectsSpacing = {20, 20};
    
    for (int i = 0; i < 5; i++) {
        colorRects[i].width = colorRectsSize.x;
        colorRects[i].height = colorRectsSize.y;
        colorRects[i].x = (WINDOW_WIDTH - (5 * colorRectsSize.x + 4 * colorRectsSpacing.x)) / 2 + i * (colorRectsSize.x + colorRectsSpacing.x);
        colorRects[i].y = WINDOW_HEIGHT / 2;
    }
    
    while (!WindowShouldClose() && *showSettings) {
        BeginDrawing();
        ClearBackground(bgColor);
        
        DrawTextEx(GetFontDefault(), "Settings", (Vector2){(WINDOW_WIDTH - titleSize.x) / 2, 100}, 60, 2, BLACK);
        DrawTextEx(GetFontDefault(), settingsText, (Vector2){(WINDOW_WIDTH - settingsTextSize.x) / 2, WINDOW_HEIGHT / 2 - 100}, 30, 2, BLACK);
        
        for (int i = 0; i < 5; i++) {
            DrawRectangleRec(colorRects[i], colorOptions[i]);
            if (CheckCollisionPointRec(GetMousePosition(), colorRects[i]) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                bgColor = colorOptions[i];
            }
        }
        
        DrawText("Press 'S' to go back", (WINDOW_WIDTH / 2) - MeasureTextEx(GetFontDefault(), "Press 'S' to go back", 20, 1.5).x / 2, WINDOW_HEIGHT - 50, 20, GRAY);
        
        if (IsKeyPressed(KEY_S)) {
            *showSettings = false;
        }
        
        EndDrawing();
    }
}

void endingScreen(int score, bool *playAgain, bool *quitToMenu) {
    Vector2 titleSize = MeasureTextEx(GetFontDefault(), "ROUND COMPLETE", 60, 2);
    char scoreText[20];
    snprintf(scoreText, sizeof(scoreText), "SCORE: %d", score);
    Vector2 scoreSize = MeasureTextEx(GetFontDefault(), scoreText, 40, 2);
    
    Vector2 buttonSize = {300, 60};
    Vector2 buttonSpacing = {0, 20};
    
    Rectangle playAgainButton = {(WINDOW_WIDTH - buttonSize.x) / 2, WINDOW_HEIGHT / 2 - buttonSize.y - buttonSpacing.y, buttonSize.x, buttonSize.y};
    Rectangle quitToMenuButton = {(WINDOW_WIDTH - buttonSize.x) / 2, WINDOW_HEIGHT / 2, buttonSize.x, buttonSize.y};
    
    Vector2 playAgainTextSize = MeasureTextEx(GetFontDefault(), "Play Again", 30, 2);
    Vector2 quitToMenuTextSize = MeasureTextEx(GetFontDefault(), "Quit To Menu", 30, 2);
    
    while (!WindowShouldClose() && !(*playAgain) && !(*quitToMenu)) {
        BeginDrawing();
        ClearBackground(bgColor);
        
        DrawTextEx(GetFontDefault(), "ROUND COMPLETE", (Vector2){(WINDOW_WIDTH - titleSize.x) / 2, 100}, 60, 2, BLACK);
        DrawTextEx(GetFontDefault(), scoreText, (Vector2){(WINDOW_WIDTH - scoreSize.x) / 2, 200}, 40, 2, BLACK);
        
        DrawRectangleRec(playAgainButton, LIGHTGRAY);
        DrawTextEx(GetFontDefault(), "Play Again", (Vector2){playAgainButton.x + (playAgainButton.width - playAgainTextSize.x) / 2, playAgainButton.y + (playAgainButton.height - playAgainTextSize.y) / 2}, 30, 2, BLACK);
        
        DrawRectangleRec(quitToMenuButton, LIGHTGRAY);
        DrawTextEx(GetFontDefault(), "Quit To Menu", (Vector2){quitToMenuButton.x + (quitToMenuButton.width - quitToMenuTextSize.x) / 2, quitToMenuButton.y + (quitToMenuButton.height - quitToMenuTextSize.y) / 2}, 30, 2, BLACK);
        
    
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 mousePos = GetMousePosition();
            if (CheckCollisionPointRec(mousePos, playAgainButton)) {
                *playAgain = true;
            } else if (CheckCollisionPointRec(mousePos, quitToMenuButton)) {
                *quitToMenu = true;
            }
        }
        
        EndDrawing();
    }
}

int main(void) {
    bool startGame = false; // CHANGE TO FALSE LATER
    bool showHelp = false;
    bool showSettings = false;
    bool playAgain = false;
    bool quitToMenu = false;

    bool isGameOver = false;
    unsigned int seed = 42;
    srand(seed);

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "GYUL HAP"); 
    SetTargetFPS(60);
    
    Vector2 gyulTextSize = MeasureTextEx(GetFontDefault(), "GYUL", 50, 1);

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
    
    while (!WindowShouldClose()) {
        if (!startGame) {
            startScreen(&startGame, &showHelp, &showSettings);
        }

        if (showHelp) {
            helpScreen(&showHelp);
        }

        if (showSettings) {
            settingsScreen(&showSettings);
        }

        if (startGame) {
            isGameOver = false;
            playAgain = false;

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

            while (!WindowShouldClose() && !isGameOver)
            {

                // Update
                handleTileSelection(boardTiles, NUM_TILES, selectedTiles, &numSelectedTiles, haps, duplicates, &numHaps, &numDuplicates, &remainingHaps, &score);
                handleGyul(remainingHaps, &score, &isGameOver);

                // Draw Loop
                BeginDrawing();
                ClearBackground(bgColor);
                
                DrawText(TextFormat("Remaining Haps: %d", remainingHaps), 10, 10, 20, BLACK);
                DrawText(TextFormat("Score: %d", score), 10, 40, 20, BLACK);

                // Draw tiles
                for (int i = 0; i < NUM_TILES; i++) {
                    int tileX = H_MARGIN + ((i % 3) * SPACING) + (i % 3) * TILE_SIDE_LENGTH;
                    int tileY = V_MARGIN + ((i / 3) * SPACING) + (i / 3) * TILE_SIDE_LENGTH;
                    
                    if (isTileSelected(boardTiles[i], selectedTiles, numSelectedTiles)) {
                        drawTileWithBorder(boardTiles[i], tileX, tileY, TILE_SIDE_LENGTH, SELECTION_BORDER_COLOR, SELECTION_BORDER_WIDTH);
                    } else {
                        drawTile(boardTiles[i], tileX, tileY, TILE_SIDE_LENGTH);
                    }
                }

                // Draw GYUL button
                DrawRectangle(300, 700, 200, 50, DARKBLUE);
                DrawText("GYUL", 300 + (200 - gyulTextSize.x) / 2, 700 + (50 - gyulTextSize.y) / 2, 50, RAYWHITE);

                EndDrawing();
            }

            free(haps);
            free(duplicates);

            endingScreen(score, &playAgain, &quitToMenu);

            if (playAgain) {
                shuffleArray(tilesArray, TOTAL_COMBINATIONS);

                for (int i = 0; i < NUM_TILES; i++) {
                    boardTiles[i] = tilesArray[i];
                }

                numSelectedTiles = 0;

                numHaps = countAllHaps(boardTiles, NUM_TILES);
                numDuplicates = 0;
                
                haps = malloc(numHaps * sizeof(Tile[3]));
                findAllHaps(boardTiles, NUM_TILES, haps);
                duplicates = malloc(numHaps * sizeof(Tile[3]));

                remainingHaps = numHaps;
                score = 0;

                startGame = true;
                playAgain = false;
            } else if (quitToMenu) {
                startGame = false;
                isGameOver = false;
            }
        }
    }

    CloseWindow();

    return 0;
}
