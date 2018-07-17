/*
 * file: ncv_ncurses.c
 */

#include "ncv_common.h"
#include "ncv_ncurses.h"

//
// The color mode configuration. On true colors are used if the terminal
// supports colors. On false monochrome mode is used.
//
static bool use_colors;

/***************************************************************************
 * The two static variables are used to be able to switch off attribute
 * after they are switched on.
 **************************************************************************/

// TODO: not a good idea
static int attr_last;

static bool attr_unset;

/***************************************************************************
 * The function switches on attributes and saves them in a static variable,
 * to be able to switch the off later.
 **************************************************************************/

//TODO: win
void ncurses_set_attr(WINDOW *win, const int attr) {

	wattron(win, attr);
	attr_last = attr;
	attr_unset = true;
}

/***************************************************************************
 * The function switches off the attributes saved in the static variable.
 **************************************************************************/
//TODO: win
void ncurses_unset_attr(WINDOW *win) {

	if (attr_unset) {
		wattroff(win, attr_last);
		attr_unset = false;
	}
}

/***************************************************************************
 * The function creates a window. It is ensured that the window has a valid
 * size and fits in its parent (which is: stdscr).
 **************************************************************************/

WINDOW *ncurses_win_create(const int rows, const int cols, const int begin_y, const int begin_x) {
	WINDOW *win;

	print_debug("ncurses_win_create() rows: %d cols: % d begin y: %d x: %d\n", rows, cols, begin_y, begin_x);

	//
	// Ensure that the minimum size of a window is requested.
	//
	if (rows < 1 || cols < 1) {
		print_exit("ncurses_win_create() Invalid dimension (rows: %d cols: % d)! Ensure that the terminal is large enough.\n", rows, cols);
	}

	//
	// Ensure that the window fits in its parent window.
	//
	if (rows + begin_y > getmaxy(stdscr) || cols + begin_x > getmaxx(stdscr)) {
		print_exit_str("ncurses_win_create() Window does not fit in parent!\n");
	}

	//
	// Create the window.
	//
	if ((win = newwin(rows, cols, begin_y, begin_x)) == NULL) {
		print_exit_str("ncurses_win_create() Unable to create header win!\n");
	}

	return win;
}

/***************************************************************************
 * The function moves a given window to the new position, if its position
 * has changed.
 **************************************************************************/

void ncurses_win_move(WINDOW *win, const int to_y, const int to_x) {
	int from_y, from_x;

	//
	// Ensure that the target is valid.
	//
	if (to_y < 0 || to_x < 0) {
		print_exit("ncurses_win_move() Win position is not valid - y: %d x: %d\n", to_y, to_x);
	}

	//
	// Check whether the position changed.
	//
	getbegyx(win, from_y, from_x);

	if (from_y == to_y && from_x == to_x) {
		print_debug("ncurses_win_move() Win position has not changed - y: %d x: %d\n", from_y, from_x);
		return;
	}

	print_debug("ncurses_win_move() Moving win from y: %d x: %d to y: %d x: %d\n", from_y, from_x, to_y, to_x);

	//
	// Do the actual moving.
	//
	if (mvwin(win, to_y, to_x) != OK) {
		print_debug("ncurses_win_move() stdscr max y: %d x: %d\n", getmaxy(stdscr), getmaxx(stdscr));
		print_debug("ncurses_win_move() win max y: %d x: %d\n", getmaxy(win), getmaxx(win));
		print_exit_str("ncurses_win_move() Unable to move window\n");
	}
}

/***************************************************************************
 * The function resizes a window. On success it returns true. It returns
 * false if the size does not changed.
 **************************************************************************/

bool ncurses_win_resize(WINDOW *win, const int to_y, const int to_x) {
	int from_y, from_x;
	int result;

	//
	// Ensure that the target is valid.
	//
	if (to_y < 1 || to_x < 1) {
		print_exit("ncurses_win_resize() Win size is not valid (y: %d x: %d)\n", to_y, to_x);
	}

	//
	// Check whether the position changed.
	//
	getmaxyx(win, from_y, from_x);

	print_debug("ncurses_win_resize() Win size from y: %d x: %d to y: %d x: %d\n", from_y, from_x, to_y, to_x);

	if (from_y == to_y && from_x == to_x) {
		print_debug_str("ncurses_win_resize() Win size has not changed.\n");
		return false;
	}

	//
	// Do the actual resizing.
	//
	if ((result = wresize(win, to_y, to_x)) != OK) {
		print_exit("ncurses_win_resize() Unable to resize window (result: %d)\n", result);
	}

	return true;
}

/***************************************************************************
 * The function is called with two attributes one for color mode and an
 * alternative attribute if monochrome mode is configured or the terminal
 * does not support colors.
 **************************************************************************/

chtype ncurses_attr_color(const chtype color, const chtype alt) {

	if (use_colors && has_colors()) {
		return color;
	} else {
		return alt;
	}
}

/***************************************************************************
 * The function sets the background of a window depending on whether the
 * terminal has colors or not.
 **************************************************************************/

void ncurses_attr_back(WINDOW *win, const chtype color, const chtype alt) {

	if (wbkgd(win, ncurses_attr_color(color, alt)) != OK) {
		print_exit_str("ncurses_win_back() Unable to set background for window!\n");
	}
}

/***************************************************************************
 * The initializes colors if the terminal supports colors.
 **************************************************************************/

static void ncurses_init_colors() {

	if (!has_colors()) {
		print_debug_str("ncurses_init_colors() Terminal has no colors!\n");
		return;
	}

	if (start_color() != OK) {
		print_exit_str("ncurses_init() Unable to init colors!\n");
	}

	if (init_pair(CP_TABLE, COLOR_WHITE, COLOR_BLUE) != OK) {
		print_exit_str("ncurses_init() Unable to init color pair!\n");
	}

	if (init_pair(CP_TABLE_HEADER, COLOR_YELLOW, COLOR_BLUE) != OK) {
		print_exit_str("ncurses_init() Unable to init color pair!\n");
	}

	if (init_pair(CP_CURSOR, COLOR_WHITE, COLOR_CYAN) != OK) {
		print_exit_str("ncurses_init() Unable to init color pair!\n");
	}

	if (init_pair(CP_CURSOR_HEADER, COLOR_YELLOW, COLOR_CYAN) != OK) {
		print_exit_str("ncurses_init() Unable to init color pair!\n");
	}

	if (init_pair(CP_STATUS, COLOR_BLACK, COLOR_WHITE) != OK) {
		print_exit_str("ncurses_init() Unable to init color pair!\n");
	}

	if (init_pair(CP_FIELD, COLOR_YELLOW, COLOR_BLACK) != OK) {
		print_exit_str("ncurses_init() Unable to init color pair!\n");
	}
}

/***************************************************************************
 * The function initializes the ncurses.
 **************************************************************************/

void ncurses_init(const bool monochrom) {

	use_colors = !monochrom;

	//
	// Start ncurses.
	//
	if (initscr() == NULL) {
		print_exit_str("ncurses_init() Unable to init screen!\n");
	}

	//
	// Allow KEY_RESIZE to be read on SIGWINCH
	//
	keypad(stdscr, TRUE);

	//
	// Switch off cursor by default
	//
	curs_set(0);

	//
	// Switch off echoing until the filter chars are inputed.
	//
	noecho();

	print_debug("ncurses_init() win - y: %d x: %d\n", getmaxy(stdscr), getmaxx(stdscr));

	//
	// Initialize the colors
	//
	ncurses_init_colors();
}
