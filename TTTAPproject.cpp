// ============================================================
//  TIC TAC TOE - Console Based Mini Game in C++
//  Concepts Used: Arrays, Loops, Conditionals, Functions
//
//  PLATFORM NOTE:
//  This file works on both Windows and Linux/Mac.
//  It auto-detects the OS at compile time to:
//    - Clear the screen correctly (cls vs clear)
//    - Read a single keypress without needing Enter
// ============================================================

#include <iostream>
#include <string>
#include <limits>

// ============================================================
//  PLATFORM DETECTION
//  _WIN32 is automatically defined by Windows compilers.
//  On Windows : use conio.h for _getch() (instant key read)
//  On Linux/Mac: use termios.h to set raw terminal mode
// ============================================================

#ifdef _WIN32
    #include <conio.h>      // Provides _getch() on Windows
#else
    #include <termios.h>    // Linux/Mac terminal settings
    #include <unistd.h>     // POSIX read() and STDIN_FILENO
#endif

using namespace std;


// ============================================================
//  GLOBAL VARIABLES
//  The board is a 1D array of 9 chars representing a 3x3 grid.
//  Positions are numbered 1-9 for easy player input.
// ============================================================

char board[9];          // Holds ' ', 'X', or 'O' for each cell
string player1Name;     // Name of the X player
string player2Name;     // Name of the O player
string statusMsg;       // Message shown below board each frame


// ============================================================
//  FUNCTION: clearScreen
//  Clears the entire terminal so the next renderFrame() call
//  paints the board at the top — creating a "frame refresh"
//  effect where the board updates in-place, not by scrolling.
// ============================================================

void clearScreen() {
#ifdef _WIN32
    system("cls");          // Windows clear command
#else
    system("clear");        // Linux / Mac clear command
#endif
}


// ============================================================
//  FUNCTION: getChar
//  Reads ONE character from the keyboard instantly,
//  WITHOUT waiting for the player to press Enter.
//
//  Windows : _getch() from <conio.h> handles this natively.
//  Linux/Mac: we temporarily switch the terminal to "raw"
//  mode (no line buffering, no echo), read one byte, then
//  restore the original settings before returning.
// ============================================================

char getChar() {
#ifdef _WIN32
    return (char)_getch();      // Instant keypress on Windows
#else
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);          // Save current terminal settings
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);        // Disable line-buffer + echo
    tcsetattr(STDIN_FILENO, TCSANOW, &newt); // Apply raw mode immediately

    char ch = (char)getchar();               // Read exactly one character

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt); // Restore normal terminal mode
    return ch;
#endif
}


// ============================================================
//  FUNCTION: initBoard
//  Resets all 9 cells to a space so the board is empty.
//  Also clears the status message for a clean new frame.
//  Called at the start of every game and rematch.
// ============================================================

void initBoard() {
    for (int i = 0; i < 9; i++) {
        board[i] = ' ';
    }
    statusMsg = "";
}


// ============================================================
//  FUNCTION: renderFrame
//  THE CORE DISPLAY FUNCTION.
//  Clears the screen and redraws the entire UI from scratch:
//    1. Title banner
//    2. Player legend (who is X, who is O)
//    3. The 3x3 board with dividers
//    4. A status message line that changes each turn
//
//  Empty cells show their number (1-9) as a position guide.
//  Filled cells show the player's mark (X or O).
//
//  Because clearScreen() runs first, this always draws at
//  the top of the terminal — same position every frame,
//  giving the illusion of an in-place animated update.
// ============================================================

void renderFrame() {
    clearScreen();  // Wipe terminal — start painting fresh frame

    // --- Title banner ---
    cout << "\n";
    cout << "  ======================================\n";
    cout << "   #  TIC  TAC  TOE  -  Console  Game  #\n";
    cout << "  ======================================\n";

    // --- Player legend ---
    cout << "\n";
    cout << "  Player X : " << player1Name << "\n";
    cout << "  Player O : " << player2Name << "\n";
    cout << "\n";

    // --- 3x3 board: empty cells show position number ---
    cout << "     |     |     \n";
    cout << "  " << (board[0]==' '?'1':board[0])
         << "  |  " << (board[1]==' '?'2':board[1])
         << "  |  " << (board[2]==' '?'3':board[2]) << "  \n";
    cout << "     |     |     \n";
    cout << "-----+-----+-----\n";
    cout << "     |     |     \n";
    cout << "  " << (board[3]==' '?'4':board[3])
         << "  |  " << (board[4]==' '?'5':board[4])
         << "  |  " << (board[5]==' '?'6':board[5]) << "  \n";
    cout << "     |     |     \n";
    cout << "-----+-----+-----\n";
    cout << "     |     |     \n";
    cout << "  " << (board[6]==' '?'7':board[6])
         << "  |  " << (board[7]==' '?'8':board[7])
         << "  |  " << (board[8]==' '?'9':board[8]) << "  \n";
    cout << "     |     |     \n";

    // --- Status message (updates every turn) ---
    cout << "\n  " << statusMsg << "\n";
}


// ============================================================
//  FUNCTION: checkWin
//  Checks all 8 possible winning lines (3 rows, 3 columns,
//  2 diagonals). Returns true if the given mark has won.
// ============================================================

bool checkWin(char mark) {
    // Every possible winning combination by board index
    int wins[8][3] = {
        {0,1,2}, {3,4,5}, {6,7,8},  // Rows
        {0,3,6}, {1,4,7}, {2,5,8},  // Columns
        {0,4,8}, {2,4,6}            // Diagonals
    };

    for (int i = 0; i < 8; i++) {
        if (board[wins[i][0]] == mark &&
            board[wins[i][1]] == mark &&
            board[wins[i][2]] == mark) {
            return true;    // This line is a winner
        }
    }
    return false;           // No winning line found
}


// ============================================================
//  FUNCTION: checkTie
//  Returns true if every cell is filled with no winner yet.
// ============================================================

bool checkTie() {
    for (int i = 0; i < 9; i++) {
        if (board[i] == ' ') return false;  // Still empty cells
    }
    return true;    // Board full = tie
}


// ============================================================
//  FUNCTION: getPlayerMove
//  Handles one player's turn using INSTANT key input.
//  - No Enter key required: the move is placed the moment
//    the player presses a digit key 1-9.
//  - If the key is out of range or the cell is taken,
//    the status message updates and the frame redraws,
//    prompting the player to try again.
// ============================================================

void getPlayerMove(const string& playerName, char mark) {
    while (true) {
        // Show whose turn it is in the status line
        statusMsg = playerName + " [" + mark + "] -> Press 1-9 to place:";
        renderFrame();      // Redraw full frame with updated prompt

        char key = getChar();   // Instant keypress — no Enter needed

        // Only accept digit characters '1' through '9'
        if (key < '1' || key > '9') {
            statusMsg = "[!] Invalid key. Press a number 1-9.";
            continue;
        }

        int idx = key - '1';    // Convert '1'-'9' to array index 0-8

        // Reject if this cell is already taken
        if (board[idx] != ' ') {
            statusMsg = string("[!] Cell ") + key + " is taken! Try another.";
            continue;
        }

        // Valid move: place the mark on the board and return
        board[idx] = mark;
        break;
    }
}


// ============================================================
//  FUNCTION: getPlayerNames
//  Draws a clean screen with the title and how-to-play info,
//  then collects both player names using normal text input.
//  Called only at the very start or when "New Game" is chosen.
// ============================================================

void getPlayerNames() {
    clearScreen();
    cout << "\n";
    cout << "  ======================================\n";
    cout << "   #  TIC  TAC  TOE  -  Console  Game  #\n";
    cout << "  ======================================\n";
    cout << "\n";
    cout << "  HOW TO PLAY:\n";
    cout << "  - Cells are numbered 1-9 (shown on the board).\n";
    cout << "  - Press a number key to place your mark.\n";
    cout << "  - No Enter needed during gameplay!\n";
    cout << "  - First to line up 3 in a row wins.\n";
    cout << "\n";

    // Flush any leftover newline before getline
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    cout << "  Enter name for Player X: ";
    getline(cin, player1Name);

    cout << "  Enter name for Player O: ";
    getline(cin, player2Name);

    cout << "\n  Welcome, " << player1Name
         << " and " << player2Name << "!\n";
    cout << "  Press any key to begin...\n";
    getChar();  // Hold screen until player is ready
}


// ============================================================
//  FUNCTION: playGame
//  Main game loop. Alternates turns, renders a fresh frame
//  after each move, and checks for win or tie each turn.
//  Returns: 'X' if X wins, 'O' if O wins, 'T' for tie.
// ============================================================

char playGame() {
    initBoard();    // Wipe the board and reset the status message

    // Show the empty board with a "game starting" message
    statusMsg = "Game on!  " + player1Name + " (X) goes first.";
    renderFrame();

    // Loop for up to 9 turns (all cells filled = max turns)
    for (int turn = 0; turn < 9; turn++) {

        if (turn % 2 == 0) {
            getPlayerMove(player1Name, 'X');    // X's turn
        } else {
            getPlayerMove(player2Name,  'O');   // O's turn
        }

        // After placing, check if the current player won
        if (turn % 2 == 0 && checkWin('X')) {
            statusMsg = "*** " + player1Name + " (X) WINS! ***";
            renderFrame();      // Show the winning board
            return 'X';
        }
        if (turn % 2 == 1 && checkWin('O')) {
            statusMsg = "*** " + player2Name + " (O) WINS! ***";
            renderFrame();
            return 'O';
        }

        // Check for a completely filled board with no winner
        if (checkTie()) {
            statusMsg = "*** IT'S A TIE! Great game by both! ***";
            renderFrame();
            return 'T';
        }
    }

    return 'T';     // Fallback safety return
}


// ============================================================
//  FUNCTION: printResult
//  Prints a formatted result summary below the final frame,
//  clearly stating winner, loser, or a tie with player names.
// ============================================================

void printResult(char result) {
    cout << "\n  --------------------------------------\n";
    if (result == 'X') {
        cout << "  WINNER : " << player1Name << " (X)\n";
        cout << "  LOSER  : " << player2Name << " (O) - better luck next time!\n";
    } else if (result == 'O') {
        cout << "  WINNER : " << player2Name << " (O)\n";
        cout << "  LOSER  : " << player1Name << " (X) - better luck next time!\n";
    } else {
        cout << "  RESULT : It's a TIE!\n";
        cout << "  Both " << player1Name << " and "
             << player2Name << " played evenly.\n";
    }
    cout << "  --------------------------------------\n\n";
}


// ============================================================
//  FUNCTION: showPostGameMenu
//  Displays three choices after a game ends.
//  Input is instant (no Enter) — just press 1, 2, or 3.
//    1 = Rematch  : same names, fresh board
//    2 = New Game : enter new player names
//    3 = Exit     : quit the program
//  Returns the character '1', '2', or '3'.
// ============================================================

char showPostGameMenu() {
    cout << "  What would you like to do?\n\n";
    cout << "  [1]  Rematch      (same players, new board)\n";
    cout << "  [2]  New Game     (enter new player names)\n";
    cout << "  [3]  Exit\n\n";
    cout << "  Press 1, 2, or 3: ";

    while (true) {
        char key = getChar();   // No Enter needed
        if (key == '1' || key == '2' || key == '3') {
            cout << key << "\n";    // Echo choice for visual feedback
            return key;
        }
        // Any other key is silently ignored — loop continues
    }
}


// ============================================================
//  FUNCTION: main
//  Entry point. Runs the full game lifecycle:
//  name entry  ->  game loop  ->  result  ->  menu  ->  repeat
// ============================================================

int main() {

    // First run always starts with name entry
    getPlayerNames();

    // --------------------------------------------------------
    //  OUTER LOOP: Keeps the program alive across rematches
    //  and new games until the player selects Exit.
    // --------------------------------------------------------
    while (true) {

        // Play one complete game and capture the outcome
        char result = playGame();

        // Print winner/loser/tie summary under the final frame
        printResult(result);

        // Show the post-game menu and read the player's choice
        char choice = showPostGameMenu();

        if (choice == '1') {
            // REMATCH: names stay the same, board resets next loop
            // statusMsg carries a brief note into the first frame
            statusMsg = "Rematch!  " + player1Name + " (X) goes first.";

        } else if (choice == '2') {
            // NEW GAME: clear everything, collect fresh names
            getPlayerNames();

        } else {
            // EXIT: say goodbye and break the loop
            clearScreen();
            cout << "\n  Thanks for playing, "
                 << player1Name << " and " << player2Name << "!\n";
            cout << "  See you next time. Goodbye!\n\n";
            break;
        }
    }

    return 0;   // Program ends cleanly
}
