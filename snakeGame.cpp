#include <iostream>
#include <deque>
#include <conio.h>
#include <windows.h>
#include <cstdlib>
#include <ctime>
#include <fstream>

using namespace std;

enum Difficulty { EASY, MEDIUM, HARD };
struct Config {
    int gridSize;
    int speed;
} configs[] = {
    {25, 200},
    {20, 150},
    {15, 100}
};

enum Direction { UP, DOWN, LEFT, RIGHT };

struct Position {
    int x, y;
    bool operator==(const Position &other) const { return x == other.x && y == other.y; }
};

class Snake {
public:
    deque<Position> body;
    Direction direction;

    Snake(int startX, int startY) {
        body.push_front({startX, startY});
        body.push_back({startX - 1, startY});
        body.push_back({startX - 2, startY});
        direction = RIGHT;
    }

    void move() {
        Position newHead = body.front();
        switch (direction) {
            case UP:    newHead.y--; break;
            case DOWN:  newHead.y++; break;
            case LEFT:  newHead.x--; break;
            case RIGHT: newHead.x++; break;
        }
        body.push_front(newHead);
        body.pop_back();
    }

    void grow() { body.push_back(body.back()); }

    bool hasCollided(int gridSize) {
        Position head = body.front();
        if (head.x < 0 || head.x >= gridSize || head.y < 0 || head.y >= gridSize)
            return true;
        for (size_t i = 1; i < body.size(); i++)
            if (body[i] == head)
                return true;
        return false;
    }

    bool isOnSnake(Position pos) {
        for (const auto &segment : body)
            if (segment == pos)
                return true;
        return false;
    }
};

class Food {
public:
    Position pos;

    void generateNewPosition(Snake &snake, int gridSize) {
        srand(time(0));
        do {
            pos.x = rand() % gridSize;
            pos.y = rand() % gridSize;
        } while (snake.isOnSnake(pos));
    }
};

class Game {
public:
    Snake snake;
    Food food;
    int gridSize;
    int speed;
    int score;
    int highestScore;
    bool gameOver;
    Difficulty difficulty;

    Game(Difficulty diff) : snake(10, 10), score(0), gameOver(false), difficulty(diff) {
        gridSize = configs[diff].gridSize;
        speed = configs[diff].speed;
        food.generateNewPosition(snake, gridSize);
        highestScore = readHighestScore();
    }

    void processInput() {
        if (_kbhit()) {
            char key = _getch();
            switch (key) {
                case 'w': if (snake.direction != DOWN) snake.direction = UP; break;
                case 's': if (snake.direction != UP) snake.direction = DOWN; break;
                case 'a': if (snake.direction != RIGHT) snake.direction = LEFT; break;
                case 'd': if (snake.direction != LEFT) snake.direction = RIGHT; break;
            }
        }
    }

    void updateGame() {
        snake.move();
        if (snake.hasCollided(gridSize)) {
            gameOver = true;
            return;
        }
        if (snake.body.front() == food.pos) {
            snake.grow();
            food.generateNewPosition(snake, gridSize);
            score++;
            if (score > highestScore) {
                highestScore = score;
                saveHighestScore();
            }
        }
    }

    void displayBoard() {
        system("cls");
        for (int i = 0; i < gridSize + 2; i++) cout << "#";
        cout << endl;

        for (int i = 0; i < gridSize; i++) {
            cout << "#";
            for (int j = 0; j < gridSize; j++) {
                bool printed = false;
                if (food.pos.x == j && food.pos.y == i) {
                    cout << "*";
                    printed = true;
                }
                for (size_t k = 0; k < snake.body.size(); k++) {
                    if (snake.body[k].x == j && snake.body[k].y == i) {
                        cout << (k == 0 ? 'O' : 'o');
                        printed = true;
                        break;
                    }
                }
                if (!printed) cout << " ";
            }
            cout << "#\n";
        }

        for (int i = 0; i < gridSize + 2; i++) cout << "#";
        cout << "\nScore: " << score << "  |  Highest Score: " << highestScore << endl;
    }

    void run() {
        while (!gameOver) {
            processInput();
            updateGame();
            displayBoard();
            Sleep(speed);
        }
        cout << "Game Over! Final Score: " << score << endl;
        cout << "Highest Score: " << highestScore << endl;
        chooseNextAction();
    }

    void chooseNextAction() {
        int choice;
        cout << "1. Restart with new difficulty\n";
        cout << "2. Restart with the same difficulty\n";
        cout << "3. Quit\n";
        cout << "Enter choice: ";
        cin >> choice;

        if (choice == 1) {
            chooseMode();
        } else if (choice == 2) {
            restartGame(difficulty);
        } else {
            cout << "Exiting game. Thanks for playing!\n";
            exit(0);
        }
    }

    void chooseMode() {
        int choice;
        cout << "Choose difficulty:\n";
        cout << "1. Easy\n2. Medium\n3. Hard\nEnter choice (1-3): ";
        cin >> choice;
        Difficulty newDifficulty;
        switch (choice) {
            case 1: newDifficulty = EASY; break;
            case 2: newDifficulty = MEDIUM; break;
            case 3: newDifficulty = HARD; break;
            default: cout << "Invalid choice. Defaulting to Medium.\n"; newDifficulty = MEDIUM;
        }
        restartGame(newDifficulty);
    }

    void restartGame(Difficulty newDifficulty) {
        cout << "Restarting in ";
        for (int i = 3; i > 0; i--) {
            cout << i << "... ";
            Sleep(1000);
        }
        cout << "Go!\n";
        snake = Snake(gridSize / 2, gridSize / 2);
        difficulty = newDifficulty;
        gridSize = configs[newDifficulty].gridSize;
        speed = configs[newDifficulty].speed;
        food.generateNewPosition(snake, gridSize);
        score = 0;
        gameOver = false;
        run();
    }

    int readHighestScore() {
        ifstream file("highscore.txt");
        int storedHighScore = 0;
        if (file) {
            file >> storedHighScore;
        }
        file.close();
        return storedHighScore;
    }

    void saveHighestScore() {
        ofstream file("highscore.txt");
        if (file) {
            file << highestScore;
        }
        file.close();
    }
};

int main() {
    int choice;
    cout << "Choose difficulty:\n";
    cout << "1. Easy\n2. Medium\n3. Hard\nEnter choice (1-3): ";
    cin >> choice;
    Difficulty difficulty;
    switch (choice) {
        case 1: difficulty = EASY; break;
        case 2: difficulty = MEDIUM; break;
        case 3: difficulty = HARD; break;
        default: cout << "Invalid choice. Defaulting to Medium.\n"; difficulty = MEDIUM;
    }
    Game game(difficulty);
    game.run();
    return 0;
}