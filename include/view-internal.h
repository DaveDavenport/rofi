#ifndef ROFI_VIEW_INTERNAL_H
#define ROFI_VIEW_INTERNAL_H
#include "widget.h"
#include "textbox.h"
#include "scrollbar.h"
#include "separator.h"
#include "box.h"
#include "keyb.h"
#include "x11-helper.h"

/**
 * @ingroup ViewHandle
 *
 * @{
 */
// State of the menu.

struct RofiViewState
{
    Mode             *sw;
    unsigned int     menu_lines;
    unsigned int     max_elements;
    unsigned int     max_rows;
    unsigned int     columns;

    unsigned int     element_width;

    // Update/Refilter list.
    int              update;
    int              refilter;
    int              rchanged;
    unsigned int     cur_page;


    Box              *main_box;
    // Entries
    Box              *input_bar;
    separator        *input_bar_separator;

    textbox          *text;
    textbox          *case_indicator;

    Box              *list_bar;
    Widget           *list_place_holder;
    textbox          **boxes;
    scrollbar        *scrollbar;
    // Small overlay.
    textbox          *overlay;
    int              *distance;
    unsigned int     *line_map;

    unsigned int     num_lines;

    // Selected element.
    unsigned int     selected;
    unsigned int     filtered_lines;
    // Last offset in paginating.
    unsigned int     last_offset;

    KeyBindingAction prev_action;
    xcb_timestamp_t  last_button_press;

    int              quit;
    int              skip_absorb;
    // Return state
    unsigned int     selected_line;
    MenuReturn       retv;
    int              line_height;
    unsigned int     border;
    workarea         mon;

    // Sidebar view
    Box              *sidebar_bar;
    unsigned int     num_modi;
    textbox          **modi;

    MenuFlags        menu_flags;
    int              mouse_seen;

    int              reload;
    // Handlers.
    void             ( *x11_event_loop )( struct RofiViewState *state, xcb_generic_event_t *ev, xkb_stuff *xkb );
    void             ( *finalize )( struct RofiViewState *state );

    int              width;
    int              height;
    int              x;
    int              y;
};
/** @} */
#endif
