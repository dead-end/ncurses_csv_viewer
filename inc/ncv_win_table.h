/*
 * ncv_win_table.h
 *
 */

#ifndef INC_NCV_WIN_TABLE_H_
#define INC_NCV_WIN_TABLE_H_

#include "ncv_table.h"
#include "ncv_table_part.h"
#include "ncv_cursor.h"

void win_table_init();

void win_table_resize();

void win_table_refresh_no();

void win_table_free();

void win_table_on_table_change(const s_table *table);

void win_table_content_resize(const s_table *table, s_cursor *cursor);

void win_table_set_cursor(const s_table *table, s_cursor *cursor);

bool win_table_process_input(const s_table *table, s_cursor *cursor, const int key_type, const wint_t chr);

void win_table_content_print(const s_table *table, const s_cursor *cursor);

WINDOW *win_table_get_win();

#endif /* INC_NCV_WIN_TABLE_H_ */
