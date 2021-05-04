#include <iostream>
#include <windows.h>
#include <conio.h>
#include <ctime>
using std::cout;
using std::endl;
using std::cin;

// README ! ! ! : Program behaves correctly in Eng keyboard layout only :::::::::::::::::::::::::::::::::::::
// ==========================================================================================================
// Value :::::::: Interpretation ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
// ==========================================================================================================
// 0 :::::::::::: This cell is empty (no mines around) and it was not checked yet :::::::::::::::::::::::::::
// 10 ::::::::::: Checked cell with no mines around :::::::::::::::::::::::::::::::::::::::::::::::::::::::::
// -11 to - 88 :: Unrevealed cell with mine :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
// -1 to -8 ::::: Unrevealed cell with mine(s) around :::::::::::::::::::::::::::::::::::::::::::::::::::::::
// 1 to 8 ::::::: Checked cell with mine(s) around. Value == q-ty of mines around the cell ::::::::::::::::::
// value * 1000 : Player has marked this cell as potential mined one. No actions avaiable till mark withdrawn
// 9999 ::::::::: Player has marked this cell as potential mined one. No actions avaiable till mark withdrawn

struct Attributes
{
	int** matrix;
	int rows;
	int columns;
	int minesQuantity;
	int cursor_Y;
	int cursor_X;
	int cellsLeftToOpen;
};

void InitializeProperties(Attributes*);
void MainMenu(Attributes*);
void SettingsMenu(Attributes*);
void UserSettings(Attributes*);
void PlantMines(Attributes*);
void InilializeCellsAroundMines(Attributes*);
void WholeGameFieldOutput(int** const, const int, const int, const int, const int, bool = false);
void MoveAndReaction(Attributes*, COORD*);
void CursorPositionOutput(const int, COORD, bool);
void ClearNearbySafeCells(Attributes*, const int, const int, COORD);
void SettingsReset(Attributes*, COORD*);
void RulesOutput(Attributes* const, bool);
inline void CaseWinningGameOuput();
inline void ShowGameTime(unsigned int);
inline void CaseLosingGameOutput();

int main()
{
	Attributes game;
	COORD cursor = { 18, 4 }; // Console cursor coordinates

	InitializeProperties(&game);
	MainMenu(&game);

	while (true)
	{
		PlantMines(&game);
		InilializeCellsAroundMines(&game);

		game.cellsLeftToOpen = game.rows * game.columns - game.minesQuantity;

		unsigned int gameStartTime = clock(); // Begin counting time of game session

		WholeGameFieldOutput(game.matrix, game.rows, game.columns, game.cursor_Y, game.cursor_X);

		while (game.cellsLeftToOpen > 0) // The very damn magic of the game process is performed here
		{
			MoveAndReaction(&game, &cursor);
		}

		if (game.cellsLeftToOpen == 0) // Case: game is won
		{
			WholeGameFieldOutput(game.matrix, game.rows, game.columns, game.cursor_Y, game.cursor_X);
			CaseWinningGameOuput();
			ShowGameTime(gameStartTime);
		}

		else if (game.cellsLeftToOpen == -1) // Case: game is lost
		{
			WholeGameFieldOutput(game.matrix, game.rows, game.columns, game.cursor_Y, game.cursor_X, true);
			CaseLosingGameOutput();
		}

		SettingsReset(&game, &cursor);
		MainMenu(&game);
	}
	return 0;
}

void InitializeProperties(Attributes* pGame)
{
	pGame->matrix = new int* [10];

	for (int i = 0; i < 10; i++)
		pGame->matrix[i] = new int[10]{};

	pGame->rows = 10;
	pGame->columns = 10;
	pGame->minesQuantity = 10;
	pGame->cursor_Y = 0;
	pGame->cursor_X = 0;
}

void MainMenu(Attributes* pGame)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	int menuChoice(-1);

	do
	{
		menuChoice = -1;
		system("cls");
		SetConsoleTextAttribute(hConsole, 0x0A);

		cout << " _    _   _   _  _   ___   ___  __      __  ___   ___   ___   ___   ___" << endl;
		cout << "| \\  / | | | | \\| | | __| / __| \\ \\    / / | __| | __| | _ \\ | __| | _ \\" << endl;
		cout << "| |\\/| | | | | .` | | _|  \\ _ \\  \\ \\/\\/ /  | _|  | _|  |  _/ | _|  |   /" << endl;
		cout << "|_|  |_| |_| |_|\\_| |___| |___/   \\_/\\_/   |___| |___| |_|   |___| |_|_\\" << endl;

		cout << "\t\t" << char(201);
		for (int i = 27; i > 0; i--)
		{
			cout << char(205);
		}
		cout << char(187) << endl;

		cout << "\t\t" << char(186) << " Start new game ---- key 1 " << char(186) << endl;
		cout << "\t\t" << char(186) << " Difficulty setup -- key 2 " << char(186) << endl;
		cout << "\t\t" << char(186) << " Rules & controls -- key 3 " << char(186) << endl;
		cout << "\t\t" << char(186) << " Leave the game ---- key 0 " << char(186) << endl;

		cout << "\t\t" << char(200);
		for (int i = 27; i > 0; i--)
		{
			cout << char(205);
		}			cout << char(188) << endl;

		SetConsoleTextAttribute(hConsole, 0x0F);

		while (menuChoice < 48 || menuChoice >51)
		{
			menuChoice = _getch();
		}

		switch (menuChoice)
		{
		case 49: // Start the game
			break;

		case 50: // Change difficulty
			SettingsMenu(pGame);
			menuChoice = -1; // So user still stays in the menu of this function after changing difficulty settings
			break;

		case 51: // Check for controls, rules and game field legend
			RulesOutput(pGame, true);
			menuChoice = -1; // So user still stays in the menu of this function after changing difficulty settings
			break;

		case 48: // Close the program
			exit(0);
		}
	} while (menuChoice != 48 && menuChoice != 49);
}

void SettingsMenu(Attributes* pGame)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	int difficultyLevelChoice(0);

	do
	{
		system("cls");
		SetConsoleTextAttribute(hConsole, 0x0A);
		cout << "\n\n\t\t      Difficulty setting\t" << endl;
		cout << "\t\t" << char(201);

		for (int i = 28; i > 0; i--)
		{
			cout << char(205);
		}
		cout << char(187) << endl;

		cout << "\t\t" << char(186) << "     Easy --- press key 1   " << char(186) << endl;
		cout << "\t\t" << char(186) << "     Medium - press key 2   " << char(186) << endl;
		cout << "\t\t" << char(186) << "     Hard --- press key 3   " << char(186) << endl;
		cout << "\t\t" << char(186) << "     Custom - press key 4   " << char(186) << endl;
		cout << "\t\t" << char(204);

		for (int i = 28; i > 0; i--)
		{
			cout << char(205);
		}
		cout << char(185) << endl;

		cout << "\t\t" << char(186) << "Return to main menu -- key 0" << char(186) << endl;
		cout << "\t\t" << char(200);

		for (int i = 28; i > 0; i--)
		{
			cout << char(205);
		}
		cout << char(188) << endl;

		difficultyLevelChoice = _getch();
		SetConsoleTextAttribute(hConsole, 0x0F);
	} while (difficultyLevelChoice < 48 || difficultyLevelChoice > 52);

	switch (difficultyLevelChoice) // Reassembling matrix and properties according to player's choice
	{
	case 49: // Easy mode
		for (int i = 0; i < pGame->rows; i++) // Clear memory
			delete[] pGame->matrix[i];

		delete[] pGame->matrix;

		pGame->rows = 10; // Setting horizontal size
		pGame->columns = 10; // Setting vertical size
		pGame->minesQuantity = 10; // Setting mines quantity

		pGame->matrix = new int* [pGame->rows]; // New matrix for game field

		for (int i = 0; i < pGame->rows; i++)
			pGame->matrix[i] = new int[pGame->columns]{};
		break;

	case 50: // Medium mode
		for (int i = 0; i < pGame->rows; i++) // Clear memory
			delete[] pGame->matrix[i];

		delete[] pGame->matrix;

		pGame->rows = 16; // Setting horizontal size
		pGame->columns = 16; // Setting vertical size
		pGame->minesQuantity = 40; // Setting mines quantity

		pGame->matrix = new int* [pGame->rows]; // New matrix for game field

		for (int i = 0; i < pGame->rows; i++)
			pGame->matrix[i] = new int[pGame->columns]{};
		break;

	case 51: // Hard mode
		for (int i = 0; i < pGame->rows; i++) // Clear memory
			delete[] pGame->matrix[i];

		delete[] pGame->matrix;

		pGame->rows = 16; // Setting horizontal size
		pGame->columns = 30; // Setting vertical size
		pGame->minesQuantity = 99; // Setting mines quantity

		pGame->matrix = new int* [pGame->rows]; // New matrix for game field

		for (int i = 0; i < pGame->rows; i++)
			pGame->matrix[i] = new int[pGame->columns]{};
		break;

	case 52: // User's settings
		UserSettings(pGame);
		break;

	case 48: // No changes, just leave the menu
		break;
	}
}

void UserSettings(Attributes* pGame)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	system("cls");

	int userGameFieldHeight(0);

	while (userGameFieldHeight < 5 || userGameFieldHeight > 50)
	{
		system("cls");
		SetConsoleTextAttribute(hConsole, 0x0A);
		cout << "\n\n\n\t\t" << char(201);

		for (int i = 27; i > 0; i--)
		{
			cout << char(205);
		}

		cout << char(187) << endl;
		cout << "\t\t" << char(186) << " Customizing mine field... " << char(186) << endl;
		cout << "\t\t" << char(200);

		for (int i = 27; i > 0; i--)
		{
			cout << char(205);
		}
		cout << char(188) << endl;
		cout << "\t\t Field height (10 to 50): ";
		cin >> userGameFieldHeight;
	}

	int userGameFieldWidth(0);

	while (userGameFieldWidth < 5 || userGameFieldWidth > 50)
	{
		system("cls");
		SetConsoleTextAttribute(hConsole, 0x0A);
		cout << "\n\n\n\t\t" << char(201);

		for (int i = 27; i > 0; i--)
		{
			cout << char(205);
		}
		cout << char(187) << endl;

		cout << "\t\t" << char(186) << " Customizing mine field... " << char(186) << endl;
		cout << "\t\t" << char(200);

		for (int i = 27; i > 0; i--)
		{
			cout << char(205);
		}

		cout << char(188) << endl;

		cout << "\t\t Field width (10 to 50): ";
		cin >> userGameFieldWidth;
	}

	int userMinesQuantity(0);

	while (userMinesQuantity < userGameFieldHeight * userGameFieldWidth / 10 || userMinesQuantity > userGameFieldHeight * userGameFieldWidth / 3)
	{
		system("cls");
		SetConsoleTextAttribute(hConsole, 0x0A);
		cout << "\n\n\n\t\t" << char(201);

		for (int i = 27; i > 0; i--)
		{
			cout << char(205);
		}
		cout << char(187) << endl;

		cout << "\t\t" << char(186) << " Customizing mine field... " << char(186) << endl;
		cout << "\t\t" << char(200);

		for (int i = 27; i > 0; i--)
		{
			cout << char(205);
		}			cout << char(188) << endl;

		cout << "\t\t Mines q-ty (" << userGameFieldHeight * userGameFieldWidth / 10 << " to " << userGameFieldHeight * userGameFieldWidth / 3 << "): ";
		cin >> userMinesQuantity;
	}

	for (int i = 0; i < pGame->rows; i++) // Clear memory
		delete[] pGame->matrix[i];

	delete[] pGame->matrix;

	pGame->rows = userGameFieldHeight;
	pGame->columns = userGameFieldWidth;
	pGame->minesQuantity = userMinesQuantity;

	pGame->matrix = new int* [pGame->rows]; // New matrix for game field

	for (int i = 0; i < pGame->rows; i++)
		pGame->matrix[i] = new int[pGame->columns]{};
}

void PlantMines(Attributes* pGame)
{
	int minesSet(0); // Quantity of already set mines

	int coordinateX, coordinateY; // Coordinates of potential cell to plant a mine at

	srand(time(NULL)); // Activates pseudo-random value generator

	while (minesSet < pGame->minesQuantity)
	{
		coordinateY = rand() % pGame->rows; // Choose y-coordinate with value from 0 to ROWS - 1
		coordinateX = rand() % pGame->columns; // Choose x-coordinate with value from 0 to COLUMNS - 1

		if (pGame->matrix[coordinateY][coordinateX] == 0)
		{
			pGame->matrix[coordinateY][coordinateX] = -11;

			for (int i = -1; i <= 1; i++)
			{
				for (int j = -1; j <= 1; j++)
				{
					if (coordinateY + i >= 0 && coordinateY + i < pGame->rows && coordinateX + j >= 0 && coordinateX + j < pGame->columns)
					{
						if (pGame->matrix[coordinateY + i][coordinateX + j] != 0 && pGame->matrix[coordinateY + i][coordinateX + j] % 11 == 0)
						{
							pGame->matrix[coordinateY][coordinateX] -= 11; // Update value of neighbor cell with mine, so it too bears info about how many mines are around that cell
							pGame->matrix[coordinateY + i][coordinateX + j] -= 11;
						}
					}
				}
			}
			pGame->matrix[coordinateY][coordinateX] += 22;
			minesSet++;
		}
	}
}

void InilializeCellsAroundMines(Attributes* pGame)
{
	// Checks each element of the matrix: if it doesn't contain mine - checks quantity of mines around and assign the value
	for (int i = 0; i < pGame->rows; i++)
	{
		for (int j = 0; j < pGame->columns; j++)
		{
			if (pGame->matrix[i][j] == 0)
			{
				for (int k = -1; k <= 1; k++)
				{
					for (int l = -1; l <= 1; l++)
					{
						if (i + k >= 0 && i + k < pGame->rows && j + l >= 0 && j + l < pGame->columns)
						{
							if (pGame->matrix[i + k][j + l] % 11 == 0 && pGame->matrix[i + k][j + l] < 0)
							{
								pGame->matrix[i][j] -= 1;
							}
						}
					}
				}
			}
		}
	}
}

void WholeGameFieldOutput(int** const MATRIX, const int ROWS, const int COLUMNS, const int Y, const int X, bool showMines)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	// Colors used in this function
	enum colors { SHOW_MINES = 0x4C, UNREVEALED = 0x2A, CURSOR_NO_INFO = 0x19, RIM = 0x09, CURSOR_WITH_INFO = 0x1F, DEFAULT = 0x0F, SHOW_INFO = 0x0A, MARK = 0xAC, CURSOR_ON_MARK = 0x1D };

	system("cls");

	// Upper game field rim output
	SetConsoleTextAttribute(hConsole, RIM);
	cout << "\n\n\n\t\t" << char(201);
	for (int k = 0; k < COLUMNS * 2 + 2; k++)
	{
		cout << char(205);
	}
	cout << char(187) << endl;

	// Game field and its side rims output
	for (int i = 0; i < ROWS; i++)
	{
		SetConsoleTextAttribute(hConsole, RIM); // Color game field left rim
		cout << "\t\t" << char(186) << " "; // Left rim part of the game field


		for (int j = 0; j < COLUMNS; j++)
		{
			// :: Value :::::: Description ::::::::::::::::::::::: Output :::::::::::::::::::::::
			// ==================================================================================
			// :: 10 ::::::::: Checked, no mines around :::::::::: No background, blank :::::::::
			// :: 0 :::::::::: Unchecked, no mines around :::::::: Background :::::::::::::::::::
			// :: 1...8 :::::: Mines around this cell, revealed :: Show value, no background ::::
			// :: -1...-8 :::: Mines around the cell, unrevealed : Background :::::::::::::::::::
			// :: -11...-88 :: Mine, unrevealed  ::::::::::::::::: Background :::::::::::::::::::
			// :: val x 1000 : Player has marked the cell :::::::: Mark color as background :::::

			// Cursor position
			if (i == Y && j == X)
			{
				// Case: cursor is at player's mark
				if (MATRIX[i][j] < -999 || MATRIX[i][j] > 999)
				{
					if (MATRIX[i][j] == 9999)
					{
						SetConsoleTextAttribute(hConsole, CURSOR_ON_MARK);
					}

					else
					{
						if (showMines == false)
							SetConsoleTextAttribute(hConsole, CURSOR_ON_MARK);

						else if (showMines == true)
							SetConsoleTextAttribute(hConsole, SHOW_MINES);
					}
					cout << "? ";
				}

				// Case: cursor is at cell with mines-nearby
				else if (MATRIX[i][j] >= 1 && MATRIX[i][j] <= 8)
				{
					SetConsoleTextAttribute(hConsole, CURSOR_WITH_INFO);
					cout << MATRIX[i][j] << " ";
				}

				// Case: cursor is at unrevealed/revealed clear cell or at unopened cell with mines nearby
				else if (MATRIX[i][j] == 10 || MATRIX[i][j] == 0 || MATRIX[i][j] <= -1 && MATRIX[i][j] >= -8)
				{
					SetConsoleTextAttribute(hConsole, CURSOR_NO_INFO);
					cout << "  ";
				}

				// Case: cursor is at mined cell
				else if (MATRIX[i][j] % 11 == 0 && MATRIX[i][j] != 0)
				{
					if (showMines == false)
						SetConsoleTextAttribute(hConsole, CURSOR_NO_INFO);

					else if (showMines == true)
						SetConsoleTextAttribute(hConsole, SHOW_MINES);

					cout << "  ";
				}
			}

			// :: Value :::::: Description ::::::::::::::::::::::: Output :::::::::::::::::::::::
			// ==================================================================================
			// :: 10 ::::::::: Checked, no mines around :::::::::: No background, blank :::::::::
			// :: 0 :::::::::: Unchecked, no mines around :::::::: Background :::::::::::::::::::
			// :: 1...8 :::::: Mines around this cell, revealed :: Show value, no background ::::
			// :: -1...-8 :::: Mines around the cell, unrevealed : Background :::::::::::::::::::
			// :: -11...-88 :: Mine, unrevealed  ::::::::::::::::: Background :::::::::::::::::::
			// :: val * 1000 : Player's mark ::::::::::::::::::::: Background with dif. color :::
			// :: 9999 ::::::: Player's mark ::::::::::::::::::::: Background with dif. color :::

			// Ordinary output - no cursor here
			else
			{
				//Case: player's mark on cell with mine nearby or clear unchecker cell
				if (MATRIX[i][j] <= -1000 && MATRIX[i][j] >= -8000 || MATRIX[i][j] >= 1000 && MATRIX[i][j] <= 8000 || MATRIX[i][j] == 9999)
				{
					SetConsoleTextAttribute(hConsole, MARK);
					cout << "? ";
				}

				// Case: mark on mined cell
				else if (MATRIX[i][j] >= 11000 || MATRIX[i][j] <= -11000)
				{
					if (showMines == false)
						SetConsoleTextAttribute(hConsole, MARK);

					else if (showMines == true)
						SetConsoleTextAttribute(hConsole, SHOW_MINES);

					cout << "? ";
				}

				// Case: opened clear cell
				else if (MATRIX[i][j] == 10)
				{
					SetConsoleTextAttribute(hConsole, DEFAULT);
					cout << "  ";
				}

				// Case: unopened cell with no mines around
				else if (MATRIX[i][j] == 0)
				{
					SetConsoleTextAttribute(hConsole, UNREVEALED);
					cout << "  ";
				}

				// Case: cell with mines around was opened
				else if (MATRIX[i][j] >= 1 && MATRIX[i][j] <= 8)
				{
					SetConsoleTextAttribute(hConsole, SHOW_INFO);
					cout << MATRIX[i][j] << " ";
				}

				// Case: unopened cell with mines around
				else if (MATRIX[i][j] <= -1 && MATRIX[i][j] >= -8)
				{
					SetConsoleTextAttribute(hConsole, UNREVEALED);
					cout << "  ";
				}

				// Case: cell contains a mine
				else if (MATRIX[i][j] % 11 == 0 && MATRIX[i][j] != 0)
				{
					if (showMines == false)
						SetConsoleTextAttribute(hConsole, UNREVEALED);

					else if (showMines == true)
						SetConsoleTextAttribute(hConsole, SHOW_MINES);

					cout << "  ";
				}
			}
		}
		SetConsoleTextAttribute(hConsole, RIM); // Color right game field rim
		cout << " " << char(186) << endl; // Right rim part of the game field output
	}

	// Output of game field bottom rim
	cout << "\t\t" << char(200);
	for (int k = 0; k < COLUMNS * 2 + 2; k++)
	{
		cout << char(205);
	}
	cout << char(188) << endl;

	SetConsoleTextAttribute(hConsole, DEFAULT); // Get color to default
}

void MoveAndReaction(Attributes* pGame, COORD* cursor)
{
	int moveDirection(0);

	while (moveDirection != 87 && moveDirection != 119 && moveDirection != 65 && moveDirection != 97 && moveDirection != 83 && moveDirection != 115 && moveDirection != 68 && moveDirection != 100 && moveDirection != 13 && moveDirection != 32 && moveDirection != 48 && moveDirection != 76 && moveDirection != 108)
	{
		moveDirection = _getch();

		// Not to spend PC resources in vain - no need act in any way if user tries to open already cleared cell
		if (moveDirection == 13)
		{
			if (pGame->matrix[pGame->cursor_Y][pGame->cursor_X] == 10 || pGame->matrix[pGame->cursor_Y][pGame->cursor_X] % 100 == 0 && pGame->matrix[pGame->cursor_Y][pGame->cursor_X] != 0)
			{
				moveDirection = 0;
			}
		}
	}

	switch (moveDirection)
	{
	case 87: // Key W
	case 119: // Key w

		if (pGame->cursor_Y > 0)
		{
			CursorPositionOutput(pGame->matrix[pGame->cursor_Y][pGame->cursor_X], *cursor, true);
			(pGame->cursor_Y)--;

			(cursor->Y)--;
			CursorPositionOutput(pGame->matrix[pGame->cursor_Y][pGame->cursor_X], *cursor, false);
		}
		break;

	case 65: // Key A
	case 97: // Key a

		if (pGame->cursor_X > 0)
		{
			CursorPositionOutput(pGame->matrix[pGame->cursor_Y][pGame->cursor_X], *cursor, true);
			(pGame->cursor_X)--;

			(cursor->X) -= 2;
			CursorPositionOutput(pGame->matrix[pGame->cursor_Y][pGame->cursor_X], *cursor, false);
		}
		break;

	case 83: // Key S
	case 115: // Key s

		if (pGame->cursor_Y < pGame->rows - 1)
		{
			CursorPositionOutput(pGame->matrix[pGame->cursor_Y][pGame->cursor_X], *cursor, true);
			(pGame->cursor_Y)++;

			(cursor->Y)++;
			CursorPositionOutput(pGame->matrix[pGame->cursor_Y][pGame->cursor_X], *cursor, false);
		}

		break;

	case 68: // Key D
	case 100: // Key d

		if (pGame->cursor_X < pGame->columns - 1)
		{
			CursorPositionOutput(pGame->matrix[pGame->cursor_Y][pGame->cursor_X], *cursor, true);
			(pGame->cursor_X)++;

			(cursor->X) += 2;
			CursorPositionOutput(pGame->matrix[pGame->cursor_Y][pGame->cursor_X], *cursor, false);
		}
		break;

	case 76: // Key L
	case 108: // Key l
		RulesOutput(pGame, false);
		break;

	case 13: // Enter key

		// :: Value :::::: Description ::::::::::::::::::::::: Enter key reaction :::::::::::::
		// ====================================================================================
		// :: 10 ::::::::: Checked, no mines around :::::::::: Do nothing :::::::::::::::::::::
		// :: 0 :::::::::: Unchecked, no mines around :::::::: Open recursion check :::::::::::
		// :: 1...8 :::::: Mines around this cell, checked ::: Do nothing :::::::::::::::::::::
		// :: -1...-8 :::: Mines around the cell, unrevealed : Convert to positive ::::::::::::
		// :: -11...-88 :: Mine, unrevealed  ::::::::::::::::: Game over ::::::::::::::::::::::
		// :: val * 1000 : Player's mark on mine or near to :: Do nothing :::::::::::::::::::::
		// :: 9999 ::::::: Player's mark on unchecked clear :: Do nothing :::::::::::::::::::::

		// "Stumbled" on a mine ( -11, -22... -88 )
		if (pGame->matrix[pGame->cursor_Y][pGame->cursor_X] % 11 == 0 && pGame->matrix[pGame->cursor_Y][pGame->cursor_X] != 0 && pGame->matrix[pGame->cursor_Y][pGame->cursor_X] <= 88 && pGame->matrix[pGame->cursor_Y][pGame->cursor_X] >= -88)
		{
			pGame->cellsLeftToOpen = -1;
			return;
		}

		// If the cell being open is unchecked and not neighboring mines
		else if (pGame->matrix[pGame->cursor_Y][pGame->cursor_X] == 0)
		{
			ClearNearbySafeCells(pGame, pGame->cursor_Y, pGame->cursor_X, *cursor);

			CursorPositionOutput(pGame->matrix[pGame->cursor_Y][pGame->cursor_X], *cursor, false);
		}

		// If the cell being open is mine-neighbour
		else if (pGame->matrix[pGame->cursor_Y][pGame->cursor_X] <= -1 && pGame->matrix[pGame->cursor_Y][pGame->cursor_X] >= -8)
		{
			pGame->matrix[pGame->cursor_Y][pGame->cursor_X] *= -1;
			(pGame->cellsLeftToOpen)--;

			CursorPositionOutput(pGame->matrix[pGame->cursor_Y][pGame->cursor_X], *cursor, false);

			if (pGame->cellsLeftToOpen == 0)
			{
				return;
			}
		}

	case 32: // Space key

		// Value :::::: Description ::::::::::::::::::::::: Space key reaction ::::
		// ========================================================================
		// 0 :::::::::: Unchecked, no mines around :::::::: Mark ( val += 9999) :::
		// -1...-8 :::: Mines around the cell, unrevealed : Mark (val * 1000) :::::
		// -11...-88 :: Mine, unrevealed  ::::::::::::::::: Mark (val * 1000) :::::
		// val * 1000 : Player's mark on mine or near to :: Unmark (value / 1000) :
		// 9999 ::::::: Player's mark on unchecked clear :: Unmark (value = 0) ::::

		// Wanna mark unopened clear cell
		if (pGame->matrix[pGame->cursor_Y][pGame->cursor_X] == 0)
		{
			pGame->matrix[pGame->cursor_Y][pGame->cursor_X] = 9999;

		}

		// Wanna mark unrevealed cell with mines around
		else if (pGame->matrix[pGame->cursor_Y][pGame->cursor_X] <= -1 && pGame->matrix[pGame->cursor_Y][pGame->cursor_X] >= -8)
		{
			pGame->matrix[pGame->cursor_Y][pGame->cursor_X] *= 1000;
		}

		// Wanna mark unrevealed mined cell
		else if (pGame->matrix[pGame->cursor_Y][pGame->cursor_X] <= -11 && pGame->matrix[pGame->cursor_Y][pGame->cursor_X] >= -88)
		{
			pGame->matrix[pGame->cursor_Y][pGame->cursor_X] *= 1000;
		}

		// Wanna unmark cell
		else if (pGame->matrix[pGame->cursor_Y][pGame->cursor_X] < -999 || pGame->matrix[pGame->cursor_Y][pGame->cursor_X] >999)
		{
			if (pGame->matrix[pGame->cursor_Y][pGame->cursor_X] == 9999)
				pGame->matrix[pGame->cursor_Y][pGame->cursor_X] = 0;

			else
				pGame->matrix[pGame->cursor_Y][pGame->cursor_X] /= 1000;
		}
		CursorPositionOutput(pGame->matrix[pGame->cursor_Y][pGame->cursor_X], *cursor, false);

		break;

	case 48: // 0 key
		pGame->cellsLeftToOpen = -1111; // Return to main menu
	}
}

void CursorPositionOutput(const int valueInCurrentCell, COORD const cursor, bool isPastCursorPosition)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	enum colors { UNREVEALED = 0x2A, CURSOR_NO_INFO = 0x19, CURSOR_WITH_INFO = 0x1F, DEFAULT = 0x0F, SHOW_INFO = 0x0A, MARK = 0xAC, CURSOR_ON_MARK = 0x1D };

	SetConsoleCursorPosition(hConsole, cursor);


	// Case: player's mark here
	if (valueInCurrentCell <= -1000 || valueInCurrentCell >= 1000)
	{
		if (isPastCursorPosition == false)
			SetConsoleTextAttribute(hConsole, CURSOR_ON_MARK);

		else if (isPastCursorPosition == true)
			SetConsoleTextAttribute(hConsole, MARK);

		cout << "? ";
	}

	// Case: opened clear cell
	else if (valueInCurrentCell == 10)
	{
		if (isPastCursorPosition == false)
			SetConsoleTextAttribute(hConsole, CURSOR_NO_INFO);

		else if (isPastCursorPosition == true)
			SetConsoleTextAttribute(hConsole, DEFAULT);

		cout << "  ";
	}

	// Case: unopened clear cell
	else if (valueInCurrentCell == 0)
	{
		if (isPastCursorPosition == false)
			SetConsoleTextAttribute(hConsole, CURSOR_NO_INFO);

		else if (isPastCursorPosition == true)
			SetConsoleTextAttribute(hConsole, UNREVEALED);

		cout << "  ";
	}

	// Case: opened cell with mines around
	else if (valueInCurrentCell >= 1 && valueInCurrentCell <= 8)
	{
		if (isPastCursorPosition == false)
			SetConsoleTextAttribute(hConsole, CURSOR_WITH_INFO);

		else if (isPastCursorPosition == true)
			SetConsoleTextAttribute(hConsole, SHOW_INFO);

		cout << valueInCurrentCell << " ";
	}

	// Case: unopened cell with mines around
	else if (valueInCurrentCell <= -1 && valueInCurrentCell >= -8)
	{
		if (isPastCursorPosition == false)
			SetConsoleTextAttribute(hConsole, CURSOR_NO_INFO);

		else if (isPastCursorPosition == true)
			SetConsoleTextAttribute(hConsole, UNREVEALED);

		cout << "  ";
	}

	// Case: cell contains a mine
	else if (valueInCurrentCell % 11 == 0 && valueInCurrentCell != 0)
	{
		if (isPastCursorPosition == false)
			SetConsoleTextAttribute(hConsole, CURSOR_NO_INFO);

		else if (isPastCursorPosition == true)
			SetConsoleTextAttribute(hConsole, UNREVEALED);

		cout << "  ";
	}

	COORD getCursorOutOfField = { 0,0 };
	SetConsoleTextAttribute(hConsole, DEFAULT);
	SetConsoleCursorPosition(hConsole, getCursorOutOfField);
}

void ClearNearbySafeCells(Attributes* pGame, const int Y, const int X, COORD cursor)
{
	// Y & X - coordinates of currently checked cell of the matrix. 
	// Not using pGame fields because in the end of recursion calls I got to have cursor where it was when recursion started


	// Base case
	if (Y < 0 || Y >= pGame->rows || X < 0 || X >= pGame->columns || pGame->cellsLeftToOpen == 0 || pGame->matrix[Y][X] != 0)
	{
		return;
	}

	// If the function was called and base case was not executed, it means the cell should be opened
	pGame->matrix[Y][X] = 10;
	(pGame->cellsLeftToOpen)--;
	CursorPositionOutput(pGame->matrix[pGame->cursor_Y][pGame->cursor_X], cursor, true);

	// Check all the cells around 
	for (int i = -1; i <= 1; i++)
	{
		for (int j = -1; j <= 1; j++)
		{
			// :: Value :::: Description ::::::::::::::::::::::: What to do with value :
			// =========================================================================
			// :: 0 :::::::: Unchecked, no mines around :::::::: Open, call recursion ::
			// :: -1...-8 :: Mines around the cell, unchecked :: Open (val * - 1) ::::::

			if (Y + i >= 0 && Y + i < pGame->rows && X + j >= 0 && X + j < pGame->columns)
			{
				COORD tempCursor = cursor;

				// Open cell that neighbors mined cell
				if (pGame->matrix[Y + i][X + j] <= -1 && pGame->matrix[Y + i][X + j] >= -8)
				{
					pGame->matrix[Y + i][X + j] *= -1;
					(pGame->cellsLeftToOpen)--;

					tempCursor.Y += i;
					tempCursor.X += (j * 2);

					CursorPositionOutput(pGame->matrix[Y + i][X + j], tempCursor, true);
				}

				// Recursively open cell that is not neighbouring mines
				else if (pGame->matrix[Y + i][X + j] == 0)
				{
					tempCursor.Y += i;
					tempCursor.X += (j * 2);

					ClearNearbySafeCells(pGame, Y + i, X + j, tempCursor);
				}
			}
		}
	}
}

void SettingsReset(Attributes* pGame, COORD* cursor)
{
	for (int j = 0; j < pGame->rows; j++)
		for (int i = 0; i < pGame->columns; i++)
			pGame->matrix[j][i] = 0;

	pGame->cursor_Y = 0;
	pGame->cursor_X = 0;

	pGame->cellsLeftToOpen = pGame->rows * pGame->columns - pGame->minesQuantity;

	cursor->X = 18;
	cursor->Y = 4;
}

void RulesOutput(Attributes* const pGame, bool isFunctionWasCalledFromMainMenu)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	system("cls");
	SetConsoleTextAttribute(hConsole, 0x0F);
	cout << "\n\t\t\t   Rules" << endl;
	SetConsoleTextAttribute(hConsole, 0x0A);
	cout << "  ";

	for (int i = 57; i > 0; i--)
		cout << char(176);
	cout << endl;

	cout << "  " << char(176) << char(176);
	SetConsoleTextAttribute(hConsole, 0x0F);
	cout << "* Click a cell, you get a number. That number is     ";
	SetConsoleTextAttribute(hConsole, 0x0A);
	cout << char(176) << char(176) << endl;
	cout << "  " << char(176) << char(176);
	SetConsoleTextAttribute(hConsole, 0x0F);
	cout << " quantity of mined cells around opened cell.         ";
	SetConsoleTextAttribute(hConsole, 0x0A);
	cout << char(176) << char(176) << endl;
	cout << "  " << char(176) << char(176);
	SetConsoleTextAttribute(hConsole, 0x0F);
	cout << "* If you are sure you detected a mine, you can open  ";
	SetConsoleTextAttribute(hConsole, 0x0A);
	cout << char(176) << char(176) << endl;
	cout << "  " << char(176) << char(176);
	SetConsoleTextAttribute(hConsole, 0x0F);
	cout << " revealed cells around it, opening more areas.       ";
	SetConsoleTextAttribute(hConsole, 0x0A);
	cout << char(176) << char(176) << endl;
	cout << "  " << char(176) << char(176);
	SetConsoleTextAttribute(hConsole, 0x0F);
	cout << "* Hint 1: click some corner cell at the start:       ";
	SetConsoleTextAttribute(hConsole, 0x0A);
	cout << char(176) << char(176) << endl;
	cout << "  " << char(176) << char(176);
	SetConsoleTextAttribute(hConsole, 0x0F);
	cout << " often it'd give you a good start!                   ";
	SetConsoleTextAttribute(hConsole, 0x0A);
	cout << char(176) << char(176) << endl;
	cout << "  " << char(176) << char(176);
	SetConsoleTextAttribute(hConsole, 0x0F);
	cout << "* Hint 2: Mark all the cells that are OBVIOUSLY      ";
	SetConsoleTextAttribute(hConsole, 0x0A);
	cout << char(176) << char(176) << endl;
	cout << "  " << char(176) << char(176);
	SetConsoleTextAttribute(hConsole, 0x0F);
	cout << " mined (such as eight 1's surrounding an unopened    ";
	SetConsoleTextAttribute(hConsole, 0x0A);
	cout << char(176) << char(176) << endl;
	cout << "  " << char(176) << char(176);
	SetConsoleTextAttribute(hConsole, 0x0F);
	cout << " square - it's obviously a mine).                    ";
	SetConsoleTextAttribute(hConsole, 0x0A);
	cout << char(176) << char(176) << endl;
	cout << "  " << char(176) << char(176) << " <> <> <> <> <> <> <>";
	SetConsoleTextAttribute(hConsole, 0x0F);
	cout << " Have fun! ";
	SetConsoleTextAttribute(hConsole, 0x0A);
	cout << "<> <> <> <> <> <> <> " << char(176) << char(176) << endl;

	cout << "  ";
	for (int i = 57; i > 0; i--)
		cout << char(176);
	cout << endl;

	SetConsoleTextAttribute(hConsole, 0x0F);
	cout << "\n\t\t    Controls & Map legend" << endl;
	SetConsoleTextAttribute(hConsole, 0x0A);

	cout << "\t ";
	for (int i = 43; i > 0; i--)
		cout << char(176);
	cout << endl;

	cout << "\t " << char(176) << char(176);
	SetConsoleTextAttribute(hConsole, 0x0F);
	cout << " Blue cell ----------- cursor position ";
	SetConsoleTextAttribute(hConsole, 0x0A);
	cout << char(176) << char(176) << endl;
	cout << "\t " << char(176) << char(176);
	SetConsoleTextAttribute(hConsole, 0x0F);
	cout << " Green cell ---------- terra incognita ";
	SetConsoleTextAttribute(hConsole, 0x0A);
	cout << char(176) << char(176) << endl;
	cout << "\t " << char(176) << char(176);
	SetConsoleTextAttribute(hConsole, 0x0F);
	cout << " Yellow \"?\" --- mark at potential mine ";
	SetConsoleTextAttribute(hConsole, 0x0A);
	cout << char(176) << char(176) << endl;
	cout << "\t " << char(176) << char(176);
	SetConsoleTextAttribute(hConsole, 0x0F);
	cout << " 1/2/3 --------- mines quantity around ";
	SetConsoleTextAttribute(hConsole, 0x0A);
	cout << char(176) << char(176) << endl;
	cout << "\t " << char(176) << char(176);
	SetConsoleTextAttribute(hConsole, 0x0F);
	cout << " WASD keys --------------- move cursor ";
	SetConsoleTextAttribute(hConsole, 0x0A);
	cout << char(176) << char(176) << endl;
	cout << "\t " << char(176) << char(176);
	SetConsoleTextAttribute(hConsole, 0x0F);
	cout << " Enter key --------- open current cell ";
	SetConsoleTextAttribute(hConsole, 0x0A);
	cout << char(176) << char(176) << endl;
	cout << "\t " << char(176) << char(176);
	SetConsoleTextAttribute(hConsole, 0x0F);
	cout << " Space key --------- mark current cell ";
	SetConsoleTextAttribute(hConsole, 0x0A);
	cout << char(176) << char(176) << endl;
	cout << "\t " << char(176) << char(176);
	SetConsoleTextAttribute(hConsole, 0x0F);
	cout << " Zero key ----------------- close game ";
	SetConsoleTextAttribute(hConsole, 0x0A);
	cout << char(176) << char(176) << endl;
	cout << "\t " << char(176) << char(176);


	for (int i = 41; i > 0; i--)
		cout << char(176);
	cout << endl << "\t ";

	cout << char(176) << char(176) << "  <>";
	SetConsoleTextAttribute(hConsole, 0x0F);
	cout << " Press any key to resume game ";
	SetConsoleTextAttribute(hConsole, 0x0A);
	cout << " <>  " << char(176) << char(176) << endl << "\t ";

	for (int i = 43; i > 0; i--)
		cout << char(176);
	cout << endl;

	SetConsoleTextAttribute(hConsole, 0x0F);

	int justToPauseExecution;
	justToPauseExecution = _getch();

	if (isFunctionWasCalledFromMainMenu == false)
		WholeGameFieldOutput(pGame->matrix, pGame->rows, pGame->columns, pGame->cursor_Y, pGame->cursor_X);
}

inline void CaseWinningGameOuput()
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	// Shows phrase "YOU LOST"
	SetConsoleTextAttribute(hConsole, 0x0A);

	cout << "  ";

	for (int i = 56; i > 0; i--)
	{
		cout << char(176);
	}
	cout << endl;

	cout << "  __   __   ___    _   _    __       __   ___    _  _   _" << endl;
	cout << "  \\ \\ / /  / _ \\  | | | |   \\ \\     / /  / _ \\  | \\| | | |" << endl;
	cout << "   \\ V /  | (_) | | |_| |    \\ \\ /\\/ /  | (_) | | .` | |_|" << endl;
	cout << "    |_|    \\___/   \\___/      \\_ /\\_/    \\___/  |_|\\_| (_)" << endl;
	SetConsoleTextAttribute(hConsole, 0x0F);
}

inline void ShowGameTime(unsigned int gameStartTime)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	unsigned int gameDuration = (clock() - gameStartTime) / CLOCKS_PER_SEC; //value in seconds

	int seconds = gameDuration % 60;
	int minutes = gameDuration / 60;

	SetConsoleTextAttribute(hConsole, 0x0A);

	cout << "  ";

	for (int i = 56; i > 0; i--)
	{
		cout << char(176);
	}
	cout << endl;

	SetConsoleTextAttribute(hConsole, 0x0F);

	cout << "  GOOD JOB, COMRADE! YOUR TIME: ";

	if (minutes > 0)
	{
		cout << minutes << " MINUTE(S) ";
	}

	if (seconds > 0)
	{
		cout << seconds << " SECOND(S)" << endl;
	}

	SetConsoleTextAttribute(hConsole, 0x0A);

	cout << "  ";

	for (int i = 56; i > 0; i--)
	{
		cout << char(176);
	}
	cout << endl;

	SetConsoleTextAttribute(hConsole, 0x0F);
	cout << "  Press any key to continue";
	gameDuration = _getch();
}

inline void CaseLosingGameOutput()
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	// Shows phrase "YOU LOST"
	SetConsoleTextAttribute(hConsole, 0x0C);
	cout << "  ";

	for (int i = 57; i > 0; i--)
	{
		cout << char(176);
	}
	cout << endl;

	cout << "  __   __   ___    _   _     _      ___    ___   _____   _" << endl;
	cout << "  \\ \\ / /  / _ \\  | | | |   | |    / _ \\  / __| |_   _| | |" << endl;
	cout << "   \\ V /  | (_) | | |_| |   | |__ | (_) | \\__ \\   | |   |_|" << endl;
	cout << "    |_|    \\___/   \\___/    |____| \\___/  |___/   |_|   (_)" << endl;

	cout << "  ";

	for (int i = 57; i > 0; i--)
	{
		cout << char(176);
	}
	cout << endl;

	SetConsoleTextAttribute(hConsole, 0x0F);

	int justToFreezeFinalOutput;
	cout << "  Press any key to continue";
	justToFreezeFinalOutput = _getch();
}