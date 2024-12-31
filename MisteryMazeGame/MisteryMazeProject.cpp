/*
    Name: Anamaria Carla Grigoroiu
	Student ID: 10873935
	University Of Plymouth, Computer Science(artificial inteligents), Year 1
    Project Title: Mystery Maze Game
    Description: A text-based maze navigation game where the player must collect keys, avoid enemies, and solve puzzles to progress through levels.
    Features:
    - Randomly generated maze
    - Player movement (WASD)
    - Enemies that chase the player
	- Resources keys to open the door and potions to eliminate enemies
	- Math puzzles to unlock doors (10-second timer)
	- Inventory system to track keys and potions
	- Multiple levels with increasing difficulty(increasing enemies and doors numbers)
	- Game over if the player runs out of potions or keys
	- Instructions and menu screen
	- Exit confirmation
	Controls:
	- W: Move up
	- A: Move left
	- S: Move down
	- D: Move right
	- K: Use key
	- O: Use potion
	- Esc: Exit game
	- Enter: Restart game
*/
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <thread> // For sleep function 
#include <conio.h> // For _getch() function
#include <map>     // For inventory system
#include <algorithm>
#include <chrono>  // For timer

const int WIDTH = 20;  // Column of the maze
const int HEIGHT = 20; // Row of the maze
char maze[HEIGHT][WIDTH]; // 2D array for the maze
int playerX = 0, playerY = 2; // Player position
int exitX = HEIGHT - 1, exitY = WIDTH - 2; // Exit position
int enemyCount = 1; // Number of enemies
int doorCount = 3; // Initial number of doors
int resourceCount = 2; // Number of resources (keys and potions)
int level = 1; // Track the current level

std::map<std::string, int> inventory = { {"keys", 1}, {"potions", 1} }; // Player starts with 1 key and 1 potion

// Directions: {row offset, column offset}
int directions[4][2] = { {-1, 0}, {1, 0}, {0, -1}, {0, 1} };

// Enemy structure
struct Enemy {
    int x, y;

    void moveTowardsPlayer() {
        int newX = x, newY = y;

        if (std::abs(playerX - x) > std::abs(playerY - y)) {
            newX += (playerX > x) ? 1 : -1;
        }
        else {
            newY += (playerY > y) ? 1 : -1;
        }

        if (newX >= 0 && newX < HEIGHT && newY >= 0 && newY < WIDTH && maze[newX][newY] == ' ') {
            maze[x][y] = ' '; // Clear enemy's current position
            x = newX;
            y = newY;
            maze[x][y] = 'E'; // Mark new position
        }
    }
};

struct Door {
    int x, y;
};
std::vector<Door> doors;

struct Resource {
    int x, y;
    char type; // 'K' for key, 'O' for potion
};
std::vector<Resource> resources;

// Function Prototypes
void resetGame(std::vector<Enemy>& enemies);
void resetGameOver(std::vector<Enemy>& enemies);
bool solvePuzzle();
void initializeMaze();
void carvePaths(int x, int y);
void addEntranceAndExit();
void displayMaze();
bool movePlayer(char direction, std::vector<Enemy>& enemies);

// Initialize the maze with walls
void initializeMaze() {
    for (int i = 0; i < HEIGHT; ++i) {
        for (int j = 0; j < WIDTH; ++j) {
            maze[i][j] = '#'; // Fill the maze with walls
        }
    }
}

// Display the maze and current level
void displayMaze() {
    system("cls");
    std::cout << "Level: " << level << " | Keys: " << inventory["keys"]
        << " | Potions: " << inventory["potions"] << "\n";
    for (int i = 0; i < HEIGHT; ++i) {
        for (int j = 0; j < WIDTH; ++j) {
            std::cout << maze[i][j] << ' ';
        }
        std::cout << std::endl;
    }
}

// Check if a cell is valid for carving
bool isValidCell(int x, int y) {
    return x > 0 && y > 0 && x < HEIGHT - 1 && y < WIDTH - 1 && maze[x][y] == '#';
}

// Recursive function to carve paths
void carvePaths(int x, int y) {
    maze[x][y] = ' ';

    for (int i = 0; i < 4; ++i) {
        int r = rand() % 4;
        std::swap(directions[i], directions[r]);
    }

    for (auto dir : directions) {
        int nx = x + dir[0] * 2;
        int ny = y + dir[1] * 2;
        if (isValidCell(nx, ny)) {
            maze[x + dir[0]][y + dir[1]] = ' ';
            carvePaths(nx, ny);
        }
    }
}

// Add entrance and exit points
void addEntranceAndExit() {
    maze[0][2] = ' ';
    playerX = 0;
    playerY = 2;
    maze[playerX][playerY] = 'P';

    maze[exitX][exitY] = ' ';
}
// Solve a puzzle to unlock a door
bool solvePuzzle() {
    int a = rand() % 100 + 1;
    int b = rand() % 100 + 1;
    int answer = -1;
    int timeLimit = 10; // Set time limit in seconds

    std::cout << "Solve the puzzle to unlock the door: What is " << a << " + " << b << "?\n";

    auto startTime = std::chrono::steady_clock::now(); // Start the timer

    while (true) {
        // Calculate elapsed time
        auto currentTime = std::chrono::steady_clock::now();
        int elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime).count();
        int timeLeft = timeLimit - elapsedTime;

        // Display the remaining time
        if (timeLeft >= 0) {
            std::cout << "\rTime left: " << timeLeft << " seconds      " << std::flush;
        }

        // If time runs out
        if (timeLeft <= 0) {
            std::cout << "\nTime's up! Press 'K' to use a key or 'Esc' to exit the game.\n";
            char input = _getch(); // Wait for user input
            if (input == 'K' || input == 'k') {
                if (inventory["keys"] > 0) {
                    inventory["keys"]--;
                    return true; // Use a key and unlock the door
                }
                else {
                    std::cout << "You have no keys left! Game over.\n";
                    return false; // Game over if no keys
                }
            }
            else if (input == 27) { // Esc key
                exit(0); // Exit the game
            }
            return false; // Fallback: Game over
        }

        // Check if the player pressed "Enter"
        if (_kbhit()) {
            std::cin >> answer;
            if (answer == a + b) {
                return true; // Correct answer
            }
            else {
                std::cout << "\nIncorrect answer. Press 'K' to use a key or 'Esc' to exit the game.\n";
                char input = _getch();
                if (input == 'K' || input == 'k') {
                    if (inventory["keys"] > 0) {
                        inventory["keys"]--;
                        return true; // Use a key and unlock the door
                    }
                    else {
                        std::cout << "You have no keys left! \n";
                        return false; // Game over if no keys
                    }
                }
                else if (input == 27) { // Esc key
                    exit(0); // Exit the game
                }
                return false; // Game over
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Small delay for smooth timer updates
    }
}

// Reset game after game over
void resetGameOver(std::vector<Enemy>& enemies) {
    std::cout << "Game Over! Press Enter to restart or Esc to exit.\n";
    while (true) {
        char input = _getch();
        if (input == 13) { // Enter key to restart
            level = 1;
            enemyCount = 1;
            doorCount = 3;
            resourceCount = 2;
            inventory["keys"] = 1;
            inventory["potions"] = 1;
            resetGame(enemies);
            break;
        }
        else if (input == 27) { // Esc key to exit
            exit(0);
        }
    }
}

// Reset the game and initialize a new level
void resetGame(std::vector<Enemy>& enemies) {
    initializeMaze();
    carvePaths(0, 2);
    addEntranceAndExit();

    doors.resize(doorCount);
    for (int i = 0; i < doorCount; ++i) {
        do {
            doors[i].x = rand() % HEIGHT;
            doors[i].y = rand() % WIDTH;
        } while (maze[doors[i].x][doors[i].y] != ' ');
        maze[doors[i].x][doors[i].y] = 'D';
    }

    resources.resize(resourceCount);
    for (int i = 0; i < resourceCount; ++i) {
        do {
            resources[i].x = rand() % HEIGHT;
            resources[i].y = rand() % WIDTH;
            resources[i].type = (rand() % 2 == 0) ? 'K' : 'O';
        } while (maze[resources[i].x][resources[i].y] != ' ');
        maze[resources[i].x][resources[i].y] = resources[i].type;
    }

    enemies.resize(enemyCount);
    for (int i = 0; i < enemyCount; ++i) {
        do {
            enemies[i].x = rand() % HEIGHT;
            enemies[i].y = rand() % WIDTH;
        } while (maze[enemies[i].x][enemies[i].y] != ' ');
        maze[enemies[i].x][enemies[i].y] = 'E';
    }
}
// Handle player movement
bool movePlayer(char direction, std::vector<Enemy>& enemies) {
    int newX = playerX, newY = playerY;

    // Adjust player position based on input
    if (direction == 'W' || direction == 'w') newX--;
    else if (direction == 'S' || direction == 's') newX++;
    else if (direction == 'A' || direction == 'a') newY--;
    else if (direction == 'D' || direction == 'd') newY++;

    // Check if new position is within bounds
    if (newX >= 0 && newX < HEIGHT && newY >= 0 && newY < WIDTH) {
        if (maze[newX][newY] == 'E') { // Enemy encountered
            std::cout << "Enemy encountered! Press 'O' to use a potion or 'Esc' to quit.\n";
            char action = _getch();
            if (action == 'O' || action == 'o') { // Use potion
                if (inventory["potions"] > 0) {
                    inventory["potions"]--;
                    std::cout << "Potion used! Enemy eliminated.\n";

                    // Remove enemy from maze and list
                    auto it = std::find_if(enemies.begin(), enemies.end(),
                        [newX, newY](const Enemy& e) {
                            return e.x == newX && e.y == newY;
                        });
                    if (it != enemies.end()) {
                        maze[it->x][it->y] = ' ';
                        enemies.erase(it);
                    }
                }
                else {
                    std::cout << "No potions left! You cannot defeat the enemy.\n";
                    resetGameOver(enemies); // Game over if no potions
                    return false;
                }
            }
            else if (action == 27) { // Esc key
                exit(0); // Quit game
            }
        }
        else if (maze[newX][newY] == 'K') { // Key encountered
            inventory["keys"]++;
            maze[newX][newY] = ' ';
        }
        else if (maze[newX][newY] == 'O') { // Potion encountered
            inventory["potions"]++;
            maze[newX][newY] = ' ';
        }
        else if (maze[newX][newY] == 'D') { // Door encountered
            if (!solvePuzzle()) {
                resetGameOver(enemies);
                return false;
            }
            maze[newX][newY] = ' '; // Unlock door
        }
        else if (maze[newX][newY] == ' ') { // Valid move
            maze[playerX][playerY] = ' ';
            playerX = newX;
            playerY = newY;
            maze[playerX][playerY] = 'P';
        }
    }

    // Return true if player reaches the exit
    return playerX == exitX && playerY == exitY;
}

// Function to display the menu
void displayMenu() {
    system("cls"); // Clear the console screen
    std::cout << "========================\n";
    std::cout << "     MYSTERY MAZE      \n";
    std::cout << "========================\n";
    std::cout << "1. Start Game\n";
    std::cout << "2. Instructions\n";
    std::cout << "3. Exit\n";
    std::cout << "========================\n";
    std::cout << "Enter your choice: ";
}

// Function to show instructions
void showInstructions() {
    system("cls");
    std::cout << "========================\n";
    std::cout << "      INSTRUCTIONS      \n";
    std::cout << "========================\n";
    std::cout << "* Use W, A, S, D to move in the maze.\n";
    std::cout << "* Collect keys ('K') to unlock doors ('D').\n";
    std::cout << "* Avoid enemies unless you have potions.\n";
    std::cout << "* Collect potions ('O') to eliminate enemies ('E').\n";
    std::cout << "* The game is over if your inventory has 0 potions.\n";
    std::cout << "* Solve math puzzles or use keys to unlock doors ('D').\n";
    std::cout << "* A 10-second timer will run to solve the puzzle.\n";
    std::cout << "* If the timer runs out, use a key to open the door.\n";
    std::cout << "* The game is over if your inventory has 0 keys.\n";
    std::cout << "* Reach the exit (' ') to advance to the next level.\n";
    std::cout << "* Each new level increases the number of enemies ('E') and doors ('D').\n";
    std::cout << "========================\n";
    std::cout << "Press any key to return to the menu.\n";
    (void)_getch(); // Wait for the user to press a key and suppress warning
}
int main() {
    srand(static_cast<unsigned int>(time(0)));
    std::vector<Enemy> enemies;
    resetGame(enemies);
    // Menu loop
    while (true) {
        displayMenu();
        char choice;
        std::cin >> choice;

        if (choice == '1') {
            // Start the game
            std::vector<Enemy> enemies; // Vector to store multiple enemies
            resetGame(enemies); // Initialize the game

            while (true) {
                displayMaze();
                char direction = _getch(); // Get player input

                if (movePlayer(direction, enemies)) {
                    level++;
                    enemyCount++;
                    doorCount++;
                    resourceCount++;
                    resetGame(enemies); // Restart the game with new level
                }

                for (auto& enemy : enemies) {
                    enemy.moveTowardsPlayer(); // Move all enemies
                }
            }
        }
        else if (choice == '2') {
            // Show instructions
            showInstructions();
        }
        else if (choice == '3') {
            std::cout << "Are you sure you want to exit? (y/n): ";
            char confirm;
            std::cin >> confirm;
            if (confirm == 'y' || confirm == 'Y') {
                std::cout << "Thanks for playing! Goodbye!\n";
                break;
            }
        }
        else {
            std::cout << "Invalid choice. Please try again.\n";
        }
    }

    return 0;
}