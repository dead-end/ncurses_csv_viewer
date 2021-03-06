/*
 * MIT License
 *
 * Copyright (c) 2018 dead-end
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef INCLUDE_COMMON_H_
#define INCLUDE_COMMON_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define VERSION "0.3.0"

#define VERISON_SHORT "0.3"

/******************************************************************************
 * Definition of the print_debug macro. It is only defined if the DEBUG flag is
 * defined. It prints to stderr not to restrain curses.
 *****************************************************************************/

#ifdef DEBUG

#define log_debug(fmt, ...) fprintf(stderr, "DEBUG %s:%d:%s() " fmt "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#define log_debug_str(fmt)  fprintf(stderr, "DEBUG %s:%d:%s() " fmt "\n", __FILE__, __LINE__, __func__)

#define DEBUG_USED

#else

#define log_debug(fmt, ...)
#define log_debug_str(fmt)

#define DEBUG_USED __attribute__((unused))

#endif

/******************************************************************************
 * Definition of the print_error macro, that finishes the program after
 * printing the error message.
 *****************************************************************************/

#define log_exit(fmt, ...) fprintf(stderr, "FATAL %s:%d:%s() " fmt "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__); exit(EXIT_FAILURE)
#define log_exit_str(fmt)  fprintf(stderr, "FATAL %s:%d:%s() " fmt "\n", __FILE__, __LINE__, __func__); exit(EXIT_FAILURE)

/******************************************************************************
 * Simple macro to print readable bool values.
 *****************************************************************************/

#define bool_2_str(b) (b) ? "true" : "false"

/******************************************************************************
 * Buffer and line sizes.
 *****************************************************************************/

#define MAX_LINE 1024

#define BUF_SIZE 4096

/******************************************************************************
 * Some wchar character constants.
 *****************************************************************************/

#define W_NEW_LINE L'\n'
#define W_STR_TERM L'\0'
#define W_QUOTE    L'\"'
#define W_CR       L'\r'
#define W_TAB      L'\t'
#define W_DELIM    L','
#define W_SPACE    L' '

#define NL         "\n"
#define CR         "\r"
#define DL         ","
#define EC         "\""

/******************************************************************************
 * An enum value for a direction (example: direction * width)
 *****************************************************************************/

enum e_direction {
	E_DIR_BACKWARD = -1, E_DIR_FORWARD = 1
};

#define e_direction_str(d) (d) == E_DIR_FORWARD ? "FORWARD" : "BACKWARD"

/******************************************************************************
 * A boolean type that can be undefined.
 *****************************************************************************/

enum bool_defined {
	BOOL_TRUE, BOOL_FLASE, BOOL_UNDEF
};

/******************************************************************************
 * The struct defines a field with rows and columns.
 *****************************************************************************/

typedef struct s_field {

	//
	// The index of the fields row.
	//
	int row;

	//
	// The index of the fields column.
	//
	int col;

} s_field;

/******************************************************************************
 * The struct defines a buffer of wchar_t. If is used to define a substring of
 * a wchar_t string. The string may be larger than len, so the substring is not
 * necessary \0 terminated.
 *****************************************************************************/

typedef struct s_buffer {

	//
	// A pointer to the buffer, which defines the start of the substring.
	//
	wchar_t *ptr;

	//
	// The length of the buffer which can be other than wcslen(ptr).
	//
	size_t len;

} s_buffer;

/******************************************************************************
 * The macros simply get or set s_buffer values. The make the expressions
 * easier to read, because there name expresses the meaning of the expression.
 *****************************************************************************/

#define s_buffer_start(b) ((b)->ptr)

#define s_buffer_end(b) ((b)->ptr + (b)->len)

#define s_buffer_set(b,p,l) (b)->ptr = (p) ; (b)->len = (l)

/******************************************************************************
 * The two macros return min / max or equal values.
 *****************************************************************************/

#define min_or_equal(a,b) ((a) <= (b) ? (a) : (b))

#define max_or_equal(a,b) ((a) >= (b) ? (a) : (b))

/******************************************************************************
 * A function definition for alignments.
 *****************************************************************************/

enum e_align {
	AT_LEFT, AT_RIGHT, AT_CENTER
};

int get_align_start(const int max, const int len, const enum e_align align);

/******************************************************************************
 * Compute the power of two.
 *****************************************************************************/

#define pow2(a) (a) * (a)

#define bool_str(b) (b) ? "true" : "false"

//
// chtype cht = (chtype) (unsigned char) chr;
//
#define char_to_chtype(ch) ((chtype) (unsigned char) (ch))

/******************************************************************************
 * Various function definitions.
 *****************************************************************************/

void* xmalloc(const size_t size);

size_t mbs_2_wchars(const char *mbs, wchar_t *buffer, const int buf_size);

char* trim(char *str);

wchar_t* wcstrim(wchar_t *str);

wchar_t read_wchar(FILE *file);

wchar_t* wcs_casestr(const wchar_t *s, const wchar_t *find);

bool wcs_is_empty(const wchar_t *str);

void str_array_sizes(const char *msgs[], int *rows, int *cols);

#endif
