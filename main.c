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
int neighbor_count(int y, int x);

WINDOW *field;
#define fieldwidth 42 // +2 for 20 units
#define fieldheight 17 // +2 for 10 units
int cells[fieldheight][fieldwidth];
int tmpcells[fieldheight][fieldwidth];
int ccolor;

typedef struct cursor 
{
	int y;
	int x;
	char curs;
} cursors;

cursors cur;

int should_run;
int single_shot;

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
		if (should_run || single_shot)
			logic();
		if (single_shot == 1) {single_shot = 0; should_run = 0;};
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
	if (c == 'g')
	{
		should_run = (should_run == 0) ? 1 : 0;
	}
	if (c == 's')
	{
		single_shot = 1;
	}
	if (c == 'c')
	{
		int i, n;
		for (i = 0; i < fieldheight; i++)
		{
			for (n = 0; n < fieldwidth; n++)
			{
				cells[i][n] = 0;
				tmpcells[i][n] = 0;
			}
		}
	}
	if (c == 'v')
	{
		if (ccolor >= 8)
			ccolor = 1;
		else
			ccolor++;
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
	init_pair(4, COLOR_BLACK, COLOR_BLACK);
	init_pair(5, COLOR_BLACK, COLOR_MAGENTA);
	init_pair(6, COLOR_BLACK, COLOR_CYAN);
	init_pair(7, COLOR_BLACK, COLOR_WHITE);
	init_pair(8, COLOR_BLACK, COLOR_YELLOW);

	/* seed random generator */
	srand(time(NULL));
	ccolor = 7;
	cur.y = 1;
	cur.x = 1;
	cur.curs = ' ';
	field = create_newwin(fieldheight, fieldwidth, 0, 5);
	should_run = 0;
	single_shot = 0;
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
	//cells[5][5] = neighbor_count(5, 5);
	int i;
	int n;
	int count;
	//int should_be_dead = 0;

	for (i = 0; i < fieldheight; i++)
	{
		for (n = 0; n < fieldwidth; n++)
		{
			count = neighbor_count(i, n);	
			/* any live cell with < 2 neighbors dies */
			if (cells[i][n] == 1 && count < 2)
				tmpcells[i][n] = 0;
			/* any live cell with > 3 neighbors dies */
			if (cells[i][n] == 1 && count > 3)
				tmpcells[i][n] = 0;
			/* any live cell with 2 or 3 neighbors lives */
			if (cells[i][n] == 1 && count == 2 || cells[i][n] == 1 && count == 3)
				tmpcells[i][n] = 1;
			/* any dead cell with 3 live neighbors becomes alive */
			if (cells[i][n] == 0 && count == 3)
				tmpcells[i][n] = 1;

			//cells[i][n] = (should_be_dead == 1) ? 0 : 1;
		}
	}
	for (i = 0; i < fieldheight; i++)
	{
		for (n = 0; n < fieldwidth; n++)
		{
			cells[i][n] = tmpcells[i][n];
		}
	}
	usleep(90000);
}

int neighbor_count(int y, int x)
{
	int count = 0; // change to 0
	/*
	 * we need to check 8 total cells
	 * top left (y=y-1 x=x-1), top middle (y=y-1, x=x), top right (y=y-1, x=x+1)
	 * middle left (y=y, x=x-1),  middle right (y=y, x=x+1)
	 * bottom left(y+y+1, x=x-1), bottom middley=y+1, x=x), bottom right(y=y+1. x=x+1)
	 */
	if (y - 1 > 0 && x - 1 > 0)
		count += (cells[y - 1][x - 1] == 1) ? 1 : 0;
	if (y - 1 > 0)
		count += (cells[y-1][x] == 1) ? 1 : 0;
	if (y - 1 > 0 && x + 1 < fieldwidth)
		count += (cells[y-1][x+1] == 1) ? 1 : 0;

	if (x - 1 > 0)
		count += (cells[y][x-1] == 1) ? 1 : 0;
	if (x + 1 < fieldwidth)
		count += (cells[y][x+1] == 1) ? 1 : 0;

	if (y + 1 < fieldheight && x - 1 > 0)
		count += (cells[y+1][x-1] == 1) ? 1 : 0;
	if (y + 1 < fieldheight)
		count += (cells[y+1][x] == 1) ? 1 : 0;
	if (y + 1 < fieldheight && x + 1 < fieldwidth)
		count += (cells[y+1][x+1] == 1) ? 1 : 0;
	return count;
}

void draw()
{
	/*
	 * draws our screen
	 */
	wclear(stdscr);
	mvprintw(fieldheight + 4, 0, "arrow keys to move your cursor, space to flip the cell.\npress 'g' to start the simulation.\n's' to step one generation.\n'c' to clear the field.\n'v' to change the cell color.");
	if (should_run == 1)
	{
		wattron(stdscr, COLOR_PAIR(3));
		mvprintw(fieldheight + 5 - 3, 0, "simulation running.");
		wattroff(stdscr, COLOR_PAIR(3));
	}
	else
	{
		wattron(stdscr, COLOR_PAIR(1));
		mvprintw(fieldheight + 5 - 3, 0, "simulation not running.");
		wattroff(stdscr, COLOR_PAIR(1));
	}

	mvprintw(fieldheight + 5 - 4, 0, "%d neighbors", neighbor_count(cur.y, cur.x));
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
				//wattron(field, A_REVERSE);
				wattron(field, COLOR_PAIR(ccolor));
				mvwprintw(field, i, n, " ", cells[i][n]);
				wattroff(field, COLOR_PAIR(ccolor));
				//wattroff(field, A_REVERSE);
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
