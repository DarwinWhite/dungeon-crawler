#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "logic.h"

using namespace std;

/**
 * TODO: Student implement this function
 * Load representation of the dungeon level from file into the 2D map.
 * Calls createMap to allocate the 2D array.
 * @param   fileName    File name of dungeon level.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @param   player      Player object by reference to set starting position.
 * @return  pointer to 2D dynamic array representation of dungeon map with player's location., or nullptr if loading fails for any reason
 * @updates  maxRow, maxCol, player
 */
char** loadLevel(const string& fileName, int& maxRow, int& maxCol, Player& player) {
    
    //Variables and streams needed
    ifstream inFile;
    stringstream inString;
    string currentLine = "";
    char** map;
    bool isDoor = false;
    bool isExit = false;
    int currRow = 0;
    int currCol = 0;
    char dummy;
    int dummyInt;

    //Opening the file and making sure the file opened correctly
    inFile.open(fileName);
    if (!inFile.is_open()) {
        return nullptr;
    }

    //Grabbing the first and second line and error checking them, 
    // very specific 2 integer per line format
    getline(inFile, currentLine);
    inString.clear();
    inString.str(currentLine);
    inString >> maxRow >> maxCol;
    if (inString.fail()) {
        return nullptr;
    }
    inString >> dummyInt;
    if (!inString.fail()) {
        return nullptr;
    }
    getline(inFile, currentLine);
    inString.clear();
    inString.str(currentLine);
    inString >> player.row >> player.col;
    if (inString.fail()) {
        return nullptr;
    }
    inString >> dummyInt;
    if (!inString.fail()) {
        return nullptr;
    }

    //Checking for overflow
    if (maxCol != 0 && maxRow > INT32_MAX / (double) maxCol) {
        return nullptr;
    }
    if (player.col != 0 && player.row > INT32_MAX / (double) player.col) {
        return nullptr;
    }

    //Checking to make sure ints are positive and then
    // calling createMap() to allocate the 2D array
    if (maxRow > 0 && maxCol > 0 && maxRow <= 999999 && maxCol <= 999999 && player.row > -1 && player.col > -1 && player.row < maxRow && player.col < maxCol) {
        map = createMap(maxRow, maxCol);
    } else {
        return nullptr;
    }

    //Looping through rows and columns and assigning char from
    // file to corresponding spot in 2D array
    for (int i = 0; i < maxRow; i++) {
        for (int j = 0; j < maxCol; j++) {
            
            inFile >> map[i][j];
            currRow = i;
            currCol = j;

            //For if the input is not a char or if there
            // aren't enough values
            if (inFile.fail()) {
                deleteMap(map, maxRow);
                return nullptr;
            }

            //For if the input is not one of the used char symbols
            if (map[i][j] != TILE_OPEN && map[i][j] != TILE_TREASURE && map[i][j] != TILE_AMULET && map[i][j] != TILE_MONSTER
                && map[i][j] != TILE_PILLAR && map[i][j] != TILE_DOOR && map[i][j] != TILE_EXIT) {
                    deleteMap(map, maxRow);
                    return nullptr;
            }


            //Making sure there is a door
            if (map[i][j] == TILE_DOOR) {
                isDoor = true;
            }
            if (map[i][j] == TILE_EXIT) {
                isExit = true;
            }
        }
    }

    //If there are too many values in the file
    inFile >> dummy;
    if (currRow == maxRow - 1 && currCol == maxCol - 1 && !inFile.eof()) {
        deleteMap(map, maxRow);
        return nullptr;
    }

    //If there is no door
    if (!isDoor && !isExit) {
        deleteMap(map, maxRow);
        return nullptr;
    } else if (isDoor && isExit) {
        deleteMap(map, maxRow);
        return nullptr;
    }

    //Changing the player location in the map 2D array
    // to the correct char
    map[player.row][player.col] = 'o';

    //Closing the file
    inFile.close();

    return map;
}

/**
 * TODO: Student implement this function
 * Translate the character direction input by the user into row or column change.
 * That is, updates the nextRow or nextCol according to the player's movement direction.
 * @param   input       Character input by the user which translates to a direction.
 * @param   nextRow     Player's next row on the dungeon map (up/down).
 * @param   nextCol     Player's next column on dungeon map (left/right).
 * @updates  nextRow, nextCol
 */
void getDirection(char input, int& nextRow, int& nextCol) {
    if (input == MOVE_DOWN) {
        nextRow++;
    } else if (input == MOVE_LEFT) {
        nextCol--;
    } else if (input == MOVE_RIGHT) {
        nextCol++;
    } else if (input == MOVE_UP) {
        nextRow--;
    }
}

/**
 * TODO: [suggested] Student implement this function
 * Allocate the 2D map array.
 * Initialize each cell to TILE_OPEN.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @return  2D map array for the dungeon level, holds char type.
 */
char** createMap(int maxRow, int maxCol) {

        //Creating double pointer 2D array
        char** grid = new char*[maxRow];
        for (int i = 0; i < maxRow; i++) {
            grid[i] = new char[maxCol];
        }

        //Filling each cell with TILE_OPEN
        for (int i = 0; i < maxRow; i++) {
            for (int j = 0; j < maxCol; j++) {
                grid[i][j] = TILE_OPEN;
            }
        }

        return grid;
}

/**
 * TODO: Student implement this function
 * Deallocates the 2D map array.
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @return None
 * @update map, maxRow
 */
void deleteMap(char**& map, int& maxRow) {
    
    //Deallocating the 2D array
    if (map) {
        for (int i = 0; i < maxRow; i++) {
            delete[] map[i];
        }
        delete[] map;
    }

    //Setting map to nullptr and maxRow to
    // 0 as default
    map = nullptr;
    maxRow = 0;
}

/**
 * TODO: Student implement this function
 * Resize the 2D map by doubling both dimensions.
 * Copy the current map contents to the right, diagonal down, and below.
 * Do not duplicate the player, and remember to avoid memory leaks!
 * You can use the STATUS constants defined in logic.h to help!
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height), to be doubled.
 * @param   maxCol      Number of columns in the dungeon table (aka width), to be doubled.
 * @return  pointer to a dynamically-allocated 2D array (map) that has twice as many columns and rows in size.
 * @update maxRow, maxCol
 */
char** resizeMap(char** map, int& maxRow, int& maxCol) {

    //To reassign later
    int currentRowValue = maxRow;

    //Making sure map isn't a nullptr
    if (map == nullptr) {
        return nullptr;
    }

    //Another round of error checking just in case invalid
    // dimensions hasn't been caught yet
    if (maxRow < 1 || maxCol < 1) {
        return nullptr;
    }

    //Creating a pointer and calling createMap
    // to allocate the new resized map
    char** resizedMap = createMap(maxRow * 2, maxCol * 2);

    //Copying over the contents of the old 2D array
    // into 4 quadrants to create the new layout
    for (int i = 0; i < maxRow; i++) {
        for (int j = 0; j < maxCol; j++) {
            resizedMap[i][j] = map[i][j];
        }
    }
    int mapI = 0; //Iterators for the original 2D array
    int mapJ;
    for (int i = 0; i < maxRow; i++) {
        mapJ = 0;
        for (int j = maxCol; j < maxCol * 2; j++) {
            resizedMap[i][j] = map[mapI][mapJ];
            if (resizedMap[i][j] == TILE_PLAYER) {
                resizedMap[i][j] = TILE_OPEN;
            }
            mapJ++;
        }
        mapI++;
    }
    mapI = 0;
    for (int i = maxRow; i < maxRow * 2; i++) {
        mapJ = 0;
        for (int j = 0; j < maxCol; j++) {
            resizedMap[i][j] = map[mapI][mapJ];
            if (resizedMap[i][j] == TILE_PLAYER) {
                resizedMap[i][j] = TILE_OPEN;
            }
            mapJ++;
        }
        mapI++;
    }
    mapI = 0;
    for (int i = maxRow; i < maxRow * 2; i++) {
        mapJ = 0;
        for (int j = maxCol; j < maxCol * 2; j++) {
            resizedMap[i][j] = map[mapI][mapJ];
            if (resizedMap[i][j] == TILE_PLAYER) {
                resizedMap[i][j] = TILE_OPEN;
            }
            mapJ++;
        }
        mapI++;
    }

    //Deallocating the old map array
    deleteMap(map, maxRow);

    //Doubling maxRow and maxCol to match the new
    // 2D array
    maxRow = currentRowValue;
    maxRow *= 2;
    maxCol *= 2;

    return resizedMap;
}

/**
 * TODO: Student implement this function
 * Checks if the player can move in the specified direction and performs the move if so.
 * Cannot move out of bounds or onto TILE_PILLAR or TILE_MONSTER.
 * Cannot move onto TILE_EXIT without at least one treasure. 
 * If TILE_TREASURE, increment treasure by 1.
 * Remember to update the map tile that the player moves onto and return the appropriate status.
 * You can use the STATUS constants defined in logic.h to help!
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @param   player      Player object to by reference to see current location.
 * @param   nextRow     Player's next row on the dungeon map (up/down).
 * @param   nextCol     Player's next column on dungeon map (left/right).
 * @return  Player's movement status after updating player's position.
 * @update map contents, player
 */
int doPlayerMove(char** map, int maxRow, int maxCol, Player& player, int nextRow, int nextCol) {

    //Immediately returning STATUS_STAY if the tile is not in the bounds
    // of the map
    if (nextRow >= maxRow || nextCol >= maxCol || nextRow < 0 || nextCol < 0) {
        nextRow = player.row;
        nextCol = player.col;
        return STATUS_STAY;
    }

    //If there is a monster in the way cannot move there
    if (map[nextRow][nextCol] == TILE_MONSTER) {
        nextRow = player.row;
        nextCol = player.col;
        return STATUS_STAY;
    }

    //If there is a pillar in the way cannot move there
    if (map[nextRow][nextCol] == TILE_PILLAR) {
        nextRow = player.row;
        nextCol = player.col;
        return STATUS_STAY;
    }

    //For a normal move onto an open tile
    if (map[nextRow][nextCol] == TILE_OPEN) {
        map[player.row][player.col] = TILE_OPEN;
        player.row = nextRow;
        player.col = nextCol;
        map[nextRow][nextCol] = TILE_PLAYER;
        return STATUS_MOVE;
    }

    //If the tile being moved to is a treasure tile
    if (map[nextRow][nextCol] == TILE_TREASURE) {
        map[player.row][player.col] = TILE_OPEN;
        player.row = nextRow;
        player.col = nextCol;
        player.treasure++;
        map[nextRow][nextCol] = TILE_PLAYER;
        return STATUS_TREASURE;
    }

    //If the tile being moved to is an amulet tile
    if (map[nextRow][nextCol] == TILE_AMULET) {
        map[player.row][player.col] = TILE_OPEN;
        player.row = nextRow;
        player.col = nextCol;
        map[nextRow][nextCol] = TILE_PLAYER;
        return STATUS_AMULET;
    }

    //If the tile being moved to is a door tile
    if (map[nextRow][nextCol] == TILE_DOOR) {
        map[player.row][player.col] = TILE_OPEN;
        player.row = nextRow;
        player.col = nextCol;
        map[nextRow][nextCol] = TILE_PLAYER;
        return STATUS_LEAVE;
    }

    //If the tile being moved to is an exit tile
    if (map[nextRow][nextCol] == TILE_EXIT && player.treasure >= 1) {
        map[player.row][player.col] = TILE_OPEN;
        player.row = nextRow;
        player.col = nextCol;
        map[nextRow][nextCol] = TILE_PLAYER;
        return STATUS_ESCAPE;
    } else {
        nextRow = player.row;
        nextCol = player.col;
        return STATUS_STAY;
    }
}

/**
 * TODO: Student implement this function
 * Update monster locations:
 * We check up, down, left, right from the current player position.
 * If we see an obstacle, there is no line of sight in that direction, and the monster does not move.
 * If we see a monster before an obstacle, the monster moves one tile toward the player.
 * We should update the map as the monster moves.
 * At the end, we check if a monster has moved onto the player's tile.
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @param   player      Player object by reference for current location.
 * @return  Boolean value indicating player status: true if monster reaches the player, false if not.
 * @update map contents
 */
bool doMonsterAttack(char** map, int maxRow, int maxCol, const Player& player) {

    //Checking above the player
    for (int i = player.row - 1; i > -1; i--) {
        
        //If the tile is a pillar we can just exit the loop
        if (map[i][player.col] == TILE_PILLAR) {
            break;
        }

        //If we find a monster above
        if (map[i][player.col] == TILE_MONSTER) {
            map[i][player.col] = TILE_OPEN;
            map[i + 1][player.col] = TILE_MONSTER;
        }
    }

    //Checking below the player
    for (int i = player.row + 1; i < maxRow; i++) {
        if (map[i][player.col] == TILE_PILLAR) {
            break;
        }
        if (map[i][player.col] == TILE_MONSTER) {
            map[i][player.col] = TILE_OPEN;
            map[i - 1][player.col] = TILE_MONSTER;
        }
    }

    //Checking to the left of the player
    for (int i = player.col - 1; i > -1; i--) {
        if (map[player.row][i] == TILE_PILLAR) {
            break;
        }
        if (map[player.row][i] == TILE_MONSTER) {
            map[player.row][i] = TILE_OPEN;
            map[player.row][i + 1] = TILE_MONSTER;
        }
    }

    //Checking to the right of the player
    for (int i = player.col + 1; i < maxCol; i++) {
        if (map[player.row][i] == TILE_PILLAR) {
            break;
        }
        if (map[player.row][i] == TILE_MONSTER) {
            map[player.row][i] = TILE_OPEN;
            map[player.row][i - 1] = TILE_MONSTER;
        }
    }

    if (map[player.row][player.col] == TILE_MONSTER) {
        return true;
    } else {
        return false;
    }
}
