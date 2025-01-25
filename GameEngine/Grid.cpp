#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/vec3.hpp> // glm::vec3

int map[60][100];

bool LoadMap(const std::string& filename)
{
	std::ifstream file("../Question1/Maze_1286580.txt");
	if (!file.is_open()) {
		std::cerr << "Error opening file: " << filename << std::endl;
		return false;
	}

	std::string line;
	int y = 0;
	while (std::getline(file, line) && y < 60) {
		for (int x = 0; x < 100 && x < line.length(); ++x) {
			map[y][x] = (line[x] == 'X') ? 1 : 0;
		}
		y++;
	}
	file.close();
	return true;
}

bool IsGridEmpty(int playerX, int playerY)
{
	if (playerX > 0 || playerX < -100 || playerY < 0 || playerY > 60) {
		return false;
	}

	return (map[playerY][playerX] == 0);  // Adjusted to ensure row/column alignment
}

bool IsDeadEnd(int x, int y, const glm::vec3& direction) 
{
    const std::pair<int, int> directions[] = {
         {0, -1},  // Up
         {0, 1},   // Down
         {-1, 0},  // Left
         {1, 0}    // Right
    };

    int blockedDirections = 0;
    glm::vec3 backtrackingDir = -direction;

    // Loop through all directions
    for (const std::pair<int, int>& dir : directions) {
        int newX = x + dir.first;
        int newY = y + dir.second;

        // Skip the backtracking direction
        glm::vec3 checkDir(dir.first, 0.0f, dir.second);
        if (checkDir == backtrackingDir) {
            continue;
        }

        // Check if out of bounds or blocked
        if (newX < 0 || newX >= 100 || newY < 0 || newY >= 60 || map[newY][newX] == 1) {
            blockedDirections++;
        }
    }

    // A dead-end is when all directions (except the backtracking direction) are blocked
    return (blockedDirections >= 3);
}