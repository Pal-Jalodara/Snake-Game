#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <utility>
#include <stack>
#include <ctime>
#include <algorithm>
#include <random>
#include <windows.h>
#include <conio.h>
#include <fstream>
#include <iomanip>
#include <string>
#include <chrono>

using namespace std;
using namespace chrono;

enum class Difficulty
{
    EASY,
    MEDIUM,
    HARD
};
enum class ConsoleColor
{
    BLACK = 0,
    BLUE = 1,
    GREEN = 2,
    CYAN = 3,
    RED = 4,
    MAGENTA = 5,
    BROWN = 6,
    LIGHTGRAY = 7,
    DARKGRAY = 8,
    LIGHTBLUE = 9,
    LIGHTGREEN = 10,
    LIGHTCYAN = 11,
    LIGHTRED = 12,
    LIGHTMAGENTA = 13,
    YELLOW = 14,
    WHITE = 15
};

class Console
{
private:
    HANDLE hConsole;

public:
    Console()
    {
        hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_CURSOR_INFO cursorInfo;
        GetConsoleCursorInfo(hConsole, &cursorInfo);
        cursorInfo.bVisible = false;
        SetConsoleCursorInfo(hConsole, &cursorInfo);
        system("mode con cols=130 lines=40");
        SetConsoleTitleA("Maze Runner");
    }

    ~Console()
    {
        CONSOLE_CURSOR_INFO cursorInfo;
        GetConsoleCursorInfo(hConsole, &cursorInfo);
        cursorInfo.bVisible = true;
        SetConsoleCursorInfo(hConsole, &cursorInfo);
    }

    void setColor(ConsoleColor textColor, ConsoleColor bgColor = ConsoleColor::BLACK)
    {
        SetConsoleTextAttribute(hConsole, static_cast<WORD>(textColor) | (static_cast<WORD>(bgColor) << 4));
    }

    void resetColor()
    {
        setColor(ConsoleColor::LIGHTGRAY);
    }

    void clearScreen()
    {
        COORD coordScreen = {0, 0};
        DWORD cCharsWritten;
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(hConsole, &csbi);
        DWORD dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
        FillConsoleOutputCharacter(hConsole, ' ', dwConSize, coordScreen, &cCharsWritten);
        FillConsoleOutputAttribute(hConsole, csbi.wAttributes, dwConSize, coordScreen, &cCharsWritten);
        SetConsoleCursorPosition(hConsole, coordScreen);
    }

    void gotoXY(int x, int y)
    {
        COORD coord;
        coord.X = x;
        coord.Y = y;
        SetConsoleCursorPosition(hConsole, coord);
    }

    void printCentered(const string &text, int yOffset = 0)
    {
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(hConsole, &csbi);
        int width = csbi.srWindow.Right - csbi.srWindow.Left + 1;

        int x = (width - text.length()) / 2;
        gotoXY(x, yOffset);
        cout << text;
    }

    void drawBox(int left, int top, int width, int height, ConsoleColor bgColor = ConsoleColor::BLACK)
    {
        const char BORDER = (char)219;
        gotoXY(left, top);
        setColor(ConsoleColor::WHITE, bgColor);
        cout << string(width, BORDER);

        for (int i = 1; i < height - 1; i++)
        {
            gotoXY(left, top + i);
            cout << BORDER << string(width - 2, ' ') << BORDER;
        }

        gotoXY(left, top + height - 1);
        cout << string(width, BORDER);
        resetColor();
    }

    void drawTitle(const string &title, int yPos)
    {
        int titleLength = title.length() + 4;
        int left = (80 - titleLength) / 2;

        drawBox(left, yPos, titleLength, 3);
        gotoXY(left + 2, yPos + 1);
        setColor(ConsoleColor::YELLOW);
        cout << title;
        resetColor();
    }
};

struct HighScore
{
    double time;
    string date;
    bool operator<(const HighScore &other) const
    {
        return time < other.time;
    }
};

class ScoreManager
{
private:
    vector<HighScore> highScores[3];
    const string difficultyNames[3] = {"Easy", "Medium", "Hard"};
    const int MAX_HIGH_SCORES = 5;
    Console &console;

public:
    ScoreManager(Console &con) : console(con)
    {
        initHighScores();
    }

    void initHighScores()
    {
        ifstream inFile("maze_scores.dat");
        if (!inFile.good())
        {
            for (int i = 0; i < 3; i++)
            {
                highScores[i].clear();
            }

            ofstream outFile("maze_scores.dat");
            for (int i = 0; i < 3; i++)
            {
                outFile << i << " 0" << endl;
            }
            outFile.close();
        }
        else
        {
            for (int i = 0; i < 3; i++)
            {
                highScores[i].clear();

                int difficulty, numScores;
                inFile >> difficulty >> numScores;

                for (int j = 0; j < numScores; j++)
                {
                    HighScore score;
                    inFile >> score.time;
                    inFile.ignore();
                    getline(inFile, score.date);
                    highScores[i].push_back(score);
                }
            }
            inFile.close();
        }
    }

    void saveHighScores()
    {
        ofstream outFile("maze_scores.dat");
        for (int i = 0; i < 3; i++)
        {
            outFile << i << " " << highScores[i].size() << endl;
            for (const auto &score : highScores[i])
            {
                outFile << score.time << " " << score.date << endl;
            }
        }
        outFile.close();
    }

    bool addHighScore(Difficulty diff, double time)
    {
        time_t now = system_clock::to_time_t(system_clock::now());
        struct tm timeinfo;
        localtime_s(&timeinfo, &now);

        char buffer[80];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d", &timeinfo);
        string date(buffer);

        HighScore newScore = {time, date};
        int diffIndex = static_cast<int>(diff);

        if (highScores[diffIndex].size() < MAX_HIGH_SCORES || time < highScores[diffIndex].back().time)
        {
            highScores[diffIndex].push_back(newScore);
            sort(highScores[diffIndex].begin(), highScores[diffIndex].end());

            if (highScores[diffIndex].size() > MAX_HIGH_SCORES)
            {
                highScores[diffIndex].resize(MAX_HIGH_SCORES);
            }

            saveHighScores();
            return true;
        }

        return false;
    }

    void showHighScores()
    {
        console.clearScreen();

        console.drawTitle("HIGH SCORES", 2);

        for (int i = 0; i < 3; i++)
        {
            console.gotoXY(20, 6 + i * 7);
            console.setColor(ConsoleColor::WHITE);
            cout << "=== " << difficultyNames[i] << " ===";

            if (highScores[i].empty())
            {
                console.gotoXY(20, 8 + i * 7);
                console.setColor(ConsoleColor::LIGHTGRAY);
                cout << "No scores yet!";
            }
            else
            {
                for (size_t j = 0; j < highScores[i].size(); j++)
                {
                    console.gotoXY(20, 8 + j + i * 7);
                    console.setColor(j == 0 ? ConsoleColor::YELLOW : (j == 1 ? ConsoleColor::LIGHTCYAN : ConsoleColor::WHITE));
                    cout << j + 1 << ". " << fixed << setprecision(2) << highScores[i][j].time
                         << " seconds  (" << highScores[i][j].date << ")";
                }
            }
        }

        console.gotoXY(20, 25);
        console.setColor(ConsoleColor::LIGHTGRAY);
        cout << "Press any key to continue...";
        console.resetColor();
        _getch();
    }

    const string &getDifficultyName(Difficulty diff) const
    {
        return difficultyNames[static_cast<int>(diff)];
    }
};

class Maze
{
private:
    static const char WALL = '#';
    static const char PATH = ' ';
    static const char PLAYER = 'P';
    static const char EXIT = 'E';
    static const char VISITED = '*';
    static const char SOLUTION = '+';
    static const char TRAIL = '.';

    int ROWS, COLS, WALL_DENSITY;
    vector<vector<char>> maze;
    vector<vector<bool>> playerTrail;
    stack<pair<int, int>> playerPath;
    pair<int, int> playerPos;
    pair<int, int> exitPos;
    bool showingSolution;
    vector<pair<int, int>> solutionPath;
    vector<pair<int, int>> fullSolutionPath;
    high_resolution_clock::time_point startTime;
    double elapsedSeconds;
    int stepCount;

    bool isValidMove(int x, int y)
    {
        return (x >= 0 && x < ROWS && y >= 0 && y < COLS &&
                (maze[x][y] == PATH || maze[x][y] == EXIT || maze[x][y] == VISITED || maze[x][y] == SOLUTION));
    }

    void findSolutionPath()
    {
        solutionPath.clear();

        queue<pair<int, int>> q;
        vector<vector<bool>> visited(ROWS, vector<bool>(COLS, false));
        vector<vector<pair<int, int>>> parent(ROWS, vector<pair<int, int>>(COLS, {-1, -1}));

        q.push(playerPos);
        visited[playerPos.first][playerPos.second] = true;

        int dx[] = {-1, 1, 0, 0};
        int dy[] = {0, 0, -1, 1};

        bool found = false;

        while (!q.empty() && !found)
        {
            auto [x, y] = q.front();
            q.pop();

            if (x == exitPos.first && y == exitPos.second)
            {
                found = true;
                break;
            }

            for (int i = 0; i < 4; i++)
            {
                int nx = x + dx[i];
                int ny = y + dy[i];
                if (isValidMove(nx, ny) && !visited[nx][ny])
                {
                    visited[nx][ny] = true;
                    parent[nx][ny] = {x, y};
                    q.push({nx, ny});
                }
            }
        }

        if (found)
        {
            pair<int, int> current = exitPos;
            while (current != playerPos && current != parent[current.first][current.second])
            {
                solutionPath.push_back(current);
                current = parent[current.first][current.second];
            }

            if (current == playerPos)
            {
                solutionPath.push_back(current);
            }

            reverse(solutionPath.begin(), solutionPath.end());
        }
    }

public:
    Maze() : ROWS(0), COLS(0), WALL_DENSITY(0), showingSolution(false), elapsedSeconds(0.0) {}

    void init(Difficulty difficulty)
    {
        random_device rd;
        mt19937 gen(rd());

        switch (difficulty)
        {
        case Difficulty::EASY:
            ROWS = 11;
            COLS = 19;
            WALL_DENSITY = 20;
            break;
        case Difficulty::MEDIUM:
            ROWS = 15;
            COLS = 27;
            WALL_DENSITY = 30;
            break;
        case Difficulty::HARD:
            ROWS = 21;
            COLS = 39;
            WALL_DENSITY = 40;
            break;
        }

        maze.assign(ROWS, vector<char>(COLS, WALL));
        playerTrail.assign(ROWS, vector<bool>(COLS, false));

        while (!playerPath.empty())
        {
            playerPath.pop();
        }

        playerPos = {1, 1};
        exitPos = {ROWS - 2, COLS - 2};
        showingSolution = false;

        stack<pair<int, int>> cells;
        cells.push(playerPos);
        maze[playerPos.first][playerPos.second] = PATH;

        int dx[] = {-2, 2, 0, 0};
        int dy[] = {0, 0, -2, 2};

        while (!cells.empty())
        {
            auto [x, y] = cells.top();

            vector<int> directions = {0, 1, 2, 3};
            shuffle(directions.begin(), directions.end(), gen);

            bool found = false;
            for (int dir : directions)
            {
                int nx = x + dx[dir];
                int ny = y + dy[dir];

                if (nx > 0 && nx < ROWS - 1 && ny > 0 && ny < COLS - 1 && maze[nx][ny] == WALL)
                {
                    maze[nx][ny] = PATH;
                    maze[x + dx[dir] / 2][y + dy[dir] / 2] = PATH;

                    cells.push({nx, ny});
                    found = true;
                    break;
                }
            }

            if (!found)
                cells.pop();
        }

        maze[exitPos.first][exitPos.second] = EXIT;
        maze[playerPos.first][playerPos.second] = PLAYER;

        playerPath.push(playerPos);

        findSolutionPath();
        fullSolutionPath = solutionPath;

        startTime = high_resolution_clock::now();
        elapsedSeconds = 0.0;
        stepCount = 0;
    }

    void movePlayer(int dx, int dy)
    {
        int newX = playerPos.first + dx;
        int newY = playerPos.second + dy;

        if (isValidMove(newX, newY))
        {
            bool backtracking = false;

            if (playerPath.size() > 1)
            {
                auto prevPos = playerPath.top();
                playerPath.pop();
                auto prevPrevPos = playerPath.top();
                playerPath.push(prevPos);

                if (prevPrevPos.first == newX && prevPrevPos.second == newY)
                {
                    backtracking = true;
                    playerPath.pop();
                }
            }

            if (playerPos != exitPos)
            {
                maze[playerPos.first][playerPos.second] = PATH;
            }
            else
            {
                maze[playerPos.first][playerPos.second] = EXIT;
            }

            if (!backtracking)
            {
                playerTrail[playerPos.first][playerPos.second] = true;
                playerPath.push({newX, newY});
            }
            else
            {
                playerTrail[playerPos.first][playerPos.second] = false;
                playerTrail[newX][newY] = false;
            }

            playerPos = {newX, newY};
            stepCount++;
            findSolutionPath();

            if (playerPos != exitPos)
            {
                maze[newX][newY] = PLAYER;
            }
        }
    }

    void clearVisited()
    {
        for (int i = 0; i < ROWS; i++)
        {
            for (int j = 0; j < COLS; j++)
            {
                if (maze[i][j] == VISITED || maze[i][j] == SOLUTION)
                {
                    maze[i][j] = PATH;
                }
            }
        }
        showingSolution = false;
    }

    void clearTrail()
    {
        for (auto &row : playerTrail)
        {
            fill(row.begin(), row.end(), false);
        }

        pair<int, int> currentPos = playerPos;
        while (!playerPath.empty())
        {
            playerPath.pop();
        }
        playerPath.push(currentPos);
    }

    void toggleSolution()
    {
        showingSolution = !showingSolution;

        if (!showingSolution)
        {
            clearVisited();
        }
    }

    bool isCompleted() const
    {
        return playerPos == exitPos;
    }

    int getRows() const { return ROWS; }
    int getCols() const { return COLS; }
    const pair<int, int> &getPlayerPosition() const { return playerPos; }
    const pair<int, int> &getExitPosition() const { return exitPos; }
    bool isShowingSolution() const { return showingSolution; }
    int getPathLength() const { return stepCount; }
    int getOptimalPathLength() const { return fullSolutionPath.size(); }
    void updateTimer()
    {
        auto currentTime = high_resolution_clock::now();
        elapsedSeconds = duration_cast<milliseconds>(currentTime - startTime).count() / 1000.0;
    }

    double getElapsedTime() const
    {
        return elapsedSeconds;
    }

    void render(Console &console, int startX, int startY)
    {
        for (int i = 0; i < ROWS; i++)
        {
            console.gotoXY(startX, startY + i);

            for (int j = 0; j < COLS; j++)
            {
                if (i == playerPos.first && j == playerPos.second)
                {
                    console.setColor(ConsoleColor::LIGHTGREEN);
                    cout << PLAYER;
                    console.resetColor();
                }
                else if (i == exitPos.first && j == exitPos.second)
                {
                    console.setColor(ConsoleColor::LIGHTMAGENTA);
                    cout << EXIT;
                    console.resetColor();
                }
                else if (showingSolution)
                {
                    bool isSolution = false;
                    for (const auto &cell : solutionPath)
                    {
                        if (cell.first == i && cell.second == j)
                        {
                            isSolution = true;
                            break;
                        }
                    }

                    if (isSolution)
                    {
                        console.setColor(ConsoleColor::LIGHTRED);
                        cout << SOLUTION;
                        console.resetColor();
                    }
                    else if (playerTrail[i][j] && maze[i][j] == PATH)
                    {
                        console.setColor(ConsoleColor::YELLOW);
                        cout << TRAIL;
                        console.resetColor();
                    }
                    else if (maze[i][j] == WALL)
                    {
                        console.setColor(ConsoleColor::BLUE);
                        cout << WALL;
                        console.resetColor();
                    }
                    else
                    {
                        cout << maze[i][j];
                    }
                }
                else if (playerTrail[i][j] && maze[i][j] == PATH)
                {
                    console.setColor(ConsoleColor::YELLOW);
                    cout << TRAIL;
                    console.resetColor();
                }
                else if (maze[i][j] == WALL)
                {
                    console.setColor(ConsoleColor::BLUE);
                    cout << WALL;
                    console.resetColor();
                }
                else
                {
                    cout << maze[i][j];
                }
            }
        }
    }
};

const char Maze::WALL;

class Game
{
private:
    Console console;
    ScoreManager scoreManager;
    Maze maze;
    Difficulty currentDifficulty;

    void printStats(int startX, int startY)
    {
        maze.updateTimer();

        console.gotoXY(startX, startY);
        console.setColor(ConsoleColor::WHITE);
        cout << "Mode: " << scoreManager.getDifficultyName(currentDifficulty);

        console.gotoXY(startX, startY + 1);
        cout << "Time: " << fixed << setprecision(1) << maze.getElapsedTime() << "s";

        console.gotoXY(startX, startY + 2);
        cout << "Path: " << maze.getPathLength() << " steps";

        console.gotoXY(startX, startY + 3);
        cout << "Optimal: " << maze.getOptimalPathLength() << " steps";

        console.resetColor();
    }

    void printLegend(int startX, int startY)
    {
        console.gotoXY(startX, startY);
        console.setColor(ConsoleColor::LIGHTGREEN);
        cout << 'P' << " - Player";

        console.gotoXY(startX, startY + 1);
        console.setColor(ConsoleColor::LIGHTMAGENTA);
        cout << 'E' << " - Exit";

        console.gotoXY(startX, startY + 2);
        console.setColor(ConsoleColor::YELLOW);
        cout << '.' << " - Trail";

        if (maze.isShowingSolution())
        {
            console.gotoXY(startX, startY + 3);
            console.setColor(ConsoleColor::LIGHTRED);
            cout << '+' << " - Solution";
        }

        console.resetColor();
    }

    void printControls(int startX, int startY)
    {
        console.gotoXY(startX, startY);
        console.setColor(ConsoleColor::LIGHTCYAN);
        cout << "WASD/Arrows - Move";

        console.gotoXY(startX, startY + 1);
        cout << "F - Show solution";

        console.gotoXY(startX, startY + 2);
        cout << "C - Clear trail";

        console.gotoXY(startX, startY + 3);
        cout << "R - Restart";

        console.gotoXY(startX, startY + 4);
        cout << "H - High scores";

        console.gotoXY(startX, startY + 5);
        cout << "Q - Quit";

        console.resetColor();
    }

    void printMaze()
    {
        int startX = 2;
        int startY = 2;
        int panelX = startX + maze.getCols() + 4;

        console.drawBox(startX - 1, startY - 1, maze.getCols() + 2, maze.getRows() + 2);

        maze.render(console, startX, startY);

        int panelHeight = max(maze.getRows() + 2, 20);

        console.drawBox(panelX, startY - 1, 30, panelHeight);

        printLegend(panelX + 4, startY + 1);
        printStats(panelX + 4, startY + 7);
        printControls(panelX + 4, startY + 12);
    }

    bool playAgainPrompt()
    {
        console.clearScreen();
        double completionTime = maze.getElapsedTime();

        bool isHighScore = scoreManager.addHighScore(currentDifficulty, completionTime);

        console.drawBox(20, 8, 40, 14);

        console.gotoXY(30, 10);
        console.setColor(ConsoleColor::YELLOW);
        cout << "MAZE COMPLETED!";

        console.gotoXY(25, 12);
        console.setColor(ConsoleColor::WHITE);
        cout << "Time: " << fixed << setprecision(2) << completionTime << " seconds";

        console.gotoXY(25, 13);
        cout << "Steps: " << maze.getPathLength();

        console.gotoXY(25, 14);
        cout << "Optimal path: " << maze.getOptimalPathLength() << " steps";

        if (isHighScore)
        {
            console.gotoXY(25, 16);
            console.setColor(ConsoleColor::LIGHTGREEN);
            cout << "NEW HIGH SCORE!";
        }

        console.gotoXY(25, 18);
        console.setColor(ConsoleColor::LIGHTCYAN);
        cout << "Play again?";

        console.gotoXY(21, 19);
        cout << "1) Same difficulty  2) New difficulty";

        console.gotoXY(32, 20);
        cout << "3) Quit";

        console.resetColor();

        char choice;
        do
        {
            choice = _getch();
        } while (choice != '1' && choice != '2' && choice != '3');

        if (choice == '1')
        {
            return true;
        }
        else if (choice == '2')
        {
            selectDifficulty();
            return true;
        }

        return false;
    }

    int mainMenu()
    {
        console.clearScreen();

        console.drawTitle("MAZE RUNNER", 4);

        console.drawBox(30, 10, 20, 10);

        console.gotoXY(35, 12);
        console.setColor(ConsoleColor::WHITE);
        cout << "MENU";

        console.gotoXY(32, 14);
        console.setColor(ConsoleColor::LIGHTGREEN);
        cout << "1. Play";

        console.gotoXY(32, 15);
        console.setColor(ConsoleColor::YELLOW);
        cout << "2. High Scores";

        console.gotoXY(32, 16);
        console.setColor(ConsoleColor::LIGHTCYAN);
        cout << "3. Help";

        console.gotoXY(32, 17);
        console.setColor(ConsoleColor::LIGHTRED);
        cout << "4. Quit";

        console.gotoXY(32, 19);
        console.setColor(ConsoleColor::LIGHTGRAY);

        console.resetColor();

        char choice;
        do
        {
            choice = _getch();
        } while (choice < '1' || choice > '4');

        return choice - '0';
    }

    void showHelp()
    {
        console.clearScreen();

        console.drawTitle("GAME HELP", 2);

        int startY = 5;

        console.gotoXY(10, startY++);
        console.setColor(ConsoleColor::WHITE);
        cout << "Welcome to MAZE RUNNER!";

        console.gotoXY(10, startY++);
        cout << "------------------------";
        startY++;

        console.gotoXY(10, startY++);
        console.setColor(ConsoleColor::LIGHTCYAN);
        cout << "HOW TO PLAY:";

        console.gotoXY(10, startY++);
        console.setColor(ConsoleColor::LIGHTGRAY);
        cout << "Navigate through the maze using WASD or Arrow keys.";

        console.gotoXY(10, startY++);
        cout << "Find the path from the starting point (";
        console.setColor(ConsoleColor::LIGHTGREEN);
        cout << "P";
        console.setColor(ConsoleColor::LIGHTGRAY);
        cout << ") to the exit (";
        console.setColor(ConsoleColor::LIGHTMAGENTA);
        cout << "E";
        console.setColor(ConsoleColor::LIGHTGRAY);
        cout << ").";

        startY++;

        console.gotoXY(10, startY++);
        console.setColor(ConsoleColor::LIGHTCYAN);
        cout << "CONTROLS:";

        console.gotoXY(10, startY++);
        console.setColor(ConsoleColor::LIGHTGRAY);
        cout << "- WASD/Arrow Keys: Move your character";

        console.gotoXY(10, startY++);
        cout << "- F: Show the solution path (";
        console.setColor(ConsoleColor::LIGHTRED);
        cout << "+";
        console.setColor(ConsoleColor::LIGHTGRAY);
        cout << ")";

        console.gotoXY(10, startY++);
        cout << "- C: Clear your trail";

        console.gotoXY(10, startY++);
        cout << "- R: Restart the current maze";

        console.gotoXY(10, startY++);
        cout << "- H: View high scores";

        console.gotoXY(10, startY++);
        cout << "- Q: Quit to menu";

        startY++;

        console.gotoXY(10, startY++);
        console.setColor(ConsoleColor::LIGHTCYAN);
        cout << "DIFFICULTY LEVELS:";

        console.gotoXY(10, startY++);
        console.setColor(ConsoleColor::LIGHTGRAY);
        cout << "- Easy: 11x19 maze, good for beginners";

        console.gotoXY(10, startY++);
        cout << "- Medium: 15x27 maze, more challenging";

        console.gotoXY(10, startY++);
        cout << "- Hard: 21x39 maze, only for experts!";

        startY++;

        console.gotoXY(10, startY++);
        console.setColor(ConsoleColor::WHITE);
        cout << "Press any key to return to the menu...";

        console.resetColor();
        _getch();
    }

    void selectDifficulty()
    {
        console.clearScreen();

        console.drawTitle("SELECT DIFFICULTY", 4);

        console.drawBox(25, 10, 30, 9);

        console.gotoXY(32, 12);
        console.setColor(ConsoleColor::WHITE);
        cout << "Select Difficulty:";

        console.gotoXY(32, 14);
        console.setColor(ConsoleColor::LIGHTGREEN);
        cout << "1. Easy";

        console.gotoXY(32, 15);
        console.setColor(ConsoleColor::YELLOW);
        cout << "2. Medium";

        console.gotoXY(32, 16);
        console.setColor(ConsoleColor::LIGHTRED);
        cout << "3. Hard";

        console.resetColor();

        char choice;
        do
        {
            choice = _getch();
        } while (choice != '1' && choice != '2' && choice != '3');

        switch (choice)
        {
        case '1':
            currentDifficulty = Difficulty::EASY;
            break;
        case '2':
            currentDifficulty = Difficulty::MEDIUM;
            break;
        case '3':
            currentDifficulty = Difficulty::HARD;
            break;
        }
    }

    void gameLoop()
    {
        bool playing = true;
        bool completed = false;
        char input;

        while (playing)
        {
            maze.init(currentDifficulty);
            completed = false;

            while (!completed && playing)
            {
                printMaze();

                input = _getch();

                switch (tolower(input))
                {
                case 'w':
                    maze.movePlayer(-1, 0);
                    break;
                case 'a':
                    maze.movePlayer(0, -1);
                    break;
                case 's':
                    maze.movePlayer(1, 0);
                    break;
                case 'd':
                    maze.movePlayer(0, 1);
                    break;
                case 'f':
                    maze.toggleSolution();
                    break;
                case 'c':
                    maze.clearTrail();
                    break;
                case 'r':
                    maze.init(currentDifficulty);
                    break;
                case 'h':
                    scoreManager.showHighScores();
                    break;
                case 'q':
                    playing = false;
                    break;
                case -32:
                case 224:
                    input = _getch();
                    switch (input)
                    {
                    case 72:
                        maze.movePlayer(-1, 0);
                        break;
                    case 75:
                        maze.movePlayer(0, -1);
                        break;
                    case 80:
                        maze.movePlayer(1, 0);
                        break;
                    case 77:
                        maze.movePlayer(0, 1);
                        break;
                    }
                    break;
                default:
                    break;
                }

                if (maze.isCompleted())
                {
                    completed = true;
                    playing = playAgainPrompt();
                    console.clearScreen();
                }
            }
        }
    }

public:
    Game() : scoreManager(console), currentDifficulty(Difficulty::EASY)
    {
        srand(static_cast<unsigned int>(time(nullptr)));
    }

    void run()
    {
        bool running = true;

        while (running)
        {
            int choice = mainMenu();

            switch (choice)
            {
            case 1:
                selectDifficulty();
                gameLoop();
                break;
            case 2:
                scoreManager.showHighScores();
                break;
            case 3:
                showHelp();
                break;
            case 4:
                running = false;
                break;
            }
        }

        console.clearScreen();
        console.setColor(ConsoleColor::YELLOW);
        console.gotoXY(30, 10);
        cout << "Thanks for playing!";
        console.gotoXY(30, 12);
        console.setColor(ConsoleColor::LIGHTGRAY);
        cout << "Press any key to exit...";
        console.resetColor();
        _getch();
    }
};

int main()
{
    Game mazeGame;
    mazeGame.run();
    return 0;
}