#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <ctime>
#include <cstdlib>
#include <ncurses/curses.h>
#include <cstring>
#include <sstream>

using namespace std;

struct Pemain {
    string nama;
    int score;
    int levelId;
};

struct GameConfig {
    string levelName;
    int rows;
    int cols;
    int timeLimitSeconds;
    int totalPairs;
};

const vector<GameConfig> LEVEL_CONFIGS = {
    {"Mudah", 4, 3, 60, 6},
    {"Sedang", 5, 4, 120, 10},
    {"Sulit", 6, 5, 180, 15}
};

const string FILE_HIGHSCORE = "highscore_v2.txt";

void delay_ms(int ms);
void showWinAnimation(int final_steps);
void showGameOverAnimation(const string& message = "GAME OVER");
void showStartGameAnimation(const string& levelName, int rows, int cols);
void peekCards(const GameConfig& config, const vector<int>& board, int peekDurationSeconds);


bool compareScores(const Pemain &a, const Pemain &b) {
    return a.score < b.score;
}

vector<Pemain> loadScores() {
    vector<Pemain> data;
    ifstream file(FILE_HIGHSCORE);

    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            string nama;
            int score, levelId;
            if (ss >> nama >> score >> levelId) {
                data.push_back({nama, score, levelId});
            }
        }
        file.close();
    }
    return data;
}

void saveScore(const string &nama, int score, int levelId) {
    ofstream file(FILE_HIGHSCORE, ios::app);
    if (file.is_open()) {
        string sanitized_nama = nama;
        replace(sanitized_nama.begin(), sanitized_nama.end(), ' ', '_');

        file << sanitized_nama << " " << score << " " << levelId << endl;
        file.close();
    }
}


void playGame(const GameConfig& config, int levelId) {
    vector<int> board;
    int totalPairs = config.totalPairs;
    int totalCards = config.rows * config.cols;

    if (totalCards != totalPairs * 2) {
        showGameOverAnimation("Error: Konfigurasi level salah.");
        return;
    }

    for (int i = 1; i <= totalPairs; ++i) {
        board.push_back(i);
        board.push_back(i);
    }

    for (int i = (int)board.size() - 1; i > 0; --i) {
        int j = rand() % (i + 1);
        swap(board[i], board[j]);
    }

    int peekDuration = 0;
    if (config.levelName == "Mudah") peekDuration = 5;
    else if (config.levelName == "Sedang") peekDuration = 7;
    else if (config.levelName == "Sulit") peekDuration = 10;

    showStartGameAnimation(config.levelName, config.rows, config.cols);

    peekCards(config, board, peekDuration);


    vector<bool> matched(totalCards, false);

    int steps = 0;
    int cursorRow = 0;
    int cursorCol = 0;

    int firstIndex  = -1;
    int secondIndex = -1;

    int matchedPairs = 0;
    bool surrendered = false;
    bool timedOut = false;

    time_t startTime = time(0);

    auto drawBoard = [&]() {
        clear();
        box(stdscr, 0, 0);

        time_t currentTime = time(0);
        long elapsed = currentTime - startTime;
        long timeRemaining = config.timeLimitSeconds - elapsed;

        if (timeRemaining <= 0) {
            timedOut = true;
            return;
        }

        int minutes = timeRemaining / 60;
        int seconds = timeRemaining % 60;

        mvprintw(1, 2, "Level: %s | Langkah: %d | Pasangan: %d/%d",
                 config.levelName.c_str(), steps, matchedPairs, totalPairs);

        if (timeRemaining <= 10) {
            if (has_colors()) attron(COLOR_PAIR(2) | A_BOLD);
        }
        mvprintw(2, 2, "Waktu Sisa: %02d:%02d", minutes, seconds);
        if (timeRemaining <= 10 && has_colors()) {
            attroff(COLOR_PAIR(2) | A_BOLD);
        }

        mvprintw(3, 2, "Panah: gerak | ENTER/Spasi: pilih | q: menyerah");


        for (int r = 0; r < config.rows; ++r) {
            for (int c = 0; c < config.cols; ++c) {
                int index = r * config.cols + c;
                int y = 5 + r * 2;
                int x = 4 + c * 6;

                bool isCursor      = (r == cursorRow && c == cursorCol);
                bool isRevealed = matched[index] ||
                                              (index == firstIndex) ||
                                              (index == secondIndex);

                if (isCursor) {
                    attron(A_REVERSE);
                }

                if (matched[index]) {
                    attron(A_BOLD);
                }
