#include <iostream>
#include <string>
#include <vector>
#include <ncurses/curses.h>
#include <cstring>

using namespace std;

struct GameConfig {
    string levelName;
    int rows;
    int cols;
    int timeLimitSeconds;
    int totalPairs;
};


void delay_ms(int ms) {
    napms(ms);
}

void showSplashScreen() {
    const int Y_CENTER = LINES / 2;
    const int X_CENTER = COLS / 2;
    const int DELAY = 100;

    clear();
    refresh();

    mvprintw(Y_CENTER, X_CENTER - 10, "* *");
    refresh();
    delay_ms(DELAY * 3);

    mvprintw(Y_CENTER, X_CENTER - 10, "* MEMORIZE *");
    refresh();
    delay_ms(DELAY * 5);

    mvprintw(Y_CENTER,        X_CENTER - 10, "* MEMORIZE *");
    mvprintw(Y_CENTER + 1, X_CENTER - 10, "* GAME     *");
    refresh();
    delay_ms(DELAY * 5);

    mvprintw(Y_CENTER + 3, X_CENTER - 5, "LOADING...");
    refresh();
    delay_ms(DELAY * 10);
}

void showWinAnimation(int final_steps) {
    const int Y_CENTER = LINES / 2;
    const int X_CENTER = COLS / 2;
    const int DELAY = 100;

    for (int i = 0; i < 4; ++i) {
        clear();
        if (has_colors()) attron(COLOR_PAIR(1) | A_BOLD);
        mvprintw(Y_CENTER, X_CENTER - 15 - i, "~~~ PASANGAN DITEMUKAN! ~~~");
        if (has_colors()) attroff(COLOR_PAIR(1) | A_BOLD);
        refresh();
        delay_ms(DELAY);
    }

    clear();
    if (has_colors()) attron(COLOR_PAIR(1) | A_BOLD);
    mvprintw(Y_CENTER, X_CENTER - 15, "~~~ PASANGAN DITEMUKAN! ~~~");
    if (has_colors()) attroff(COLOR_PAIR(1) | A_BOLD);


    mvprintw(Y_CENTER + 3, X_CENTER - 20,
             "SELAMAT! Anda menang dalam %d langkah!", final_steps);
    mvprintw(Y_CENTER + 5, X_CENTER - 20,
             "Tekan tombol apa saja untuk lanjut...");
    refresh();
    getch();
}

void showGameOverAnimation(const string& message) {
    const int Y_CENTER = LINES / 2;
    const int X_CENTER = COLS / 2;
    const int DELAY = 200;

    clear();
    refresh();

    for (int i = 0; i < 5; ++i) {
        if (has_colors()) attron(COLOR_PAIR(2) | A_BOLD);
        mvprintw(Y_CENTER, X_CENTER - 5, "GAME OVER");
        if (has_colors()) attroff(COLOR_PAIR(2) | A_BOLD);
        refresh();
        delay_ms(DELAY);

        mvprintw(Y_CENTER, X_CENTER - 5, "         ");
        refresh();
        delay_ms(DELAY);
    }

    if (has_colors()) attron(COLOR_PAIR(2) | A_BOLD);
    mvprintw(Y_CENTER,        X_CENTER - 5, "GAME OVER");
    if (has_colors()) attroff(COLOR_PAIR(2) | A_BOLD);

    mvprintw(Y_CENTER + 2, X_CENTER - (int)message.length() / 2, message.c_str());

    mvprintw(Y_CENTER + 4, X_CENTER - 12, "Tekan tombol apa saja untuk kembali...");
    refresh();
    getch();
}


void showExitAnimation() {
    const int Y_START = LINES - 2;
    const int Y_CENTER = LINES / 2;
    const int X_CENTER = COLS / 2;
    const int DELAY = 80;

    const char* message1 = "Terima Kasih telah Bermain!";
    const char* message2 = "Sampai Jumpa...";

    for (int y = Y_START; y >= Y_CENTER; --y) {
        clear();

        int x1 = X_CENTER - (int)strlen(message1) / 2;
        int x2 = X_CENTER - (int)strlen(message2) / 2;

        attron(A_DIM);
        mvprintw(y, x1, message1);
        mvprintw(y + 2, x2, message2);
        attroff(A_DIM);

        refresh();
        delay_ms(DELAY);
    }

    delay_ms(DELAY * 10);

    for (int i = 0; i < 3; ++i) {
        clear();
        refresh();
        delay_ms(DELAY * 3);

        attron(A_BOLD);
        mvprintw(Y_CENTER, X_CENTER - (int)strlen(message1) / 2, message1);
        mvprintw(Y_CENTER + 2, X_CENTER - (int)strlen(message2) / 2, message2);
        attroff(A_BOLD);
        refresh();
        delay_ms(DELAY * 3);
    }

    clear();
    refresh();
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
