#include <SFML/Graphics.hpp>
#include <iostream>
using namespace std;

// Constants for board size and piece size
const int SIZE = 8;
const float SQUARE_SIZE = 50.f; // Size of each square

// Piece structure to store piece type and whether it's selected
struct Piece 
{
    char type; // 'o' for Player 1, 'x' for Player 2, ' ' for empty
    bool isSelected; // Whether the piece is selected
};

// Function to initialize the board
void initializeBoard(Piece board[SIZE][SIZE]) 
{
    for (int i = 0; i < SIZE; i++)   //rows
	 {
        for (int j = 0; j < SIZE; j++)  //coloumns
		{
            board[i][j].type = ' '; // Start with all empty
        }
    }

    // Initialize Player 1's pieces ('o')
    for (int i = 0; i < 3; i++) 
	{
        for (int j = 0; j < SIZE; j++)
		 {
            if ((i + j) % 2 == 1) 
			{    // Alternate squares
                board[i][j].type = 'o';
            }
        }
    }

    // Initialize Player 2's pieces ('x')
    for (int i = 5; i < SIZE; i++) 
	{
        for (int j = 0; j < SIZE; j++) 
		{
            if ((i + j) % 2 == 1)
			 { // Alternate squares
                board[i][j].type = 'x';
            }
        }
    }
}

// Function to check if the move is valid
bool isValidMove(Piece board[SIZE][SIZE], int x1, int y1, int x2, int y2, bool player1Turn) 
{
    if (x2 < 0 || x2 >= SIZE || y2 < 0 || y2 >= SIZE) return false; // Out of bounds
    if (board[x2][y2].type != ' ') return false; // Target square must be empty

    int dx = x2 - x1;
    int dy = abs(y2 - y1);

    // Player 1 can only move downward, Player 2 can only move upward
    if (player1Turn && dx != 1) return false;
    if (!player1Turn && dx != -1) return false;

    return dy == 1; // Normal move (diagonal by 1)
}

// Function to check for valid capture move (jump over an opponent's piece)
bool isCaptureMove(Piece board[SIZE][SIZE], int x1, int y1, int x2, int y2, bool player1Turn) 
{
    if (x2 < 0 || x2 >= SIZE || y2 < 0 || y2 >= SIZE) return false; // Out of bounds
    if (board[x2][y2].type != ' ') return false; // Target square must be empty

    int dx = x2 - x1;
    int dy = abs(y2 - y1);

    char opponent = (player1Turn) ? 'x' : 'o'; // Determine opponent

    if ((player1Turn && dx != 2) || (!player1Turn && dx != -2)) return false; // Forward jump only

    if (dx == 2 || dx == -2) {
        int midX = (x1 + x2) / 2;
        int midY = (y1 + y2) / 2;
        return board[midX][midY].type == opponent; // Must be opponent's piece in between
    }

    return false;
}

// Function to capture an opponent's piece
void capturePiece(Piece board[SIZE][SIZE], int x1, int y1, int x2, int y2)
 {
    int midX = (x1 + x2) / 2;
    int midY = (y1 + y2) / 2;
    board[midX][midY].type = ' '; // Remove opponent's piece
    board[x2][y2].type = board[x1][y1].type; // Move player's piece to new position
    board[x1][y1].type = ' '; // Empty the old position
}

// Function to count pieces of a given type
int countPieces(Piece board[SIZE][SIZE], char type) 
 {
    int count = 0;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (board[i][j].type == type) {
                count++;
            }
        }
    }
    return count;
 }

// Function to handle mouse input and piece movement
void handleMouseClick(Piece board[SIZE][SIZE], sf::RenderWindow &window, bool &player1Turn, int &selectedX, int &selectedY)
 {
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    int x = mousePos.y / SQUARE_SIZE; // Row (y-coordinate)
    int y = mousePos.x / SQUARE_SIZE; // Column (x-coordinate)

    if (x >= 0 && x < SIZE && y >= 0 && y < SIZE) 
	{
        char currentPlayer = player1Turn ? 'o' : 'x';

        // If a piece is already selected
        if (selectedX != -1 && selectedY != -1) 
		{
            bool validMove = false;

            // Check for capture move
            if (isCaptureMove(board, selectedX, selectedY, x, y, player1Turn))
			 {
                capturePiece(board, selectedX, selectedY, x, y);
                validMove = true;
            } 
            // Check for normal move
            else if (isValidMove(board, selectedX, selectedY, x, y, player1Turn))
			 {
                board[x][y].type = board[selectedX][selectedY].type;
                board[selectedX][selectedY].type = ' ';
                validMove = true;
            }

            // If move is valid, switch turns and deselect the piece
            if (validMove)
			 {
                player1Turn = !player1Turn;
            }

            selectedX = -1;
            selectedY = -1;
        } 
        // If no piece is selected, select a piece of the current player
        else if (board[x][y].type == currentPlayer) {
            selectedX = x;
            selectedY = y;
        }
    }
}

// Function to draw the board and pieces
void displayBoardSFML(Piece board[SIZE][SIZE], sf::RenderWindow &window, int selectedX, int selectedY, bool player1Turn) 
{
    window.clear();

    sf::Color lightSquareColor(240, 217, 181);
    sf::Color darkSquareColor(181, 136, 99);
    sf::Color player1Color(255, 0, 0); // Red for Player 1
    sf::Color player2Color(0, 0, 255); // Blue for Player 2
    sf::Color selectionColor(0, 255, 0, 128); // Green with transparency for selection
    sf::Color moveHighlightColor(255, 255, 0, 128); // Yellow for available moves

    // Draw the squares and pieces
    for (int i = 0; i < SIZE; i++)
	 {
        for (int j = 0; j < SIZE; j++)
		 {
            // Draw squares
            sf::RectangleShape square(sf::Vector2f(SQUARE_SIZE, SQUARE_SIZE));
            square.setPosition(j * SQUARE_SIZE, i * SQUARE_SIZE);
            if ((i + j) % 2 == 0)
			 {
                square.setFillColor(lightSquareColor);
            } else
			 {
                square.setFillColor(darkSquareColor);
            }
            window.draw(square);

            // Highlight selected piece
            if (i == selectedX && j == selectedY) 
			{
                sf::RectangleShape highlight(sf::Vector2f(SQUARE_SIZE, SQUARE_SIZE));
                highlight.setPosition(j * SQUARE_SIZE, i * SQUARE_SIZE);
                highlight.setFillColor(selectionColor);
                window.draw(highlight);

                // Highlight valid moves
                for (int dx = -1; dx <= 1; dx += 2) { // Check for normal moves
                    for (int dy = -1; dy <= 1; dy += 2) {
                        int newX = selectedX + dx;
                        int newY = selectedY + dy;

                        // Check for valid normal moves
                        if (isValidMove(board, selectedX, selectedY, newX, newY, player1Turn)) {
                            sf::RectangleShape moveHighlight(sf::Vector2f(SQUARE_SIZE, SQUARE_SIZE));
                            moveHighlight.setPosition(newY * SQUARE_SIZE, newX * SQUARE_SIZE);
                            moveHighlight.setFillColor(moveHighlightColor);
                            window.draw(moveHighlight);
                        }

                        // Check for capture moves
                        newX = selectedX + dx * 2;
                        newY = selectedY + dy * 2;
                        if (isCaptureMove(board, selectedX, selectedY, newX, newY, player1Turn)) {
                            sf::RectangleShape moveHighlight(sf::Vector2f(SQUARE_SIZE, SQUARE_SIZE));
                            moveHighlight.setPosition(newY * SQUARE_SIZE, newX * SQUARE_SIZE);
                            moveHighlight.setFillColor(moveHighlightColor);
                            window.draw(moveHighlight);
                        }
                    }
                }
            }

            // Draw pieces
            if (board[i][j].type != ' ') {
                sf::CircleShape piece(SQUARE_SIZE / 3);
                piece.setPosition(j * SQUARE_SIZE + SQUARE_SIZE / 6, i * SQUARE_SIZE + SQUARE_SIZE / 6);
                if (board[i][j].type == 'o') {
                    piece.setFillColor(player1Color); // Player 1
                } else if (board[i][j].type == 'x') {
                    piece.setFillColor(player2Color); // Player 2
                }
                window.draw(piece);
            }
        }
    }

    window.display();
}

// Function to display a win message and restart option
bool displayWinMessage(sf::RenderWindow &window, const string &winner)
 {
    sf::Font font;
    if (!font.loadFromFile("C:\\Windows\\Fonts\\arial.ttf")) { // Updated font path
        cerr << "Error loading font!" << endl;
        return false;
    }

    sf::Text winText;
    winText.setFont(font);
    winText.setString(winner + " Wins!\nClick to Play Again");
    winText.setCharacterSize(40);
    winText.setFillColor(sf::Color::Black);
    winText.setPosition(50, 150);

    window.clear(sf::Color::White);
    window.draw(winText);
    window.display();

    // Wait for mouse click to restart
    while (true)
	 {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
			 {
                return true; // Restart the game
            }
        }
    }
}

// Main game loop
void playGame() 
{
    Piece board[SIZE][SIZE];
    initializeBoard(board);

    bool player1Turn = true;
    int selectedX = -1, selectedY = -1; // No piece selected initially

    sf::RenderWindow window(sf::VideoMode(SIZE * SQUARE_SIZE, SIZE * SQUARE_SIZE), "Checkers Game");

    while (window.isOpen()) 
	{
        sf::Event event;
        while (window.pollEvent(event)) 
		{
            if (event.type == sf::Event::Closed)
			 {
                window.close();
            }
            // Handle mouse click
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
			 {
                handleMouseClick(board, window, player1Turn, selectedX, selectedY);
            }
        }

        // Check win condition
        if (countPieces(board, 'o') == 0) 
		{
            if (displayWinMessage(window, "Player 2"))
			 {
                initializeBoard(board);
                player1Turn = true;
                selectedX = -1;
                selectedY = -1;
            }
        } else if (countPieces(board, 'x') == 0) 
		{
            if (displayWinMessage(window, "Player 1"))
			 {
                initializeBoard(board);
                player1Turn = true;
                selectedX = -1;
                selectedY = -1;
            }
        }

        // Display the board and pieces
        displayBoardSFML(board, window, selectedX, selectedY, player1Turn);
    }
}

// Main function
int main() 
{
    playGame();
    return 0;
}

