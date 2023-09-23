#include "mainwindow.hh"
#include "ui_mainwindow.h"
#include "gameboard.hh"
#include "gameboard.cpp"

#include <QMessageBox>

// Constructor for the main window
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , grid()
{
    ui->setupUi(this);

    // Initialize buttons and colors on the game board
    init_buttons();
    init_color();

    // Loop through each button on the game board
    for (unsigned int i = 0; i < buttons_.size(); i++) {
        for (unsigned int j = 0; j < buttons_.at(i).size(); j++) {
            if (check_nullptr(buttons_.at(i).at(j).button)) {
                continue;
            }

            // Connect button click event to a lambda function
            connect(buttons_.at(i).at(j).button, &QPushButton::clicked, this, [this,i,j]() {
                // Handle different button click scenarios based on color and position

                // If the same button is clicked again, reset its color and deselect it
                if(previous_x==j and previous_y==i){
                    init_color();
                    previous_x=-1;
                    previous_y=-1;
                }
                // If a colored button is clicked, check for movable positions
                else if(buttons_.at(i).at(j).color==RED or buttons_.at(i).at(j).color==GREEN){
                    check_movable(j,i);
                }
                // If a grey button is clicked, move to that position
                else{
                    move(j,i);
                }

                // Check if the game is over and handle the win condition
                if(grid.is_game_over()){
                    disable_all();
                    win_game();
                }
            });
        }
    }

    // Set up a timer to update the LCD display
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this,[this](){
        updateLCD();
    });
    timer->start(1000); // Update every 1000 ms (1 second)

    // Set initial LCD colors to light green
    ui->minLCD->setStyleSheet("background: light green");
    ui->secLCD->setStyleSheet("background: light green");

    // Connect UI buttons to corresponding functions
    connect(ui->resetButton,&QPushButton::clicked,this,&MainWindow::reset);
    connect(ui->pauseButton,&QPushButton::clicked,this,[this](){
        // Toggle between pausing and continuing the game
        if(stop){
            continue_game();
        }
        else{
            pause_game();
        }
    });
    connect(ui->undoButton,&QPushButton::clicked,this,[this](){
        // Undo the last move if possible
        if(counter!=0){
            undo();
        }
    });
}

// Destructor for the main window
MainWindow::~MainWindow()
{
    // Delete dynamically allocated buttons and UI
    for(unsigned int i=0;i<buttons_.size();i++){
        for(unsigned int j=0;j<buttons_.at(i).size();j++){
            delete buttons_.at(i).at(j).button;
        }
    }
    delete timer;
    delete blinkTimer;
    delete ui;
}

// Function to initialize the buttons on the game board
void MainWindow::init_buttons(){
    // Initialize the buttons using UI elements
    buttons_={
        {Button(ui->button1),Button(ui->button2),Button(ui->button3),Button(ui->button4)   },
        {Button(nullptr),    Button(ui->button5),Button(nullptr),    Button(nullptr)       },
        {Button(nullptr),    Button(ui->button6),Button(ui->button7),Button(nullptr)       },
        {Button(nullptr),    Button(ui->button8),Button(nullptr),    Button(nullptr)       },
        {Button(ui->button9),Button(ui->button10),Button(ui->button11),Button(ui->button12)}
    };
}

// Function to initialize colors on the game board
void MainWindow::init_color(){
    // Iterate through the grid and set button colors based on the game board
    for(unsigned int i=0;i<grid.get_board().size();i++){
        for(unsigned int j=0;j<grid.get_board().at(i).size();j++){

            if(check_nullptr(buttons_.at(i).at(j).button)){
                continue;
            }
            int color=grid.get_board().at(i).at(j);
            buttons_.at(i).at(j).color=color;

            // Set button colors and enable/disable them based on the color
            if(color==GREEN){
                buttons_.at(i).at(j).button->setStyleSheet("background-color: green");
                buttons_.at(i).at(j).button->setDisabled(false);

            }
            else if(color==RED){
                buttons_.at(i).at(j).button->setStyleSheet("background-color: red");
                buttons_.at(i).at(j).button->setDisabled(false);
            }
            else{
                buttons_.at(i).at(j).button->setStyleSheet("background-color: grey");
                buttons_.at(i).at(j).button->setDisabled(true);
            }
        }
    }
}

// Function to check movable positions for a given button
void MainWindow::check_movable(unsigned int x,unsigned int y){
    // Reset button colors
    init_color();

    // Store the starting position
    Point start;
    start.x=x;
    start.y=y;

    // Loop through positions to check for movable positions
    for(unsigned int i=0;i<5;i++){
        for(unsigned int j=0;j<4;j++){
            if(check_nullptr(buttons_.at(i).at(j).button) or (i==y and j==x)){
                continue;
            }

            // Make a copy of the current game state
            GameBoard org=grid;

            // Store the destination position
            Point destination;
            destination.x=j;
            destination.y=i;

            // Check if the move is valid and update button color
            if(grid.move(start,destination)){
                buttons_.at(i).at(j).button->setStyleSheet("background-color: yellow");
                buttons_.at(i).at(j).button->setEnabled(true);
                grid=org; // Restore the original game state
            }
        }
    }

    // Store the previous clicked position
    previous_x=x;
    previous_y=y;
}

// Function to execute a move on the game board
void MainWindow::move(unsigned int x, unsigned int y) {
    Point start;
    start.x=previous_x;
    start.y=previous_y;

    Point destination;
    destination.x=x;
    destination.y=y;

    // Store the move in the history
    std::vector<Point> points={start,destination};
    history.push_back(points);

    // Execute the move on the game board
    grid.move(start,destination);
    previous_x=-1;
    previous_y=-1;

    // Reset colors, increment counter, and update UI
    init_color();
    counter++;
    ui->labelCount->setText(QString::number(counter));
}

// Function to reset the game
void MainWindow::reset(){
    // Restore the original game state and update UI
    grid=org;
    init_color();
    updateLCD(true);
    ui->resetButton->setText("Reset");
    counter=0;
    ui->labelCount->setText(QString::number(counter));
}

// Function to disable all buttons on the game board
void MainWindow::disable_all(){
    // Disable all buttons and change their color to grey
    for(unsigned int i=0;i<buttons_.size();i++){
        for(unsigned int j=0;j<buttons_.at(i).size();j++){
            if(check_nullptr(buttons_.at(i).at(j).button)){
                continue;
            }

            buttons_.at(i).at(j).button->setStyleSheet("background-color: grey");
            buttons_.at(i).at(j).button->setDisabled(true);
        }
    }
}

// Function to check if a button is nullptr
bool MainWindow::check_nullptr(QPushButton* button){
    if(button==nullptr){
        return true;
    }
    return false;
}

// Function to update the LCD display
void MainWindow::updateLCD(bool reset)
{
    static int seconds = 0;
    static int minutes = 0;

    // Update the seconds and minutes values
    seconds = (seconds + 1) % 60;
    if (seconds == 0) {
        minutes = (minutes + 1) % 60;
    }

    // Reset the timer values if requested
    if(reset){
        seconds=0;
        minutes=0;
    }

    // Update the LCD numbers and set colors based on time
    ui->minLCD->display(minutes);
    ui->secLCD->display(seconds);

    if(minutes>=0 and minutes<2){
        ui->minLCD->setStyleSheet("background: light green");
        ui->secLCD->setStyleSheet("background: light green");
    }
    else if(minutes>=2 and minutes<=4){
        ui->minLCD->setStyleSheet("background: light yellow");
        ui->secLCD->setStyleSheet("background: light yellow");
    }
    else{
        ui->minLCD->setStyleSheet("background: light red");
        ui->secLCD->setStyleSheet("background: light red");
    }
}

// Function to pause the game
void MainWindow::pause_game(){
    // Disable buttons and stop the timer
    disable_all();
    timer->stop();

    // Change LCD colors to grey and update button text
    ui->minLCD->setStyleSheet("background-color: grey");
    ui->secLCD->setStyleSheet("background-color: grey");
    ui->pauseButton->setText("Continue");
    stop=true;
}

// Function to continue the game after pausing
void MainWindow::continue_game(){
    // Start the timer, restore button colors, and update UI
    timer->start(1000);
    init_color();
    stop=false;
    
    // Change button text
    ui->pauseButton->setText("Pause");
    updateLCD();
}

// Function to undo the last move
void MainWindow::undo(){
    // Restore the original game state
    grid=org;

    // Remove the last move from history and update counter
    history.pop_back();
    counter--;
    ui->labelCount->setText(QString::number(counter));

    // Reapply moves from history to the game board and update colors
    for(unsigned int i=0;i<history.size();i++){
        grid.move(history.at(i).at(0),history.at(i).at(1));
    }
    init_color();
}

// Function to toggle the blinking background color
void MainWindow::toggleBlinkBackgroundColor() {
    if (blinkingBackground) {
        this->setStyleSheet("background-color: light blue");
    } else {
        this->setStyleSheet("background-color: yellow");
    }
    blinkingBackground = !blinkingBackground;
}

// Function to handle winning the game
void MainWindow::win_game() {
    // Stop the timer
    timer->stop();

    // Handle win conditions based on the number of moves
    if (counter<=31) {
        ui->resetButton->setEnabled(true); // Enable the replay button

        // Set up a timer to toggle background color
        blinkTimer = new QTimer(this);
        connect(blinkTimer, &QTimer::timeout, this, &MainWindow::toggleBlinkBackgroundColor);
        blinkTimer->start(500); // Blink every 500 ms

        // Show a congratulations message
        QMessageBox::information(this, "Congratulations!", "You've won the game within 31 moves!");
    } else {
        this->setStyleSheet("background-color: light grey");
        ui->resetButton->setEnabled(true); // Enable the replay button

        // Show a congratulations message
        QMessageBox::information(this, "Congratulations!", "You've won the game!");
    }
}
