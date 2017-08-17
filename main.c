#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>


WINDOW *create_newwin(int h, int w, int sy, int sx);
void destroy_win(WINDOW *local_win);
void keyboard(int c);
void setup();
void logic();
void draw();

WINDOW *field;
#define fieldwidth 42 // +2 for 20 units
#define fieldheight 17 // +2 for 10 units
int cells[fieldwidth][fieldwidth];

typedef struct cursor 
{
	int y;
	int x;
	char curs;
} cursors;

cursors cur;

int main(int argc, char *argv[])
{
	/*
	 * program entry point
	 * args:
	 * 	arg count, arg character array
	 * returns 0 (program success)
	 */
	initscr();
	cbreak();
	noecho();
	curs_set(0); /* hide cursor */
	keypad(stdscr, TRUE);
	setup();
	refresh();
	timeout(1);
	int c;
	while(1)
	{
		/* program loop */
		c = getch();
		keyboard(c);
		logic();
		draw();
		usleep(80000);
	}
	endwin();
	return 0;
}

void keyboard(int c)
{
	if (c == KEY_LEFT && cur.x >= 2)
		cur.x--;
	if (c == KEY_RIGHT && cur.x < fieldwidth - 2)
		 cur.x++;
	if (c == KEY_UP && cur.y >= 2)
		cur.y--;
	if (c == KEY_DOWN && cur.y < fieldheight - 2)
		cur.y++;
	if (c == ' ')
	{
		int nc;
		nc = (cells[cur.y][cur.x] == 0) ? 1 : 0; 
		cells[cur.y][cur.x] = nc;
	}
}

void setup()
{
	/*
	 * setup function to initialise arrays, rand() seed etc
	 * args:
	 * 	none
	 * returns nothing
	 */
	/* color pairs */
	start_color();
	init_pair(1, COLOR_BLACK, COLOR_RED);
	init_pair(2, COLOR_BLACK, COLOR_BLUE);
	init_pair(3, COLOR_BLACK, COLOR_GREEN);
	init_pair(4, COLOR_BLACK, COLOR_YELLOW);
	init_pair(5, COLOR_BLACK, COLOR_MAGENTA);
	init_pair(6, COLOR_BLACK, COLOR_CYAN);
	init_pair(7, COLOR_BLACK, COLOR_WHITE);


	/* seed random generator */
	srand(time(NULL));
//	squares[x].w = rand() % 10 + 1;
	cur.y = 1;
	cur.x = 1;
	cur.curs = ' ';
	field = create_newwin(fieldheight, fieldwidth, 0, 5);
	//wbkgd(field, COLOR_PAIR(3));
	

	/* random grid
	int i;
	int n;
	int j = 0;
	int prnt = 0;
	for (i = 0; i < fieldheight; i++)
	{
		for (n = 0; n < fieldwidth; n++)
		{
			j = rand() % 2;
			prnt = (j == 0) ? 0 : 1;
			cells[i][n] = prnt;
		}
	}
	*/
}

void logic()
{
	/*
	 * logic
	 */
}

void draw()
{
	/*
	 * draws our screen
	 */
	wclear(stdscr);
	mvprintw(fieldheight + 5, 0, "Arrow keys to move your cursor, space to flip the cell.\npress 'g' to start the simulation.");
	refresh();
	/* render here */
	//mvwin(field, 10, 10);
	//mvwprintw(field, 0, 0, "123456789012345678901234567890");
	int i;
	int n;
	wclear(field);
	for (i = 0; i < fieldheight; i++)
	{
		for (n = 0; n < fieldwidth; n++)
		{
			if (cells[i][n] == 1)
			{
				wattron(field, A_REVERSE);
				mvwprintw(field, i, n, " ", cells[i][n]);
				wattroff(field, A_REVERSE);
			}
		}
	}
	wattron(field, COLOR_PAIR(3));
	mvwaddch(field, cur.y, cur.x, cur.curs);
	wattroff(field, COLOR_PAIR(3));
	box(field, 0, 0);
	wrefresh(field);
}

WINDOW *create_newwin(int h, int w, int sy, int sx)
{
	/* 
	 * create a new window
	 */
	WINDOW *local_win;
	local_win = newwin(h, w, sy, sx);
	return local_win;
}

void destroy_win(WINDOW *local_win)
{
	/*
	 * destroys a given window
	 * args:
	 * 	pointer to the window to be destroyed
	 */
	wborder(local_win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
	wrefresh(local_win);
	delwin(local_win);
}
