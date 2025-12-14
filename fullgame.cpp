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


void initGame();
void showMenu();
void selectLevel();
void playGame(const GameConfig& config, int levelId);


void showHighScores();
void saveScore(const string &nama, int score, int levelId);
vector<Pemain> loadScores();
bool compareScores(const Pemain &a, const Pemain &b);


void delay_ms(int ms);
void showSplashScreen();
void showWinAnimation(int final_steps);
void showGameOverAnimation(const string& message = "GAME OVER");
void showExitAnimation();
void showStartGameAnimation(const string& levelName, int rows, int cols);
void peekCards(const GameConfig& config, const vector<int>& board, int peekDurationSeconds);



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
                replace(display_nama.begin(), display_nama.end(), '_', ' ');

                mvprintw(7 + i, 4, "%zu. %-10s - %d langkah",
                         i + 1,
                         display_nama.c_str(),
                         filteredScores[i].score);
            }
        }

        mvprintw(15, 4, "Tekan 'x' untuk kembali ke Menu Utama...");
        refresh();

        ch = getch();

        switch (ch) {
            case '1':
                selectedLevel = 0;
                break;
            case '2':
                selectedLevel = 1;
                break;
            case '3':
                selectedLevel = 2;
                break;
            case 'x':
            case 'X':
                return;
            default:
                break;
        }
    }
}



void showMenu() {
    int choice;

    while (true) {
        clear();
        box(stdscr, 0, 0);
        mvprintw(2, 8, "GAME MEMORIZE");
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



void peekCards(const GameConfig& config, const vector<int>& board, int peekDurationSeconds) {
    timeout(-1);


    for (int t = peekDurationSeconds; t >= 0; --t) {
        clear();
        box(stdscr, 0, 0);

        mvprintw(1, 2, "Level: %s | INGAT POSISI KARTU!", config.levelName.c_str());
        mvprintw(2, 2, "Waktu untuk mengingat: %d detik", t);

        if (has_colors()) attron(COLOR_PAIR(2) | A_BOLD);
        string countdown_msg = "MULAI DALAM: " + to_string(t);
        mvprintw(LINES / 2, COLS / 2 - (int)countdown_msg.length() / 2, countdown_msg.c_str());
        if (has_colors()) attroff(COLOR_PAIR(2) | A_BOLD);
        for (int r = 0; r < config.rows; ++r) {
            for (int c = 0; c < config.cols; ++c) {
                int index = r * config.cols + c;
                int y = 5 + r * 2;
                int x = 4 + c * 6;

                attron(A_REVERSE | A_BOLD);
                mvprintw(y, x, "[%2d]", board[index]);
                attroff(A_REVERSE | A_BOLD);
            }
        }

        refresh();
        delay_ms(1000);
    }

    timeout(500);
}


void showStartGameAnimation(const string& levelName, int rows, int cols) {
    const int Y_CENTER = LINES / 2;
    const int X_CENTER = COLS / 2;
    const int DELAY = 100;
    string msg = "START LEVEL: " + levelName + " (" + to_string(rows) + "x" + to_string(cols) + ")";

    for (int i = 0; i < 5; ++i) {
        clear();
        box(stdscr, 0, 0);

        if (has_colors()) attron(COLOR_PAIR(3) | A_BOLD);
        mvprintw(Y_CENTER, X_CENTER - (int)msg.length() / 2, msg.c_str());
        if (has_colors()) attroff(COLOR_PAIR(3) | A_BOLD);

        if (i % 2 == 0) {
            if (has_colors()) attron(COLOR_PAIR(1) | A_REVERSE);
            mvprintw(Y_CENTER - 2, X_CENTER - 10, "  3  ");
            if (has_colors()) attroff(COLOR_PAIR(1) | A_REVERSE);
        } else {
            mvprintw(Y_CENTER - 2, X_CENTER - 10, "     ");
        }

        refresh();
        delay_ms(DELAY);
    }

    for (int count = 3; count > 0; --count) {
        clear();
        box(stdscr, 0, 0);
        if (has_colors()) attron(COLOR_PAIR(3) | A_BOLD);
        mvprintw(Y_CENTER, X_CENTER - (int)msg.length() / 2, msg.c_str());
        if (has_colors()) attroff(COLOR_PAIR(3) | A_BOLD);

        if (has_colors()) attron(COLOR_PAIR(2) | A_BOLD);
        string c_msg = to_string(count);
        mvprintw(Y_CENTER - 2, X_CENTER - (int)c_msg.length() / 2, c_msg.c_str());
        if (has_colors()) attroff(COLOR_PAIR(2) | A_BOLD);

        refresh();
        delay_ms(800);
    }


    clear();
    box(stdscr, 0, 0);
    if (has_colors()) attron(COLOR_PAIR(1) | A_BOLD | A_REVERSE);
    mvprintw(Y_CENTER, X_CENTER - 2, "GO!");
    if (has_colors()) attroff(COLOR_PAIR(1) | A_BOLD | A_REVERSE);
    refresh();
    delay_ms(500);
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
