/**
 *   MIT/X11 License
 *   Modified  (c) 2016 Qball Cow <qball@gmpclient.org>
 *
 *   Permission is hereby granted, free of charge, to any person obtaining
 *   a copy of this software and associated documentation files (the
 *   "Software"), to deal in the Software without restriction, including
 *   without limitation the rights to use, copy, modify, merge, publish,
 *   distribute, sublicense, and/or sell copies of the Software, and to
 *   permit persons to whom the Software is furnished to do so, subject to
 *   the following conditions:
 *
 *   The above copyright notice and this permission notice shall be
 *   included in all copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 *   OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *   IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 *   CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 *   TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 *   SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <config.h>
#include <settings.h>
#include <glib.h>
#include <widgets/widget.h>
#include <widgets/textbox.h>
#include <widgets/listview.h>
#include <widgets/scrollbar.h>

struct _listview
{
    widget                   widget;
    // Administration

    unsigned int             cur_page;
    unsigned int             last_offset;
    unsigned int             selected;

    unsigned int             element_height;
    unsigned int             element_width;
    unsigned int             max_rows;
    unsigned int             max_elements;

    //
    unsigned int             cur_columns;
    unsigned int             req_elements;
    unsigned int             cur_elements;

    textbox                  **boxes;
    scrollbar                *scrollbar;

    listview_update_callback callback;
    void                     *udata;
};

static void listview_free ( widget *widget )
{
    listview *lv = (listview *) widget;
    g_free ( lv );
}
static unsigned int scroll_per_page ( listview * lv )
{
    int offset = 0;

    // selected row is always visible.
    // If selected is visible do not scroll.
    if ( ( ( lv->selected - ( lv->last_offset ) ) < ( lv->max_elements ) ) && ( lv->selected >= ( lv->last_offset ) ) ) {
        offset = lv->last_offset;
    }
    else{
        // Do paginating
        unsigned int page = ( lv->max_elements > 0 ) ? ( lv->selected / lv->max_elements ) : 0;
        offset          = page * lv->max_elements;
        lv->last_offset = offset;
        if ( page != lv->cur_page ) {
            lv->cur_page = page;
        }
        // Set the position
        scrollbar_set_handle ( lv->scrollbar, page * lv->max_elements );
    }
    return offset;
}

static unsigned int scroll_continious ( listview *lv )
{
    unsigned int middle = ( lv->max_rows - ( ( lv->max_rows & 1 ) == 0 ) ) / 2;
    unsigned int offset = 0;
    if ( lv->selected > middle ) {
        if ( lv->selected < ( lv->req_elements - ( lv->max_rows - middle ) ) ) {
            offset = lv->selected - middle;
        }
        // Don't go below zero.
        else if ( lv->req_elements > lv->max_rows ) {
            offset = lv->req_elements - lv->max_rows;
        }
    }
    if ( offset != lv->cur_page ) {
        //       lv->rchanged = TRUE;
        scrollbar_set_handle ( lv->scrollbar, offset );
        lv->cur_page = offset;
    }
    return offset;
}

static void update_element ( listview *lv, unsigned int tb, unsigned int index )
{
    // Select drawing mode
    TextBoxFontType type = ( index & 1 ) == 0 ? NORMAL : ALT;
    type = ( index ) == lv->selected ? HIGHLIGHT : type;

    if ( lv->callback ) {
        lv->callback ( lv->boxes[tb], index, lv->udata, type );
    }
}

static void listview_draw ( widget *wid, cairo_t *draw )
{
    unsigned int offset = 0;
    listview     *lv    = (listview *) wid;
    if ( config.scroll_method == 1 ) {
        offset = scroll_continious ( lv );
    }
    else {
        offset = scroll_per_page ( lv );
    }
    if ( lv->cur_elements > 0 && lv->max_rows > 0 ) {
        cairo_save ( draw );
        // Set new x/y possition.
        cairo_translate ( draw, wid->x, wid->y );
        unsigned int max   = MIN ( lv->cur_elements, lv->req_elements - offset );
        unsigned int width = lv->widget.w - config.line_margin * ( lv->cur_columns - 1 );
        if ( widget_enabled ( WIDGET ( lv->scrollbar ) ) ) {
            width -= config.line_margin;
            width -= lv->scrollbar->widget.w;
        }

        unsigned int element_width = ( width ) / lv->cur_columns;
        for ( unsigned int i = 0; i < max; i++ ) {
            unsigned int ex = ( ( i ) / lv->max_rows ) * ( element_width + config.line_margin );
            unsigned int ey = ( ( i ) % lv->max_rows ) * ( lv->element_height + config.line_margin );
            textbox_moveresize ( lv->boxes[i], ex, ey, element_width, lv->element_height );

            update_element ( lv, i, i + offset );
            widget_draw ( WIDGET ( lv->boxes[i] ), draw );
        }
        widget_draw ( WIDGET ( lv->scrollbar ), draw );
        cairo_restore ( draw );
    }
}

static void listview_recompute_elements ( listview *lv )
{
    unsigned int newne = 0;
    if ( lv->max_rows == 0 ) {
        return;
    }
    if ( lv->req_elements < lv->max_elements ) {
        newne           = lv->req_elements;
        lv->cur_columns = ( lv->req_elements + ( lv->max_rows - 1 ) ) / lv->max_rows;
    }
    else {
        newne           = lv->max_elements;
        lv->cur_columns = config.menu_columns;
    }
    for ( unsigned int i = newne; i < lv->cur_elements; i++ ) {
        widget_free ( WIDGET ( lv->boxes[i] ) );
    }
    lv->boxes = g_realloc ( lv->boxes, newne * sizeof ( textbox* ) );
    if ( newne > 0   ) {
        for ( unsigned int i = lv->cur_elements; i < newne; i++ ) {
            lv->boxes[i] = textbox_create ( 0, 0, 0, 0, lv->element_height, NORMAL, "" );

            update_element ( lv, i, i );
        }
    }
    scrollbar_set_handle_length ( lv->scrollbar, lv->cur_columns * lv->max_rows );
    lv->cur_elements = newne;
}

void listview_set_num_elements ( listview *lv, unsigned int rows )
{
    lv->req_elements = rows;
    listview_set_selected ( lv, lv->selected );
    listview_recompute_elements ( lv );
    scrollbar_set_max_value ( lv->scrollbar, lv->req_elements );
    widget_queue_redraw ( WIDGET ( lv ) );
}

unsigned int listview_get_selected ( listview *lv )
{
    return lv->selected;
}

void listview_set_selected ( listview *lv, unsigned int selected )
{
    lv->selected = MAX ( 0, MIN ( selected, lv->req_elements - 1 ) );
    widget_queue_redraw ( WIDGET ( lv ) );
}

static void listview_resize ( widget *wid, short w, short h )
{
    listview *lv = (listview *) wid;
    lv->widget.w     = MAX ( 0, w );
    lv->widget.h     = MAX ( 0, h );
    lv->max_rows     = MAX ( 0, ( config.line_margin + lv->widget.h ) / ( lv->element_height + config.line_margin ) );
    lv->max_elements = lv->max_rows * config.menu_columns;

    widget_move ( WIDGET ( lv->scrollbar ), lv->widget.w - 4, 0 );
    widget_resize (  WIDGET ( lv->scrollbar ), 4, h );

    listview_recompute_elements ( lv );
    widget_queue_redraw ( wid );
}

listview *listview_create ( listview_update_callback cb, void *udata )
{
    listview *lv = g_malloc0 ( sizeof ( listview ) );
    lv->widget.free    = listview_free;
    lv->widget.resize  = listview_resize;
    lv->widget.draw    = listview_draw;
    lv->widget.enabled = TRUE;

    lv->scrollbar                = scrollbar_create ( 0, 0, 8, 0 );
    lv->scrollbar->widget.parent = WIDGET ( lv );
    if ( config.hide_scrollbar ) {
        widget_disable ( WIDGET ( lv->scrollbar ) );
    }

    // Calculate height of an element.
    lv->element_height = textbox_get_estimated_char_height () * config.element_height;

    lv->callback = cb;
    lv->udata    = udata;
    return lv;
}

/**
 * Navigation commands.
 */

void listview_nav_up ( listview *lv )
{
    if ( lv == NULL ) {
        return;
    }
    if ( lv->req_elements == 0 || ( lv->selected == 0 && !config.cycle ) ) {
        return;
    }
    if ( lv->selected == 0 ) {
        lv->selected = lv->req_elements;
    }
    lv->selected--;
    widget_queue_redraw ( WIDGET ( lv ) );
}
void listview_nav_down ( listview *lv )
{
    if ( lv == NULL ) {
        return;
    }
    if ( lv->req_elements == 0 || ( lv->selected == ( lv->req_elements - 1 ) && !config.cycle ) ) {
        return;
    }
    lv->selected = lv->selected < lv->req_elements - 1 ? MIN ( lv->req_elements - 1, lv->selected + 1 ) : 0;

    widget_queue_redraw ( WIDGET ( lv ) );
}

void listview_nav_left ( listview *lv )
{
    if ( lv == NULL ) {
        return;
    }
    if ( lv->selected >= lv->max_rows ) {
        lv->selected -= lv->max_rows;
        widget_queue_redraw ( WIDGET ( lv ) );
    }
}
void listview_nav_right ( listview *lv )
{
    if ( lv == NULL ) {
        return;
    }
    if ( ( lv->selected + lv->max_rows ) < lv->req_elements ) {
        lv->selected += lv->max_rows;
        widget_queue_redraw ( WIDGET ( lv ) );
    }
    else if ( lv->selected < ( lv->req_elements - 1 ) ) {
        // We do not want to move to last item, UNLESS the last column is only
        // partially filled, then we still want to move column and select last entry.
        // First check the column we are currently in.
        int col = lv->selected / lv->max_rows;
        // Check total number of columns.
        int ncol = lv->req_elements / lv->max_rows;
        // If there is an extra column, move.
        if ( col != ncol ) {
            lv->selected = lv->req_elements - 1;
            widget_queue_redraw ( WIDGET ( lv ) );
        }
    }
}

void listview_nav_page_prev ( listview *lv )
{
    if ( lv == NULL ) {
        return;
    }
    if ( lv->selected < lv->max_elements ) {
        lv->selected = 0;
    }
    else{
        lv->selected -= ( lv->max_elements );
    }
    widget_queue_redraw ( WIDGET ( lv ) );
}
void listview_nav_page_next ( listview *lv )
{
    if ( lv == NULL ) {
        return;
    }
    if ( lv->req_elements == 0 ) {
        return;
    }
    lv->selected += ( lv->max_elements );
    if ( lv->selected >= lv->req_elements ) {
        lv->selected = lv->req_elements - 1;
    }
    widget_queue_redraw ( WIDGET ( lv ) );
}

unsigned int listview_get_desired_height ( listview *lv )
{
    if ( lv == NULL || lv->req_elements == 0 ) {
        return 0;
    }
    unsigned int h = MIN ( config.menu_lines, lv->req_elements );
    return h * lv->element_height + ( h - 1 ) * config.line_margin;
}
