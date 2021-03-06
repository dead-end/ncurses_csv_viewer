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

#include "ut_utils.h"
#include "ncv_wbuf.h"

#include <locale.h>

/******************************************************************************
 * The function creates a s_wbuf from a wchar_t string and checks the result.
 *****************************************************************************/

static void ut_check_s_wbuf(const int init_block_size, const wchar_t *str, const int num_blocks, const int end_idx) {

	log_debug("Checking: %ls", str);

	//
	// Create the s_wbuf from the string with an initial block size.
	//
	s_wbuf *wbuf = s_wbuf_create(init_block_size);
	s_wbuf_add_str(wbuf, str);

	//
	// Ensure that the blocks and the end index is correct.
	//
	ut_check_int(wbuf->end_pos.block->idx + 1, num_blocks, "Check num blocks");
	ut_check_int(wbuf->end_pos.idx, end_idx, "Check end index");

	//
	// Iterate through the s_wbuf.
	//
	s_wbuf_pos cur_pos;
	s_wbuf_pos_init(&cur_pos);

	wchar_t wchr;

	for (const wchar_t *ptr = str; *ptr != L'\0'; ptr++) {

		//
		// Get the next wchar_t and ensure that we are not at the end of the
		// buffer.
		//
		ut_check_bool(true, s_wbuf_next(wbuf, &cur_pos, &wchr));

		//
		// Ensure that the wchar_t is correct.
		//
		ut_check_wchr(wchr, *ptr);
	}

	//
	// Ensure that the next wchar_t is NULL to indicate the end of the buffer.
	//
	ut_check_bool(false, s_wbuf_next(wbuf, &cur_pos, &wchr));

	//
	// The buffer has to be freed.
	//
	s_wbuf_free(wbuf);

	log_debug_str("End");
}

/******************************************************************************
 * The function checks s_wbuf functions. Remember that the block size is
 * doubled on every new block.
 *****************************************************************************/

static void test_s_wbuf() {

	log_debug_str("Start");

	//
	// 1111 22222222 <- blocks
	// 0123 012345   <- string / block index
	//
	ut_check_s_wbuf(4, L"0123012345", 2, 5);

	//
	// 1111 22222222 <- blocks
	// 0123 01234567 <- string / block index
	//
	ut_check_s_wbuf(4, L"012301234567", 2, 7);

	//
	// 1111 22222222 3333333333333333 <- blocks
	// 0123 01234567 0                <- string / block index
	//
	ut_check_s_wbuf(4, L"0123012345670", 3, 0);

	log_debug_str("End");
}

/******************************************************************************
 * The main function simply starts the test.
 *****************************************************************************/

int main() {

	log_debug_str("Start");

	//
	// Use the default locale ('C' or 'POSIX'). See:
	// https://www.gnu.org/software/libc/manual/html_node/Choosing-Locale.html
	//
	setlocale(LC_ALL, "C");

	test_s_wbuf();

	log_debug_str("End");

	return EXIT_SUCCESS;
}
