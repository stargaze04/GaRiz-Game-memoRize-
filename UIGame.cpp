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

extern const vector<GameConfig> LEVEL_CONFIGS;

void playGame(const GameConfig& config, int levelId);
void showExitAnimation();
vector<Pemain> loadScores();
bool compareScores(const Pemain &a, const Pemain &b);
void showSplashScreen();

void showMenu();
void selectLevel();
void showHighScores();

int main() {
    initGame();
    showSplashScreen();
    showMenu();
    endwin();
    return 0;
}


void initGame() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    srand(time(0));

    if (has_colors()) {
        start_color();
        init_pair(1, COLOR_YELLOW, COLOR_BLACK);
        init_pair(2, COLOR_RED, COLOR_BLACK);
        init_pair(3, COLOR_CYAN, COLOR_BLACK);
    }
}

void showMenu() {
    int choice;

    while (true) {
        clear();
        box(stdscr, 0, 0);
        mvprintw(2, 8, "GAME MEMORIZE (NCURSES)");
        mvprintw(4, 8, "1. Main Game");
        mvprintw(5, 8, "2. Lihat High Score");
        mvprintw(6, 8, "3. Keluar");
        mvprintw(8, 8, "Pilih (1-3): ");
        refresh();

        choice = getch();

        switch (choice) {
            case '1':
                selectLevel();
                break;
            case '2':
                showHighScores();
                break;
            case '3':
                showExitAnimation();
                return;
            default:
                break;
        }
    }
}

void selectLevel() {
    int choice;
    while (true) {
        clear();
        box(stdscr, 0, 0);
        mvprintw(2, 8, "PILIH LEVEL PERMAINAN");

        for (size_t i = 0; i < LEVEL_CONFIGS.size(); ++i) {
            const auto& config = LEVEL_CONFIGS[i];
            int minutes = config.timeLimitSeconds / 60;
            mvprintw(4 + i, 8, "%zu. %s (%dx%d kartu, %d menit)",
                     i + 1,
                     config.levelName.c_str(),
                     config.rows,
                     config.cols,
                     minutes);
        }

        mvprintw(9, 8, "4. Kembali ke Menu Utama");
        mvprintw(11, 8, "Pilih (1-4): ");
        refresh();

        choice = getch();

        switch (choice) {
            case '1':
            case '2':
            case '3': {
                int levelIndex = choice - '1';
                playGame(LEVEL_CONFIGS[levelIndex], levelIndex);
                return;
            }
            case '4':
                return;
            default:
                break;
        }
    }
}

void showHighScores() {
    vector<Pemain> allScores = loadScores();
    int selectedLevel = 0;
    int ch;

    while (true) {
        clear();
        box(stdscr, 0, 0);

        mvprintw(1, 4, "=== TOP PLAYERS (Tekan 1-3 untuk ganti Level) ===");


        for (size_t i = 0; i < LEVEL_CONFIGS.size(); ++i) {
            string levelName = LEVEL_CONFIGS[i].levelName;
            int x_pos = 5 + (int)i * 15;
            if (i == (size_t)selectedLevel) {
                if (has_colors()) attron(COLOR_PAIR(1) | A_BOLD | A_REVERSE);
                mvprintw(3, x_pos, "[%s]", levelName.c_str());
                if (has_colors()) attroff(COLOR_PAIR(1) | A_BOLD | A_REVERSE);
            } else {
                mvprintw(3, x_pos, " %s ", levelName.c_str());
            }
        }

        mvprintw(4, 4, "-------------------------------------------------");
        mvprintw(5, 4, "High Score Level: %s", LEVEL_CONFIGS[selectedLevel].levelName.c_str());


        vector<Pemain> filteredScores;
        for (const auto& p : allScores) {
            if (p.levelId == selectedLevel) {
                filteredScores.push_back(p);
            }
        }
        sort(filteredScores.begin(), filteredScores.end(), compareScores);

        if (filteredScores.empty()) {
            mvprintw(7, 4, "Belum ada data highscore untuk level ini.");
        } else {
            for (size_t i = 0; i < filteredScores.size() && i < 5; ++i) {
                string display_nama = filteredScores[i].nama;
                replace(display_nama.begin(), display_nama.end(), '_',