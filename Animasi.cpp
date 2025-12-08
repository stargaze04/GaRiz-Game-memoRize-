// ==========================================
//           FUNGSI-FUNGSI ANIMASI
// ==========================================

// Fungsi Helper untuk Jeda waktu (perlu #include <chrono> dan #include <thread>
// Jika compiler Anda tidak mendukung std::this_thread::sleep_for,
// Anda mungkin perlu menggunakan fungsi sleep() dari <unistd.h> (Linux) atau Sleep() dari <windows.h>
#include <chrono>
#include <thread>

// Fungsi untuk menunda eksekusi (milidetik)
void delay_ms(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

// 1. Splash Screen Animasi (Awal Game)
void showSplashScreen() {
    const int Y_CENTER = LINES / 2;
    const int X_CENTER = COLS / 2;
    const int DELAY = 100; // Jeda 100ms

    clear();
    refresh();

    // Frame 1: Garis Tengah
    mvprintw(Y_CENTER - 1, X_CENTER - 15, "");
    mvprintw(Y_CENTER, X_CENTER - 15, "* *");
    mvprintw(Y_CENTER + 1, X_CENTER - 15, "");
    refresh();
    delay_ms(DELAY * 3);

    // Frame 2: Teks "MEMORIZE"
    mvprintw(Y_CENTER, X_CENTER - 15, "* *MEMORIZE* *");
    refresh();
    delay_ms(DELAY * 5);

    // Frame 3: Teks "GAME"
    mvprintw(Y_CENTER, X_CENTER - 15, "* MEMORIZE GAME       *");
    refresh();
    delay_ms(DELAY * 5);

    // Frame 4: Teks "LOADING..."
    mvprintw(Y_CENTER + 3, X_CENTER - 5, "LOADING...");
    refresh();
    delay_ms(DELAY * 10);
}

// 2. Animasi Kemenangan (Win Animation)
// Bisa dipanggil saat pemain berhasil menemukan semua pasangan
void showWinAnimation(int final_steps) {
    const int Y_CENTER = LINES / 2;
    const int X_CENTER = COLS / 2;
    const int DELAY = 150;

    // Animasi Pengecilan Teks (Zoom-out Effect)
    for (int i = 0; i < 3; ++i) {
        clear();
        mvprintw(Y_CENTER - i, X_CENTER - 15 + i, "--- PASANGAN DITEMUKAN! ---");
        mvprintw(Y_CENTER + i, X_CENTER - 15 + i, "--- PASANGAN DITEMUKAN! ---");
        refresh();
        delay_ms(DELAY);
    }

    // Tampilkan Score
    mvprintw(Y_CENTER + 4, X_CENTER - 15, "SELAMAT! Anda Menang dalam %d Langkah!", final_steps);
    mvprintw(Y_CENTER + 6, X_CENTER - 15, "Tekan tombol apa saja untuk menyimpan...");
    refresh();
    getch();
}

// 3. Animasi Game Over (GameOver/Forfeit Animation)
// Bisa dipanggil saat pemain menyerah atau saat game logic mendeteksi kalah (jika ada)
void showGameOverAnimation() {
    const int Y_CENTER = LINES / 2;
    const int X_CENTER = COLS / 2;
    const int DELAY = 200;

    clear();
    refresh();

    // Animasi Berkedip
    for (int i = 0; i < 5; ++i) {
        mvprintw(Y_CENTER, X_CENTER - 5, "GAME OVER");
        refresh();
        delay_ms(DELAY);
        
        mvprintw(Y_CENTER, X_CENTER - 5, "         "); // Hapus Teks
        refresh();
        delay_ms(DELAY);
    }

    // Tampilkan Teks Final
    mvprintw(Y_CENTER, X_CENTER - 5, "GAME OVER");
    mvprintw(Y_CENTER + 2, X_CENTER - 10, "Kembali ke Menu Utama...");
    refresh();
    delay_ms(DELAY * 5);
}
