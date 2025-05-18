#include<iostream>
#include<cstdlib>
#include<Windows.h>
#include<conio.h>
#include<iomanip>
#include<stdio.h>
#include<time.h>
#include<fstream>
#include<string.h>
//rastar font 8x8

time_t g;
#define rows 10
#define cols 10
#define BLACK 0
#define BROWN 6
#define WHITE 15
#define GREEN 2
#define RED 4
#define LBLUE 9
struct record {
	uint32_t time;
	uint32_t wins = 0;
	uint32_t loses = 0;
	uint32_t total_played = 0;

};
struct account {
	char username[20];
	char password[20];
	record rec;
	char islogged_in = '0';
};

struct pos {
	char rowpos[4];
	char colpos[4];
};
struct cell {
	char isopen = '0';
	char isflag = '0';
	char x = '0';
	char value;
	char isend = '0';

};

void SetClr(int tcl, int bcl)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (tcl + (bcl * 16)));
}


void getRowColbyLeftClick(int& rpos, int& cpos)
{
	HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
	DWORD Events;
	INPUT_RECORD InputRecord;
	SetConsoleMode(hInput, ENABLE_PROCESSED_INPUT | ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS);
	do
	{
		char f = 't';
		ReadConsoleInput(hInput, &InputRecord, 1, &Events);
		if (InputRecord.Event.MouseEvent.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED)
		{
			cpos = InputRecord.Event.MouseEvent.dwMousePosition.X;
			f = 'f';
			if (f == 'f')
				rpos = InputRecord.Event.MouseEvent.dwMousePosition.Y;
			break;
		}
	} while (true);
}
bool isboundary(char& a, char& b)
{
	if (a == '0' || b == '9' || b == '0' || a == '0')
	{
		return true;
	}
	return false;
}

void gotoRowCol(int rpos, int cpos)
{
	COORD scrn;
	HANDLE hOuput = GetStdHandle(STD_OUTPUT_HANDLE);
	scrn.X = cpos;
	scrn.Y = rpos;
	SetConsoleCursorPosition(hOuput, scrn);
}
void getclick(int celllength, int cellwidth, int& pr, int& cr, char& p)
{
	int r;
	int c;
	getRowColbyLeftClick(r, c);
	pr = (r / celllength) - 2;
	cr = (c / cellwidth) - 2;
	pr++;
	cr++;
	gotoRowCol(p, p);

	//std::cout << "ROW: " << pr << " COL: " << cr ;

}

const int dx[] = { -1, -1,-1,0,0,1,1,1 };
const int dy[] = { -1,0,1, -1,1,-1,0,1 };

//void thenumber()
//{
//	srand(time(0));
//	int w = 32;
//	int grid[12][12];
//	int sgrid[12][12];
//	for (unsigned char i = 1; i <= 10; i++)
//	{
//		for (int j = 1; j <= 10; j++)
//		{
//			sgrid[i][j];
//		}
//	}
//}
void printcellborder(int row, int col, int celllength, int rowlength, cell**& mines)
{
	int rp = row * celllength;
	int cp = col * rowlength;
	gotoRowCol(rp, cp);
	for (int k = 0; k < rowlength; k++)
	{
		std::cout << char(-37);
	}
	for (int t = 0; t < celllength; t++)
	{
		gotoRowCol(rp + t, cp);
		std::cout << char(-37);
	}
	rp += celllength;
	gotoRowCol(rp, cp);
	for (int k = 0; k <= rowlength; k++)
	{
		std::cout << char(-37);
	}
	rp -= celllength;
	cp += rowlength;
	for (int t = 0; t < celllength; t++)
	{
		gotoRowCol(rp + t, cp);
		std::cout << char(-37);
	}
}
char table[20][20];
///(*(char*)(*(*table + 20) + 20));
void create()
{
	for (unsigned char s = 0; s < 20; s++)
	{
		for (unsigned char o = 0; o < 20; o++)
		{
			*(*(table + s) + o) = '*';
		}
	}
}
void placemines(cell** mines, char& p)
{
	srand(time(0));
	char x, y;
	for (unsigned char i = 0; i < p; i++)
	{
		x = rand() % p;
		y = rand() % p;
		if (*(char*)((char*)((*(mines + x) + y)) + 3) == 0)
		{
			*(char*)((char*)((*(mines + x) + y)) + 3) = -1;
		}
		else
			i--;
	}
}
void calculateneighbours(cell** mines, char p)
{
	char count = 0;
	for (unsigned char i = 0; i < p; i++) {
		for (unsigned char j = 0; j < p; j++)
		{
			if (*(char*)((char*)((*(mines + i) + j)) + 3) == 0)
			{
				char count = 0;
				for (unsigned char k = 0; k < 8; k++)
				{
					char new_x = i + *(dx + k);
					char  new_y = j + *(dy + k);
					if (new_x >= 0 && new_x < p && new_y >= 0 && new_y < p)
					{
						if (*(char*)((char*)((*(mines + new_x) + new_y)) + 3) == -1)
						{
							count++;
						}
					}
					*(char*)((char*)((*(mines + i) + j)) + 3) = count;
				}
			}
		}
	}
}
void init(cell**& mines, char p)
{
	mines = new cell * [p];
	for (unsigned char i = 0; i < p; i++)
	{
		*(mines + i) = new cell[p]{};
	}
	placemines(mines, p);
	calculateneighbours(mines, p);
}

void printgrid(const int& totalrows, const int& totalcols, cell**& mines)
{

	std::cout << "  ";
	for (int i = 0; i < totalrows; i++)
		std::cout << std::setw(2) << " " << i;
	std::cout << std::endl;
	for (int i = 1, y = 0; i <= totalrows && y <= totalrows; i++, y++)
	{

		std::cout << std::endl << std::setw(2.75) << y;
		for (unsigned char c = 1, j = 0; c <= totalcols && j <= totalcols; c++, j++)
		{

			if (j == 0)
			{

				std::cout << std::endl << "   ";
				if (y == 0)
				{
					std::cout << std::endl << "   ";
				}
				if (*(*(table + y) + j) == '*')
				{
					SetClr(BLACK, RED);
				}
				else if (*(*(table + y) + j) == 'F')
				{
					SetClr(BLACK, BROWN);
				}
				else if (*(*(table + y) + j) == 'X')
				{
					SetClr(RED, GREEN);
				}
				std::cout << std::setw(3.5) << *(*(table + y) + j);
				SetClr(WHITE, BLACK);
			}
			else
			{
				if (*(*(table + y) + j) == '*')
				{
					SetClr(BLACK, RED);
				}
				else if (*(*(table + y) + j) == 'F')
				{
					SetClr(GREEN, BROWN);
				}
				else if (*(*(table + y) + j) == 'X')
				{
					SetClr(RED, GREEN);
				}
				std::cout << std::setw(2.5) << *(*(table + y) + j);
				SetClr(WHITE, BLACK);

			}
			printcellborder(i, c, 3, 3, mines);

		}
		std::cout << std::endl;
	}

}
bool isvalidpos(pos position, char p)
{
	return (*((int*)(&position) + 0) >= 0 && *((int*)(&position) + 0) < p && *((int*)(&position) + 1) >= 0 && *((int*)(&position) + 1) < p);
}
void selectposition(pos& position)
{
	getRowColbyLeftClick(*((int*)(&position) + 0), *((int*)(&position) + 1));
	gotoRowCol(*((int*)(&position) + 0), *((int*)(&position) + 1));
	SetClr(RED, BLACK);
	std::cout << "X";
	SetClr(WHITE, BLACK);
}
void print(cell**& mines, char p)
{
	for (unsigned char i = 0; i < p; i++)
	{
		for (unsigned char j = 0; j < p; j++)
		{
			if (*(char*)((char*)((*(mines + i) + j)) + 1) == '1')
			{
				std::cout << "F";
			}
			else if (*(char*)((char*)((*(mines + i) + j)) + 0) == '1')
			{
				if (*(char*)((char*)((*(mines + i) + j)) + 3) == -1)
				{
					std::cout << "X";
				}
				else if (*(char*)((char*)((*(mines + i) + j)) + 3) == 0)
				{
					std::cout << " ";
				}
				else
				{
					std::cout << *(char*)((char*)((*(mines + i) + j)) + 3);
				}
			}
		}
	}
}

char mine_positions[20][20];
char symbol;
int flag_counter = 0;
int mines_flagged_counter = 0;
//char table2[10][10];
bool gamelose = false;
void tab(char table2[][20])
{
	for (unsigned char p = 0; p < 20; p++)
	{
		for (unsigned char l = 0; l < 20; l++)
		{
			*(*(table + p) + l) = '*';
		}
	}
}

const int cap = 1000;
struct po {
	char  x;
	char y;
};
void floodfill2(cell**& mines, int i, int j, char f)
{
	po coordinates[cap];
	char s = 0, e = 0;

	*(char*)((char*)(coordinates + e) + 0) = i;
	*(char*)((char*)(coordinates + e) + 1) = j;
	e++;

	while (s != e)
	{
		char p = *(char*)((char*)(coordinates + s) + 0);
		char q = *(char*)((char*)(coordinates + s) + 1);

		s++;
		if (*(char*)((char*)((*(mines + p) + q)) + 3) == 0 || *(char*)((char*)((*(mines + p) + q)) + 2) == '1')
		{
			for (unsigned char k = 0; k < 8; k++)
			{
				//char x = '0';
				char new_x = p + *(dx + k);
				char new_y = q + *(dy + k);
				if (new_x >= 0 && new_x < f && new_y >= 0 && new_y < f)
				{
					if (*(*(table + new_x) + new_y) == '*' && *(char*)((char*)((*(mines + new_x) + new_y)) + 3) != -1 && *(*(table + new_x) + new_y) != 'F')
					{
						if (*(char*)((char*)((*(mines + new_x) + new_y)) + 3) == 0 && *(*(table + new_x) + new_y) == '*' && *(char*)((char*)((*(mines + p) + q)) + 2) == '0')
						{
							*(char*)((char*)(coordinates + e) + 0) = new_x;
							*(char*)((char*)(coordinates + e) + 1) = new_y;

							e++;
						}
						if (*(char*)((char*)((*(mines + new_x) + new_y)) + 3) == 0 && *(*(table + new_x) + new_y) == '*' && *(char*)((char*)((*(mines + p) + q)) + 2) == '1')
						{
							*(char*)((char*)(coordinates + e) + 0) = new_x;
							*(char*)((char*)(coordinates + e) + 1) = new_y;

							e++;
						}

						*(*(table + new_x) + new_y) = *(char*)((char*)((*(mines + new_x) + new_y)) + 3) + 48;
						*(char*)((char*)((*(mines + new_x) + new_y)) + 0) = true;
					}
					if (*(char*)((char*)((*(mines + new_x) + new_y)) + 3) == -1 && *(*(table + new_x) + new_y) != 'F')
					{
						*(*(table + new_x) + new_y) = 'X';
						gamelose = true;
					}

				}
			}
		}
	}

}
void floodfill3(cell**& mines, int i, int j, char f)
{

	char countf = '0';
	char p = i;
	char q = j;
	if (*(*(table + i) + j) != '*')
	{
		for (unsigned char k = 0; k < 8; k++)
		{
			//char x = '0';
			char new_x = p + dx[k];
			char new_y = q + dy[k];
			if (new_x >= 0 && new_x < f && new_y >= 0 && new_y < f)
			{
				if (*(*(table + new_x) + new_y) == 'F')
				{
					countf++;
				}
			}

		}
		if ((countf - 48) == *(char*)((char*)((*(mines + p) + q)) + 3))
		{

			*(char*)((char*)((*(mines + p) + q)) + 2) = '1';
			floodfill2(mines, p, q, f);

		}
	}


}
void place_or_remove_flag(cell**& mines, pos position)
{

	char i = *((int*)(&position) + 0);
	char j = *((int*)(&position) + 1);

	if (*(*(table + i) + j) == '*')
	{
		*(*(table + i) + j) = 'F';
		flag_counter++;

		if (*(char*)((char*)((*(mines + i) + j)) + 3) == -1)
			mines_flagged_counter++;
	}

	else if (*(*(table + i) + j) == 'F')
	{
		*(*(table + i) + j) = '*';
		flag_counter--;
		if (*(char*)((char*)((*(mines + i) + j)) + 3) == -1)
			mines_flagged_counter--;
	}
}

bool win_check(char p)
{
	if (flag_counter == p && mines_flagged_counter == p)
		return 1;
	else
		return 0;
}
void open_cell(cell**& mines, pos& position, char p)
{

	char i = *((int*)(&position) + 0);
	char j = *((int*)(&position) + 1);

	if (*(char*)((char*)((*(mines + i) + j)) + 3) == -1)
	{
		*(*(table + i) + j) = 'X';
		*(char*)((char*)((*(mines + i) + j)) + 4) = '1';

		for (unsigned char i = 0; i < p; i++)
			for (unsigned char j = 0; j < p; j++)
				if (*(char*)((char*)((*(mines + i) + j)) + 3) == -1)
					*(*(table + i) + j) = 'X';
		gamelose = true;
		win_check;
	}
	else if (*(*(table + i) + j) != '*')
	{
		{
			floodfill3(mines, i, j, p);
		}
	}
	else
	{
		if (*(*(table + i) + j) == '*' && *(char*)((char*)((*(mines + i) + j)) + 3) != -1)
		{
			*(*(table + i) + j) = *(char*)((char*)((*(mines + i) + j)) + 3) + 48;
			if (*(char*)((char*)((*(mines + i) + j)) + 3) == 0)
			{
				floodfill2(mines, i, j, p);

			}
		}
		*(char*)((char*)((*(mines + i) + j)) + 1) = '1';

	}

}

//void up(account& ac, char l[4], char b, char z, char w, char r[], char& i, char u);
void game(account& users, char& p, time_t g, time_t t)
{
	account ac;
	cell** mines = nullptr;
	//create_table(mines);
	char table[20][20];

	create();
	init(mines, p);
	printgrid(p, p, mines);
	pos position;
	char x;
	print(mines, p);
	do
	{

		g = time(0);
		std::cout << std::endl;
		std::cout << "enter f if want to flag, o to play, f again to unflag" << std::endl;
		std::cin >> x;
		getclick(3, 3, *((int*)(&position) + 0), *((int*)(&position) + 1), p);
		if (x == 'f')
		{
			place_or_remove_flag(mines, position);
		}
		//selectposition(position);

		else
		{
			open_cell(mines, position, p);
		}
		system("cls");
		printgrid(p, p, mines);
		std::cout << std::endl << "Time:" << g - t << std::endl;
	} while (isvalidpos(position, p) && !gamelose && !win_check(p));
	//reveal(i, j, mines);
	std::cout << std::endl;

	if (gamelose) {
		system("cls");
		printgrid(p, p, mines);
		std::cout << std::endl << "GAME OVER" << std::endl;
		std::cout << std::endl << "Time:" << g - t << std::endl;
		//*((int*)(&l) + 0) = g - t;
		users.rec.time = g - t;
		users.rec.loses++;
	}

	if (win_check(p))
	{
		std::cout << std::endl << "YOU WIN!" << std::endl;
		std::cout << std::endl << "Time:" << g - t << std::endl;
		users.rec.time = g - t;
		users.rec.wins++;
	}
	/**((int*)(&w) + 0) = *((int*)(&z) + 0) + *((int*)(&b) + 0);*/
	users.rec.total_played = users.rec.loses + users.rec.wins;

	system("cls");

}

void re(account& ac, char r[]);

int main1(account& users, time_t g, time_t t)
{
	account ac;
	char x;
	char p = 0;
	std::cout << "enter the difficulty level: e for easy, m for medium, h for hard!";
	std::cin >> x;
	switch (x)
	{
	case'h':
		p = 15;
		break;
	case 'm':
		p = 10;
		break;
	case 'e':
		p = 8;
		break;
	}

	system("cls");
	cell** mines = nullptr;
	game(users, p, g, t);


	return 0;
}


void swapping(account& a, account& b)
{
	account k = a;
	a = b;
	b = k;
}
bool areEqual(char arr1[], const char* arr2) {
	int i = 0;
	while (arr1[i] != '\0' || arr2[i] != '\0') {
		if (arr1[i] != arr2[i]) {
			return false;
		}
		i++;
	}
	return true;
}
void signup(account& user)
{
	char username[20];
	char password[20];
	std::cout << "Enter your Username: ";
	std::cin >> user.username;
	std::cout << "Enter your Password: ";
	std::cin >> user.password;
	account k;
	std::ofstream ofs("account.bin", std::ios::binary | std::ios::app);
	if (!ofs)
	{
		std::cout << " FILE NOT OPENED!! ";
	}
	else
	{
		ofs.write((char*)&user, sizeof(user));
		ofs.close();
	}

}
void login(account& user, char& i)
{
	char username[20];
	char password[20];
	std::cout << "Enter your Username: ";
	std::cin >> username;
	std::cout << "Enter your Password: ";
	std::cin >> password;
	account poo;
	std::ifstream ifs("account.bin", std::ios::binary);
	if (!ifs)
	{
		std::cout << " FILE NOT OPENED!! ";
	}
	else
	{
		while (ifs.read((char*)&poo, sizeof(poo)))
		{
			if (areEqual(poo.username, username) == true && areEqual(poo.password, password) == true)
			{
				user.islogged_in = '1';
				user = poo;
				std::cout << "Logged in successfully" << std::endl;
				ifs.close();
				i = '0';
			}

		}
		if (areEqual(poo.username, username) == false || areEqual(poo.password, password) == false)
		{
			std::cout << "unsuccessful login..." << std::endl;
			i = '1';

		}
	}
	ifs.close();
}

void updateUser(account& user)
{
	std::fstream ifs("account.bin", std::ios::binary | std::ios::in | std::ios::out);
	account poo;
	while (ifs.read((char*)&poo, sizeof(poo)))
	{
		if (areEqual(poo.username, user.username) == true)
		{
			ifs.seekp(-60, std::ios::cur);
			ifs.write((char*)&user, sizeof(user));
			break;
		}
	}
	ifs.close();
}

void User(account user)
{
	std::cout << "Username: " << user.username << std::endl;
	std::cout << "Games Played: " << user.rec.total_played + 0 << std::endl;
	std::cout << "Wins: " << user.rec.wins + 0 << std::endl;
	std::cout << "Loses: " << user.rec.loses + 0 << std::endl;
	std::cout << "Least Time: " << user.rec.time + 0 << std::endl;
}

void bubbleSort(account users[], char size)
{
	for (char i = 0; i < size; i++)
	{
		for (char j = 0; j < size - i - 1; j++)
		{
			if (users[j].rec.time > users[j + 1].rec.time)
			{
				swapping(users[j], users[j + 1]);
			}
		}
	}
}
void dis()
{
	std::cout << std::endl;
	std::cout << "SCOREBOARD:" << std::endl;
	std::cout << std::endl;
	std::cout << "Sr.\t";
	std::cout << "Username" << "\t";
	std::cout << "Time\t";
	std::cout << "Games\t";
	std::cout << "Wins\t";
	std::cout << "Loses \t";
	std::cout << std::endl;

}
void displayboard()
{
	std::ifstream read("account.bin", std::ios::binary);
	std::cout << std::endl;
	account users[1000];
	char k = 0;
	account poo;
	while (read.read((char*)&poo, sizeof(poo)))
	{
		*(users + k) = poo;
		k++;
	}
	read.close();
	bubbleSort(users, k);
	dis();
	for (char i = 0; i < k; i++)
	{
		std::cout << i + 1 << "\t";
		std::cout << users[i].username << std::setw(15);
		std::cout << users[i].rec.time << "\t";
		std::cout << users[i].rec.total_played + 0 << "\t";
		std::cout << users[i].rec.wins + 0 << "\t";
		std::cout << users[i].rec.loses + 0 << std::endl;
	}
}
int main()
{
	account users;
	system("cls");
	std::cout << std::endl;
	char x;
	char r = '0';
	char i = '0';
	std::cout << "enter the program to run from the following \n 1.signup \n 2.login\n";
	std::cin >> x;
	while (r == '0')
	{
		switch (x)
		{
		case '1':
			r = '1';
			signup(users);

			break;
		case '2':
			r = '1';
			login(users, i);

			break;
		}
		if (r == '0')
		{
			std::cout << "enter the program to run from the following \n 1.signup \n 2.login\n";
			std::cin >> x;
		}
	}
	if (i != '1')
	{
		system("cls");
		time_t t = time(0);
		main1(users, g, t);
		updateUser(users);
		char d;
		std::cout << "DO YOU WANT TO DISPLAY YOUR DATA?? (enter y for yes and n for no)" << std::endl;
		std::cin >> d;
		if (d == 'y')
		{
			std::cout << std::endl;
			std::cout << "Your DATA: " << std::endl;
			User(users);
		}
		char f;
		std::cout << "DO YOU WANT TO DISPLAY THE LEADER BOARD?? (enter y for yes and n for no)" << std::endl;
		std::cin >> f;
		if (f == 'y')
		{
			displayboard();
		}
		else
		{
			std::cout << "THANK YOU FOR PLAYING THE GAME, NOW GO REST! ";
		}
	}
	/*for (int i = 0; i < 15; i++)
	{
		delete[]mines[i];

	}
	delete[]mines;*/
	return 0;
}