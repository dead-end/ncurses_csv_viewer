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

#include "ncv_table.h"
#include "ncv_field.h"

/***************************************************************************
 * The function checks the s_field_part_update function, which computes the
 * visible part of a truncated field.
 **************************************************************************/

static void test_field_part_update() {

	s_table_part table_part;
	s_field_part field_part;

	print_debug_str("test_field_part_update() Start\n");

	//
	// truncated right
	//
	table_part.first = 0;
	table_part.last = 2;
	table_part.truncated = table_part.last;
	table_part.size = 2;

	s_field_part_update(&table_part, 0, 4, &field_part);
	ut_check_int(field_part.start, 0, "test 1 - start");
	ut_check_int(field_part.size, 4, "test 2 - size");

	s_field_part_update(&table_part, 2, 4, &field_part);
	ut_check_int(field_part.start, 0, "test 3 - start");
	ut_check_int(field_part.size, 2, "test 4 - size");

	//
	// truncated left
	//
	table_part.first = 0;
	table_part.last = 2;
	table_part.truncated = table_part.first;
	table_part.size = 2;

	s_field_part_update(&table_part, 0, 4, &field_part);
	ut_check_int(field_part.start, 2, "test 5 - start");
	ut_check_int(field_part.size, 2, "test 6 - size");

	s_field_part_update(&table_part, 2, 4, &field_part);
	ut_check_int(field_part.start, 0, "test 7 - start");
	ut_check_int(field_part.size, 4, "test 8 - size");

	print_debug_str("test_field_part_update() End\n");
}

/***************************************************************************
 * The function checks the get_field_line function for a truncated field. It
 * is repeatedly called and returns a truncated and padded line of a fixed
 * width.
 **************************************************************************/

#define FIELD_SIZE 2

static void test_get_field_line() {

	print_debug_str("test_get_field_line() Start\n");

	s_field_part col_field_part;
	wchar_t buffer[FIELD_SIZE + 1];
	wchar_t *ptr;

	//
	// The string with the field content (height 6 width 5)
	//
	wchar_t *str1 = L"\nз\nза\nзая\nзаяц";


	ptr = str1;

	//
	// Field is truncated right.
	//
	col_field_part.start = 0;
	col_field_part.size = FIELD_SIZE;
	buffer[col_field_part.size] = W_STR_TERM;

	ptr = get_field_line(ptr, buffer, &col_field_part);
	ut_check_wchar_str(buffer, L"  ");

	ptr = get_field_line(ptr, buffer, &col_field_part);
	ut_check_wchar_str(buffer, L"з ");

	ptr = get_field_line(ptr, buffer, &col_field_part);
	ut_check_wchar_str(buffer, L"за");

	ptr = get_field_line(ptr, buffer, &col_field_part);
	ut_check_wchar_str(buffer, L"за");

	ptr = get_field_line(ptr, buffer, &col_field_part);
	ut_check_wchar_str(buffer, L"за");

	ut_check_wchar_null(ptr);

	//
	// Field is truncated left.
	//
	col_field_part.start = 2;
	col_field_part.size = FIELD_SIZE;

	ptr = str1;

	ptr = get_field_line(ptr, buffer, &col_field_part);
	ut_check_wchar_str(buffer, L"  ");

	ptr = get_field_line(ptr, buffer, &col_field_part);
	ut_check_wchar_str(buffer, L"  ");

	ptr = get_field_line(ptr, buffer, &col_field_part);
	ut_check_wchar_str(buffer, L"  ");

	ptr = get_field_line(ptr, buffer, &col_field_part);
	ut_check_wchar_str(buffer, L"я ");

	ptr = get_field_line(ptr, buffer, &col_field_part);
	ut_check_wchar_str(buffer, L"яц");

	ut_check_wchar_null(ptr);

	//
	// Test an empty field
	//
	wchar_t *str2 = L"";
	ptr = str2;

	col_field_part.start = 0;
	col_field_part.size = FIELD_SIZE;

	ptr = get_field_line(ptr, buffer, &col_field_part);
	ut_check_wchar_str(buffer, L"  ");

	ut_check_wchar_null(ptr);

	print_debug_str("test_get_field_line() End\n");
}

/***************************************************************************
 * The main function simply starts the test.
 **************************************************************************/

int main() {

	print_debug_str("ut_field.c - Start tests\n");

	setlocale(LC_ALL, "");

	test_field_part_update();

	test_get_field_line();

	print_debug_str("ut_field.c - End tests\n");

	return EXIT_SUCCESS;
}
