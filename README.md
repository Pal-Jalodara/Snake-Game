# 🐍 Snake Game (C++)

## 🎮 About the Project
This is a **console-based Snake game** written in C++. The player controls a snake using **W, A, S, D** keys, trying to eat food (`*`) while avoiding collisions with the walls and itself. The game includes **different difficulty levels, a high score system, and restart options**.

---

## 🛠 Features
- **Classic Snake Gameplay** – The snake moves continuously, grows when it eats food, and dies when it hits the wall or itself.
- **3 Difficulty Levels** – Easy, Medium, and Hard (smaller grids & faster speed).
- **Dynamic Food Generation** – Food appears in random locations after being eaten.
- **Score Tracking** – Keeps track of the player's current score.
- **Restart & Mode Selection** – Players can restart with the same or different difficulty after game over.
- **Countdown Before Restart** – Gives players time to prepare before restarting.

---

## 🏆 High Score System
- The game **automatically saves** the highest score in a file called `highscore.txt`.
- When you beat the previous high score, it **updates the file**.
- The high score persists even after restarting the game.

---

## 🚀 Installation & Running the Game
### 🔹 Requirements
- A **C++ compiler** (GCC, MinGW, or MSVC)
- Windows OS (for `_kbhit()` and `Sleep()` functions)

### 🔹 Steps to Run
1️⃣ Clone the repository or download the code.
```
git clone https://github.com/Pal-Jalodara/Snake-Game.git
```
2️⃣ Navigate to the project folder:
```
cd Snake-Game
```
3️⃣ Compile the program using a C++ compiler:
```
g++ snakeGame.cpp -o snakeGame.exe
```
4️⃣ Run the executable:
```
./snakeGame.exe
```

---

## 📜 How to Play
1️⃣ Run the program.  
2️⃣ Select a difficulty level (**1 - Easy, 2 - Medium, 3 - Hard**).  
3️⃣ Control the snake using:
   - `W` → Move **Up**
   - `S` → Move **Down**
   - `A` → Move **Left**
   - `D` → Move **Right**  
4️⃣ Eat food (`*`) to grow and increase your score.  
5️⃣ Avoid hitting the walls or yourself!  
6️⃣ If you collide, the game ends. Choose to restart or exit.

---

## 🔧 Possible Improvements
- 🎭 **Customizable Snake Skins**
- 🧱 **Obstacles for More Challenge**
- 🌀 **Power-Ups (Speed Boost, Slow Motion, etc.)**
- 🎮 **Multiplayer Mode**
- 📊 **Leaderboard System (Top 5 Scores)**

---
## 👥 Developed by **Tech Titans**

- **Pal Jalodara** - ID: 202401074
- **Ved Dhanani** - ID: 202401048
- **Dev Sanghani** - ID: 202401047
- **Kevin Rank** - ID: 202401091

Happy coding! 🎮🐍
