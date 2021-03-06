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

#include "ncv_forms.h"
#include "ncv_popup.h"
#include "ncv_common.h"

/******************************************************************************
 * The function set some default values.
 *****************************************************************************/

void popup_init(s_popup *popup) {

	//
	// Init form
	//
	popup->form = NULL;

	//
	// Init menu
	//
	popup->menu = NULL;

	//
	// Start on the form.
	//
	popup->is_on_form = true;
}

/******************************************************************************
 * The function posts the form and the menu of the popup.
 *****************************************************************************/

void popup_post(const s_popup *popup) {
	int result;

	//
	// Post the form. (E_NO_ROOM is returned if the window is too small)
	//
	if ((result = post_form(popup->form)) != E_OK) {
		log_exit("Unable to post popup form! (result: %d)", result);
	}

	//
	// Post the menu. (E_NO_ROOM is returned if the window is too small)
	//
	if ((result = post_menu(popup->menu)) != E_OK) {
		log_exit("Unable to post popup menu! (result: %d)", result);
	}
}

/******************************************************************************
 * The function un-posts the form and the menu of the popup. The un-posting is
 * used for the resizing of the window.
 *****************************************************************************/

void popup_unpost(const s_popup *popup) {

	if (unpost_form(popup->form) != E_OK) {
		log_exit_str("Unable to unpost popup form!");
	}

	if (unpost_menu(popup->menu) != E_OK) {
		log_exit_str("Unable to unpost popup menu!");
	}
}

/******************************************************************************
 * The function sets the cursor to the field in the form or an item in the
 * menu.
 *****************************************************************************/

void popup_pos_cursor(s_popup *popup) {

	if (popup->is_on_form) {

		if (pos_form_cursor(popup->form) != E_OK) {
			log_exit_str("Unable to set the form cursor!");
		}
	} else {

		if (pos_menu_cursor(popup->menu) != E_OK) {
			log_exit_str("Unable to set the menu cursor!");
		}
	}
}

/******************************************************************************
 * The function switches the popup state to the menu, which means the first
 * item of the menu.
 *****************************************************************************/

static void popup_switch_to_menu(s_popup *popup) {

	//
	// Check if the popup state is already on the menu.
	//
	if (!popup->is_on_form) {
		log_debug_str("Popup is already on menu!");
		return;
	}

	//
	// Switch on menu and cursor
	//
	menus_switch_on_off(popup->menu, true);

	//
	// Always move to the first item
	//
	menus_driver(popup->menu, REQ_FIRST_ITEM);
	popup->is_on_form = false;

	log_debug_str("Switch to menu.");
}

/******************************************************************************
 * The function switches the popup state of the form. This means the first or
 * the last field of the form. The requested item is given as a parameter.
 * Valid values are: REQ_FIRST_FIELD and REQ_LAST_FIELD
 *****************************************************************************/

static void popup_switch_to_form(s_popup *popup, const wint_t req_first_last) {

#ifdef DEBUG

	//
	// Ensure the valid values of req_first_last
	//
	if (req_first_last != REQ_FIRST_FIELD && req_first_last != REQ_LAST_FIELD) {
		log_exit("Invalid req_first_last: %lc", req_first_last);
	}
#endif

	//
	// Check if the popup state is already on the form.
	//
	if (popup->is_on_form) {
		log_debug_str("Popup is already on form!");
		return;
	}

	//
	// Switch off menu and cursor
	//
	menus_switch_on_off(popup->menu, false);

	//
	// Move to the first or last field
	//
	forms_driver(popup->form, KEY_CODE_YES, req_first_last);
	forms_driver(popup->form, KEY_CODE_YES, REQ_END_FIELD);
	popup->is_on_form = true;

	log_debug_str("Switch to form.");
}

/******************************************************************************
 * The function is called to show the popup. This means the first field of the
 * form is activated. This ensures that, for example the cursor is correctly
 * set.
 *****************************************************************************/

void popup_prepair_show(s_popup *popup) {
	popup_switch_to_form(popup, REQ_FIRST_FIELD);
}

/******************************************************************************
 * The function requests the prev or next field of the form. This can lead to
 * a switch to the menu.
 *****************************************************************************/

static void popup_req_prev_next_field(s_popup *popup, const wint_t req_prev_next) {

#ifdef DEBUG

	//
	// Ensure the valid values of req_prev_next
	//
	if (req_prev_next != REQ_NEXT_FIELD && req_prev_next != REQ_PREV_FIELD) {
		log_exit("Invalid prev / next value: %lc", req_prev_next);
	}
#endif

	//
	// Request next field and currently last field
	//
	if (req_prev_next == REQ_NEXT_FIELD && forms_is_last(popup->form)) {
		popup_switch_to_menu(popup);
		return;
	}

	//
	// Request prev field and currently first field
	//
	if (req_prev_next == REQ_PREV_FIELD && forms_is_first(popup->form)) {
		popup_switch_to_menu(popup);
		return;
	}

	//
	// Move to the requested field and move the cursor to the end of the field.
	//
	forms_driver(popup->form, KEY_CODE_YES, req_prev_next);
	forms_driver(popup->form, KEY_CODE_YES, REQ_END_FIELD);
}

/******************************************************************************
 * The function does the core input processing of the menu. This is the
 * processing of UP, DOWN and TAB keys. LEFT and RIGHT keys are processed for
 * the menu. It returns true if the input was processed. If not, the processing
 * has to be delegated to an other input processing function.
 *****************************************************************************/

bool popup_process_input(s_popup *popup, const int key_type, const wint_t chr) {

	switch (key_type) {

	case KEY_CODE_YES:

		switch (chr) {

		case KEY_DOWN:

			if (popup->is_on_form) {
				popup_req_prev_next_field(popup, REQ_NEXT_FIELD);

			} else {
				popup_switch_to_form(popup, REQ_FIRST_FIELD);
			}
			return true;

		case KEY_UP:

			if (popup->is_on_form) {
				popup_req_prev_next_field(popup, REQ_PREV_FIELD);

			} else {
				popup_switch_to_form(popup, REQ_LAST_FIELD);
			}
			return true;

		case KEY_LEFT:

			//
			// LEFT key is only processed for menu.
			//
			if (!popup->is_on_form) {

				if (menus_is_first(popup->menu)) {
					menus_driver(popup->menu, REQ_LAST_ITEM);

				} else {
					menus_driver(popup->menu, REQ_LEFT_ITEM);
				}
				return true;
			}
			break;

		case KEY_RIGHT:

			//
			// RIGHT key is only processed for menu.
			//
			if (!popup->is_on_form) {

				if (menus_is_last(popup->menu)) {
					menus_driver(popup->menu, REQ_FIRST_ITEM);

				} else {
					menus_driver(popup->menu, REQ_RIGHT_ITEM);
				}
				return true;
			}

			break;
		}

		break; // case KEY_CODE_YES

	case OK:

		switch (chr) {

		case W_TAB:

			if (popup->is_on_form) {
				popup_req_prev_next_field(popup, REQ_NEXT_FIELD);

			} else {

				if (menus_is_last(popup->menu)) {
					popup_switch_to_form(popup, REQ_FIRST_FIELD);

				} else {
					menus_driver(popup->menu, REQ_RIGHT_ITEM);
				}
			}
			return true;
		}

		break; // case OK
	}

	return false;
}
