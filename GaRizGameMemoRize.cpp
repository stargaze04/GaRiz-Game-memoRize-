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
