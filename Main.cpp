#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <sstream>
// Ukuran papan,kotak dan penghitung timer 
const int SIZE = 9;
const int CELL_SIZE = 50;
const int TIME_LIMIT = 5 * 60;
int mistakes = 0;
// Fungsi untuk memeriksa apakah angka aman untuk dimasukkan
bool isSafe(std::vector<std::vector<int>>& board, int row, int col, int num) {
    for (int i = 0; i < SIZE; i++) {
        if (board[row][i] == num) {
            std::cout << "Number exists in row\n";
            return false;
        }
        if (board[i][col] == num) {
            std::cout << "Number exists in column\n";
            return false;
        }
        if (board[row / 3 * 3 + i / 3][col / 3 * 3 + i % 3] == num) {
            std::cout << "Number exists in 3x3 box\n";
            return false;
        }
    }
    return true;
}
// Fungsi untuk menyelesaikan Sudoku menggunakan algoritma backtracking
bool solveSudoku(std::vector<std::vector<int>>& board) {
    for (int row = 0; row < SIZE; row++) {
        for (int col = 0; col < SIZE; col++) {
            if (board[row][col] == 0) {
                std::vector<int> numbers = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
                std::random_shuffle(numbers.begin(), numbers.end());
                for (int num : numbers) {
                    if (isSafe(board, row, col, num)) {
                        board[row][col] = num;
                        if (solveSudoku(board)) return true;
                        board[row][col] = 0;
                    }
                }
                return false;
            }
        }
    }
    return true;
}
// Fungsi untuk memberikan petunjuk pada papan Sudoku
void giveHint(std::vector<std::vector<int>>& board, std::vector<std::vector<bool>>& isIncorrect) {
    for (int row = 0; row < SIZE; row++) {
        for (int col = 0; col < SIZE; col++) {
            if (board[row][col] == 0) {
                //Mencari angka yang benar untuk sel kosong 
                for (int num = 1; num <= 9; num++) {
                    if (isSafe(board, row, col, num)) {
                        board[row][col] = num; // Menyisipkan petunjuk
                        isIncorrect[row][col] = false; 
                        return; 
                    }
                }
            }
        }
    }
}
// Fungsi untuk mereset papan Sudoku
void resetBoard(std::vector<std::vector<int>>& board) {
    for (int row = 0; row < SIZE; row++) {
        for (int col = 0; col < SIZE; col++) {
            board[row][col] = 0;
        }
    }
}

void resetIncorrectCells(std::vector<std::vector<bool>>& isIncorrect) {
    for (int row = 0; row < SIZE; row++) {
        for (int col = 0; col < SIZE; col++) {
            isIncorrect[row][col] = false;
        }
    }
}

void generateRandomBoard(std::vector<std::vector<int>>& board) {
    srand(time(0));
    resetBoard(board);
    if (!solveSudoku(board)) {
        std::cerr << "Failed to generate a valid Sudoku board!" << std::endl;
        return;
    }
    int emptyCells = SIZE * SIZE - 41;
    while (emptyCells > 0) {
        int row = rand() % SIZE;
        int col = rand() % SIZE;
        if (board[row][col] != 0) {
            board[row][col] = 0;
            emptyCells--;
        }
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode(600, 600), "Si Raja Angka (Sudoku)");
    sf::Font font;
    if (!font.loadFromFile("C:\\Windows\\Fonts\\arial.ttf")) {
        std::cerr << "Error loading font\n";
        return -1;
    }
    std::vector<std::vector<int>> board(SIZE, std::vector<int>(SIZE, 0));
    std::vector<std::vector<bool>> isIncorrect(SIZE, std::vector<bool>(SIZE, false));

    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(24);
    text.setFillColor(sf::Color::Black);

    sf::RectangleShape cell(sf::Vector2f(CELL_SIZE - 2, CELL_SIZE - 2));
    cell.setFillColor(sf::Color::White);
    cell.setOutlineColor(sf::Color::Black);
    cell.setOutlineThickness(1);

    int GRID_OFFSET_X = window.getSize().x - SIZE * CELL_SIZE;
    int GRID_OFFSET_Y = window.getSize().y - SIZE * CELL_SIZE - CELL_SIZE;

    sf::RectangleShape thickLine(sf::Vector2f(SIZE * CELL_SIZE, 3));
    thickLine.setFillColor(sf::Color::Black);
    sf::Vector2i selectedCell(-1, -1);
    // Tombol untuk memulai permainan baru
    sf::RectangleShape newGameButton(sf::Vector2f(120, 60));
    newGameButton.setFillColor(sf::Color(70, 130, 180));
    newGameButton.setOutlineColor(sf::Color::Black);
    newGameButton.setOutlineThickness(2);
    newGameButton.setPosition(20, 20);

    sf::Text newGameText;
    newGameText.setFont(font);
    newGameText.setCharacterSize(18);
    newGameText.setFillColor(sf::Color::White);
    newGameText.setString("New Game");
    newGameText.setPosition(30, 30);
    // Tombol Hint
    sf::RectangleShape hintButton(sf::Vector2f(120, 60));
    hintButton.setFillColor(sf::Color(70, 130, 180));
    hintButton.setOutlineColor(sf::Color::Black);
    hintButton.setOutlineThickness(2);
    hintButton.setPosition(20, 90);

    sf::Text hintText;
    hintText.setFont(font);
    hintText.setCharacterSize(18);
    hintText.setFillColor(sf::Color::White);
    hintText.setString("Hint");
    hintText.setPosition(50, 100);

    sf::Text mistakesText;
    mistakesText.setFont(font);
    mistakesText.setCharacterSize(20);
    mistakesText.setFillColor(sf::Color::Red);
    mistakesText.setPosition(20, window.getSize().y - 50);

    bool buttonPressed = false;
    bool gameStarted = false;
    bool gameOver = false;

    sf::Clock clock;
    sf::Time elapsedTime;
    int timeRemaining = TIME_LIMIT;
    sf::Text timerText;

    timerText.setFont(font);
    timerText.setCharacterSize(20);
    timerText.setFillColor(sf::Color::Black);
    timerText.setPosition(window.getSize().x - 150, 20);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                int mouseX = event.mouseButton.x;
                int mouseY = event.mouseButton.y;
                int col = (mouseX - GRID_OFFSET_X) / CELL_SIZE;
                int row = (mouseY - GRID_OFFSET_Y) / CELL_SIZE;
                if (row >= 0 && row < SIZE && col >= 0 && col < SIZE) {
                    selectedCell = sf::Vector2i(row, col);
                }
                if (newGameButton.getGlobalBounds().contains(mouseX, mouseY)) {
                    generateRandomBoard(board);
                    resetIncorrectCells(isIncorrect);
                    gameStarted = true;
                    gameOver = false;
                    clock.restart();
                    timeRemaining = TIME_LIMIT;
                    mistakes = 0;
                    buttonPressed = true;
                }
                // Menambahkan logika untuk tombol Hint
                if (hintButton.getGlobalBounds().contains(mouseX, mouseY)) {
                    giveHint(board, isIncorrect); // Memberikan petunjuk
                }
            }
            if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
                buttonPressed = false;
            }
            if (event.type == sf::Event::KeyPressed && selectedCell.x != -1 && selectedCell.y != -1) {
                if (event.key.code >= sf::Keyboard::Num1 && event.key.code <= sf::Keyboard::Num9) {
                    int num = event.key.code - sf::Keyboard::Num1 + 1;
                    int row = selectedCell.x;
                    int col = selectedCell.y;

                    if (isSafe(board, row, col, num)) {
                        board[row][col] = num;
                        isIncorrect[row][col] = false;
                        mistakes++;
                    }
                    else {
                        board[row][col] = num;
                        isIncorrect[row][col] = true;
                    }
                }
                else if (event.key.code == sf::Keyboard::BackSpace) {
                    int row = selectedCell.x;
                    int col = selectedCell.y;
                    board[row][col] = 0;
                    isIncorrect[row][col] = false;
                }
            }
        }
        std::stringstream mistakesStream;
        mistakesStream << "Mistakes: " << mistakes;
        mistakesText.setString(mistakesStream.str());
        window.draw(mistakesText);
        if (gameStarted && !gameOver) {
            elapsedTime = clock.getElapsedTime();
            timeRemaining = TIME_LIMIT - static_cast<int>(elapsedTime.asSeconds());
            if (timeRemaining <= 0) {
                gameOver = true;
                resetBoard(board);
            }
        }
        window.clear(sf::Color(169, 186, 197));;
        // Menggambar papan Sudoku
        for (int row = 0; row < SIZE; row++) {
            for (int col = 0; col < SIZE; col++) {
                if (isIncorrect[row][col]) {
                    cell.setFillColor(sf::Color::Red);
                }
                else if (selectedCell.x == row && selectedCell.y == col) {
                    cell.setFillColor(sf::Color(173, 216, 230));
                }
                else {
                    cell.setFillColor(sf::Color::White);
                }
                cell.setPosition(GRID_OFFSET_X + col * CELL_SIZE, GRID_OFFSET_Y + row * CELL_SIZE);
                window.draw(cell);
                if (board[row][col] != 0) {
                    text.setString(std::to_string(board[row][col]));
                    text.setPosition(GRID_OFFSET_X + col * CELL_SIZE + CELL_SIZE / 4, GRID_OFFSET_Y + row * CELL_SIZE + CELL_SIZE / 4);
                    window.draw(text);
                }
            }
        }
        for (int i = 0; i <= SIZE; i++) {
            if (i % 3 == 0) {
                thickLine.setSize(sf::Vector2f(SIZE * CELL_SIZE, 3));
                thickLine.setPosition(GRID_OFFSET_X, GRID_OFFSET_Y + i * CELL_SIZE - 1);
                window.draw(thickLine);

                thickLine.setSize(sf::Vector2f(3, SIZE * CELL_SIZE));
                thickLine.setPosition(GRID_OFFSET_X + i * CELL_SIZE - 1, GRID_OFFSET_Y);
                window.draw(thickLine);
            }
        }
        // Menggambar tombol baru
        if (buttonPressed) {
            newGameButton.setFillColor(sf::Color(50, 100, 150));
        }
        else {
            newGameButton.setFillColor(sf::Color(70, 130, 180));
        }
        window.draw(newGameButton);
        window.draw(newGameText);
        window.draw(hintButton);
        window.draw(hintText);
        window.draw(mistakesText);
        // Menampilkan waktu
        if (gameStarted && !gameOver) {
            int minutes = timeRemaining / 60;
            int seconds = timeRemaining % 60;
            if (timeRemaining <= 60) {
                int blinkInterval = 500; // Interval kedipan dalam milidetik (500 ms)
                int currentTime = static_cast<int>(elapsedTime.asMilliseconds());

                // Cek apakah waktu saat ini genap atau ganjil untuk berkedip
                if ((currentTime / blinkInterval) % 2 == 0) {
                    timerText.setFillColor(sf::Color::Red);  // Ubah warna menjadi merah
                }
                else {
                    timerText.setFillColor(sf::Color::Transparent); // Sembunyikan teks (berkedip)
                }
            }
            else {
                timerText.setFillColor(sf::Color::Black); // Kembalikan warna menjadi hitam jika lebih dari 1 menit
            }
            std::stringstream timerStream;
            timerStream << "Time: " << minutes << ":" << (seconds < 10 ? "0" : "") << seconds;
            timerText.setString(timerStream.str());
            window.draw(timerText);
        }
        else if (gameOver) {
            timerText.setString("Waktumu habis!!!!!");
            window.draw(timerText);
        }
        window.display();
    }
    return 0;
}
