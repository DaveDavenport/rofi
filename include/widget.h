#ifndef ROFI_WIDGET_H
#define ROFI_WIDGET_H
#include <glib.h>
#include <cairo.h>
/**
 * @defgroup Widgets Widgets
 *
 * Generic Widget class
 *
 * @{
 */
typedef struct _Widget
{
    /** X position relative to parent */
    short    x;
    /** Y position relative to parent */
    short    y;
    /** Width of the widget */
    short    w;
    /** Height of the widget */
    short    h;
    /** enabled or not */
    gboolean enabled;
    /** Information about packing. */
    gboolean expand;
    gboolean end;
    /** Function prototypes */

    void ( *draw )( struct _Widget *widget, cairo_t *draw );
    void ( *free )( struct _Widget *widget );
    void ( *resize) ( struct _Widget *, short, short );
    int  ( *get_width) ( struct _Widget *);
    int  ( *get_height) ( struct _Widget *);
} Widget;

/** Macro to get widget from an implementation (e.g. textbox/scrollbar) */
#define WIDGET( a )    ( a != NULL ? (Widget *) ( a ) : NULL )

/**
 * @param widget The widget to check
 * @param x The X position relative to parent window
 * @param y the Y position relative to parent window
 *
 * Check if x,y falls within the widget.
 *
 * @return TRUE if x,y falls within the widget
 */
int widget_intersect ( const Widget *widget, int x, int y );

/**
 * @param widget The widget to move
 * @param x The new X position relative to parent window
 * @param y The new Y position relative to parent window
 *
 * Moves the widget.
 */
void widget_move ( Widget *widget, short x, short y );

gboolean widget_enabled ( Widget *widget );
void widget_disable ( Widget *widget );
void widget_enable ( Widget *widget );

/**
 * @param tb  Handle to the widget
 * @param draw The cairo object used to draw itself.
 *
 * Render the textbox.
 */
void widget_draw ( Widget *widget, cairo_t *d );

/**
 * @param tb  Handle to the widget
 *
 * Free the widget and all allocated memory.
 */
void widget_free ( Widget *widget );

/**
 * @param widget The widget toresize  
 * @param w The new width 
 * @param h The new height 
 *
 * Resizes the widget.
 */
void widget_resize ( Widget *widget, short w, short h );


int widget_get_height ( Widget *widget );
int widget_get_width ( Widget *widget );
Widget *widget_create (void );
/*@}*/
#endif // ROFI_WIDGET_H
