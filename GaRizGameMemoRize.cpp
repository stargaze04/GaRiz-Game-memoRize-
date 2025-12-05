#include <iostream>
#include <vector>
#include <string>
#include <algorithm> // Untuk sorting
#include <fstream>   // Untuk File I/O
#include <ctime>
#include <cstdlib>
#include <ncurses\curses.h> // Library UI

using namespace std;

// --- STRUKTUR DATA ---
struct Pemain
{
    string nama;
    int score; // Semakin sedikit langkah, semakin baik
};

// --- GLOBAL VARIABLES ---
const string FILE_HIGHSCORE = "highscore.txt";
const int GAME_ROWS = 4; // Ubah ROWS jadi GAME_ROWS
const int GAME_COLS = 4; // Ubah COLS jadi GAME_COLS (karena COLS milik ncurses)
// --- PROTOTIPE FUNGSI ---
void initGame();
void showMenu();
void playGame();
void showHighScores();
void saveScore(string nama, int score);
vector<Pemain> loadScores();
bool compareScores(const Pemain &a, const Pemain &b);

// ==========================================
//                 MAIN PROGRAM
// ==========================================
int main()
{
    initGame(); // Setup Ncurses
    showMenu(); // Masuk ke Loop Menu Utama
    endwin();   // Tutup Ncurses saat keluar
    return 0;
}

// ==========================================
//              FUNGSI-FUNGSI
// ==========================================

// 1. Setup Ncurses
void initGame()
{
    initscr();            // Mulai mode ncurses
    cbreak();             // Disable line buffering
    noecho();             // Jangan tampilkan input keyboard
    keypad(stdscr, TRUE); // Enable tombol F1, Arrow keys, dll
    curs_set(0);          // Hilangkan kursor kedip-kedip
    srand(time(0));       // Seed untuk random
}

// 2. Fungsi Sorting (Comparator)
// Digunakan untuk std::sort
bool compareScores(const Pemain &a, const Pemain &b)
{
    return a.score < b.score; // Ascending (Score kecil = langkah sedikit = lebih baik)
}

// 3. File Input: Membaca Score
vector<Pemain> loadScores()
{
    vector<Pemain> data; // ARRAY (Vector)
    ifstream file(FILE_HIGHSCORE);

    if (file.is_open())
    {
        string nama;
        int score;
        // PERULANGAN (While) membaca file
        while (file >> nama >> score)
        {
            data.push_back({nama, score});
        }
        file.close();
    }
    return data;
}

// 4. File Output: Menyimpan Score
void saveScore(string nama, int score)
{
    ofstream file(FILE_HIGHSCORE, ios::app); // Append mode
    if (file.is_open())
    {
        file << nama << " " << score << endl;
        file.close();
    }
}

// 5. Menampilkan High Score (Sorting & Array & Loop)
void showHighScores()
{
    clear();
    mvprintw(1, 10, "=== TOP PLAYERS ===");

    // ARRAY & FILE INPUT
    vector<Pemain> scores = loadScores();

    // PERCABANGAN
    if (scores.empty())
    {
        mvprintw(3, 5, "Belum ada data highscore.");
    }
    else
    {
        // SORTING
        sort(scores.begin(), scores.end(), compareScores);

        // PERULANGAN (Menampilkan data)
        for (size_t i = 0; i < scores.size() && i < 5; ++i)
        { // Top 5
            mvprintw(3 + i, 5, "%d. %s - %d Langkah", i + 1, scores[i].nama.c_str(), scores[i].score);
        }
    }

    mvprintw(10, 5, "Tekan sembarang tombol untuk kembali...");
    refresh();
    getch();
}

// 6. UI Menu Utama (Percabangan & Perulangan)
void showMenu()
{
    int choice;
    while (true)
    { // PERULANGAN MENU
        clear();
        box(stdscr, 0, 0); // Membuat border
        mvprintw(2, 10, "GAME MEMORIZE (NCURSES)");
        mvprintw(4, 10, "1. Main Game");
        mvprintw(5, 10, "2. Lihat High Score");
        mvprintw(6, 10, "3. Keluar");
        mvprintw(8, 10, "Pilih (1-3): ");
        refresh();

        choice = getch();
// PERCABANGAN (Switch Case)
        switch (choice)
        {
        case '1':
            playGame();
            break;
        case '2':
            showHighScores();
            break;
        case '3':
            return; // Keluar dari fungsi showMenu -> program selesai
        default:
            break;
        }
    }
}

// 7. Core Game Logic (UI Board & Array)
void playGame()
{
    // Validasi Ukuran Papan (Mencegah Crash jika ganjil)
    if ((GAME_ROWS * GAME_COLS) % 2 != 0)
    {
        clear();
        mvprintw(0, 0, "Error: Jumlah kotak (Rows x Cols) harus genap!");
        mvprintw(1, 0, "Tekan tombol apa saja untuk kembali...");
        refresh();
        getch();
        return;
    }

    // 1. Setup Papan Permainan (Board)
    vector<int> board;
    int totalPairs = (GAME_ROWS * GAME_COLS) / 2;

    // Isi board dengan pasangan angka
    for (int i = 1; i <= totalPairs; ++i)
    {
        board.push_back(i);
        board.push_back(i);
    }

    // Acak posisi kartu
    // random_shuffle deprecated di C++ modern, kita gunakan iterasi acak sederhana
    random_shuffle(board.begin(), board.end());

    // 2. Setup Status Kartu (SAFE VERSION)
    // Menggunakan vector<bool> jauh lebih aman daripada array bool[]
    // Ini mencegah error "stack overflow" atau "VLA not supported"
    vector<bool> opened(GAME_ROWS * GAME_COLS, false);

    int steps = 0;
    bool gameOver = false;

    // 3. UI Loop Game
    while (!gameOver)
    {
        clear();
        mvprintw(0, 2, "Cari Pasangan Kartu! (Tekan 'q' untuk menyerah)");

        // Loop untuk menggambar Grid
        for (int i = 0; i < GAME_ROWS; ++i)
        {
            for (int j = 0; j < GAME_COLS; ++j)
            {
                int index = i * GAME_COLS + j;

                // Koordinat tampilan di layar
                int y = 2 + i * 3;
                int x = 4 + j * 6;

                // SAFETY CHECK: Pastikan index tidak melebihi ukuran board
                if (index < board.size())
                {
                    if (opened[index])
                    {
                        // Tampilkan angka jika terbuka
                        mvprintw(y, x, "[%d]", board[index]);
                    }
                    else
                    {
                        // Tampilkan tanda tanya jika tertutup
                        mvprintw(y, x, "[?]");
                    }
                }
            }
        }

        // Info Simulasi
        mvprintw(15, 2, "Demo: Tekan 'q' untuk kembali ke menu.");
        mvprintw(16, 2, "Tekan tombol lain untuk mensimulasikan 'Menang'.");
        refresh();

        // Input User
        int ch = getch();
        if (ch == 'q' || ch == 'Q')
            return; // Kembali ke menu tanpa crash

        // Simulasi Menang (Untuk tes UI)
        gameOver = true;
        steps = rand() % 50 + 10;
    }

    // 4. Game Over Screen
    // Matikan mode raw ncurses sebentar untuk input nama yang aman
    echo();
    curs_set(1);

    char playerName[50];

    clear();
    mvprintw(5, 5, "GAME OVER!");
    mvprintw(6, 5, "Total Langkah: %d", steps);
    mvprintw(8, 5, "Masukkan Nama Anda: ");
    refresh();

    getstr(playerName); // Ambil input nama

    // Simpan skor
    saveScore(string(playerName), steps);

    // Kembalikan setting ncurses
    noecho();
    curs_set(0);
}
