#ifndef MAINWINDOW_HH
#define MAINWINDOW_HH

#include <QMainWindow>
#include <QPushButton>
#include <QTimer>
#include <vector>
#include "gameboard.hh"

// Structure to represent a button and its color
struct Button{
    QPushButton* button;
    int color;
    Button(QPushButton* button):button(button){};
};

// Alias for storing the history of moves
using History = std::vector<std::vector<Point>>;
// Alias for storing the buttons on the game board
using Buttons = std::vector<std::vector<Button>>;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QTimer *timer;
    Buttons buttons_;  // Stores the game buttons
    History history;   // Stores the history of moves
    GameBoard grid;    // Represents the game board's state
    GameBoard org;     // Stores the original game board's state
    int counter = 0;   // Counts the number of moves
    bool blinkingBackground = false;  // Tracks the blinking background state
    QTimer *blinkTimer;   // Timer for blinking background
    bool stop = false;    // Tracks if the game is paused

    /**
    * @brief Initializes the game buttons on the main window.
    */
    void init_buttons();

    /**
    * @brief Initializes the colors of the game buttons.
    */
    void init_color();

    /**
    * @brief Checks movable positions for a specific button.
    * @param x The x-coordinate of the button on the game board.
    * @param y The y-coordinate of the button on the game board.
    */
    void check_movable(unsigned int x, unsigned int y);

    /**
     * @brief Executes a move on the game board.
     * @param x The x-coordinate of the button on the game board.
     * @param y The y-coordinate of the button on the game board.
     */
    void move(unsigned int x, unsigned int y);

    /**
    * @brief Resets the game state.
    */
    void reset();

    /**
    * @brief Disables all game buttons.
    */
    void disable_all();

    /**
    * @brief Updates the LCD display.
    * @param reset Whether to reset the display or not.
    */
    void updateLCD(bool reset = false);

    /**
    * @brief Pauses the game.
    */
    void pause_game();

    /**
    * @brief Continues the game after pausing.
    */
    void continue_game();

    /**
    * @brief Undoes the last move.
    */
    void undo();

    /**
    * @brief Handles winning the game.
    */
    void win_game();

    /**
     * @brief Toggles the blinking background color.
     */
    void toggleBlinkBackgroundColor();

    /**
    * @brief Checks if a button is nullptr.
    * @param button The QPushButton to check.
    * @return True if the button is nullptr, otherwise false.
    */
    bool check_nullptr(QPushButton* button);

    unsigned int previous_x = -1;  // Stores the previous clicked button's X position
    unsigned int previous_y = -1;  // Stores the previous clicked button's Y position
};
#endif // MAINWINDOW_HH

