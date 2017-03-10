#ifndef THEME_H
#define THEME_H
#include <glib.h>
#include <cairo.h>
#include <widgets/widget.h>
#include <settings.h>

/** Style of text highlight */
typedef enum
{
    /** no highlight */
    HL_NONE      = 0,
    /** bold */
    HL_BOLD      = 1,
    /** underline */
    HL_UNDERLINE = 2,
    /** italic */
    HL_ITALIC    = 4,
    /** color */
    HL_COLOR     = 8
} HighlightStyle;

/** Style of line */
typedef enum
{
    /** Solid line */
    SOLID,
    /** Dashed line */
    DASH
} LineStyle;

/**
 * Distance unit type.
 */
typedef enum
{
    /** PixelWidth in pixels. */
    PW_PX,
    /** PixelWidth in EM. */
    PW_EM,
    /** PixelWidget in percentage */
    PW_PERCENT,
} PixelWidth;

/**
 * Structure representing a distance.
 */
typedef struct
{
    /** Distance */
    double     distance;
    /** Unit type of the distance */
    PixelWidth type;
    /** Style of the line */
    LineStyle  style;
} Distance;

/**
 * Type of orientation.
 */
typedef enum
{
    ORIENTATION_VERTICAL,
    ORIENTATION_HORIZONTAL
} Orientation;
/**
 * Type of property
 */
typedef enum
{
    /** Integer */
    P_INTEGER,
    /** Double */
    P_DOUBLE,
    /** String */
    P_STRING,
    /** Boolean */
    P_BOOLEAN,
    /** Color */
    P_COLOR,
    /** Padding */
    P_PADDING,
    /** Link to global setting */
    P_LINK,
    /** Position */
    P_POSITION,
    /** Highlight */
    P_HIGHLIGHT,
} PropertyType;

/**
 * Represent the color in theme.
 */
typedef struct
{
    /** red channel */
    double red;
    /** green channel */
    double green;
    /** blue channel */
    double blue;
    /**  alpha channel */
    double alpha;
} ThemeColor;

/**
 * Padding
 */
typedef struct
{
    Distance top;
    Distance right;
    Distance bottom;
    Distance left;
} Padding;

/**
 * Theme highlight.
 */
typedef struct
{
    /** style to display */
    HighlightStyle style;
    /** Color */
    ThemeColor     color;
} ThemeHighlight;
/**
 * Property structure.
 */
typedef struct Property
{
    /** Name of property */
    char         *name;
    /** Type of property. */
    PropertyType type;
    /** Value */
    union
    {
        /** integer */
        int        i;
        /** Double */
        double     f;
        /** String */
        char       *s;
        /** boolean */
        gboolean   b;
        /** Color */
        ThemeColor color;
        /** Padding */
        Padding    padding;
        /** Reference */
        struct
        {
            /** Name */
            char            *name;
            /** Cached looked up ref */
            struct Property *ref;
        }              link;
        /** Highlight Style */
        ThemeHighlight highlight;
    } value;
} Property;
/**
 * ThemeWidget.
 */
typedef struct ThemeWidget
{
    int                set;
    char               *name;

    unsigned int       num_widgets;
    struct ThemeWidget **widgets;

    GHashTable         *properties;

    struct ThemeWidget *parent;
} ThemeWidget;

/**
 * Global pointer to the current active theme.
 */
extern ThemeWidget *rofi_theme;

/**
 * @param base Handle to the current level in the theme.
 * @param name Name of the new element.
 *
 * Create a new element in the theme structure.
 *
 * @returns handle to the new entry.
 */
ThemeWidget *rofi_theme_find_or_create_name ( ThemeWidget *base, const char *name );

/**
 * @param widget The widget handle.
 *
 * Print out the widget to the commandline.
 */
void rofi_theme_print ( ThemeWidget *widget );

/**
 * @param type The type of the property to create.
 *
 * Create a theme property of type.
 *
 * @returns a new property.
 */
Property *rofi_theme_property_create ( PropertyType type );

/**
 * @param p The property to free.
 *
 * Free the content of the property.
 */
void rofi_theme_property_free ( Property *p );

/**
 * @param wid
 *
 * Free the widget and alll children.
 */
void rofi_theme_free ( ThemeWidget *wid );

/**
 * @param file filename to parse.
 *
 * Parse the input theme file.
 *
 * @returns returns TRUE when error.
 */
gboolean rofi_theme_parse_file ( const char *file );

/**
 * @param string to parse.
 *
 * Parse the input string in addition to theme file.
 *
 * @returns returns TRUE when error.
 */
gboolean rofi_theme_parse_string ( const char *string );

/**
 * @param widget The widget handle.
 * @param table HashTable containing properties set.
 *
 * Merge properties with widgets current property.
 */
void rofi_theme_widget_add_properties ( ThemeWidget *widget, GHashTable *table );

/**
 * Public API
 */

/**
 * @param widget   The widget to query
 * @param property The property to query.
 * @param def      The default value.
 *
 * Obtain the distance of the widget.
 *
 * @returns The distance value of this property for this widget.
 */
Distance rofi_theme_get_distance ( const widget *widget, const char *property, int def );

/**
 * @param widget   The widget to query
 * @param property The property to query.
 * @param def      The default value.
 *
 * Obtain the integer of the widget.
 *
 * @returns The integer value of this property for this widget.
 */
int rofi_theme_get_integer   (  const widget *widget, const char *property, int def );

/**
 * @param widget   The widget to query
 * @param property The property to query.
 * @param def      The default value.
 *
 * Obtain the position of the widget.
 *
 * @returns The position value of this property for this widget.
 */
int rofi_theme_get_position ( const widget *widget, const char *property, int def );

/**
 * @param widget   The widget to query
 * @param property The property to query.
 * @param def      The default value.
 *
 * Obtain the integer of the widget.
 *
 * @returns The integer value of this property for this widget.
 */
int rofi_theme_get_integer_exact ( const widget *widget, const char *property, int def );

/**
 * @param widget   The widget to query
 * @param property The property to query.
 * @param def      The default value.
 *
 * Obtain the boolean of the widget.
 *
 * @returns The boolean value of this property for this widget.
 */
int rofi_theme_get_boolean   (  const widget *widget, const char *property, int def );

/**
 * @param widget   The widget to query
 * @param property The property to query.
 * @param def      The default value.
 *
 * Obtain the string of the widget.
 *
 * @returns The string value of this property for this widget.
 */
char *rofi_theme_get_string  (  const widget *widget, const char *property, char *def );

/**
 * @param widget   The widget to query
 * @param property The property to query.
 * @param def      The default value.
 *
 * Obtain the padding of the widget.
 *
 * @returns The double value of this property for this widget.
 */
double rofi_theme_get_double (  const widget *widget, const char *property, double def );

/**
 * @param widget   The widget to query
 * @param property The property to query.
 * @param d        The drawable to apply color.
 *
 * Obtain the color of the widget and applies this to the drawable d.
 *
 */
void rofi_theme_get_color ( const widget *widget, const char *property, cairo_t *d );

/**
 * @param widget   The widget to query
 * @param property The property to query.
 * @param pad      The default value.
 *
 * Obtain the padding of the widget.
 *
 * @returns The padding of this property for this widget.
 */
Padding rofi_theme_get_padding ( const widget *widget, const char *property, Padding pad );

/**
 * @param widget   The widget to query
 * @param property The property to query.
 * @param th The default value.
 *
 * Obtain the highlight .
 *
 * @returns The highlight of this property for this widget.
 */
ThemeHighlight rofi_theme_get_highlight ( widget *widget, const char *property, ThemeHighlight th );

/**
 * @param d The distance handle.
 * @param ori The orientation.
 *
 * Convert Distance into pixels.
 * @returns the number of pixels this distance represents.
 */
int distance_get_pixel ( Distance d, Orientation ori );
/**
 * @param d The distance handle.
 * @param draw The cairo drawable.
 *
 * Set linestyle.
 */
void distance_get_linestyle ( Distance d, cairo_t *draw );

#ifdef THEME_CONVERTER
/**
 * Function to convert old theme into new theme format.
 */
void rofi_theme_convert_old_theme ( void );
#endif

/**
 * Low-level functions.
 * These can be used by non-widgets to obtain values.
 */
/**
 * @param name The name of the element to find.
 * @param state The state of the element.
 * @param exact If the match should be exact, or parent can be included.
 *
 * Find the theme element. If not exact, the closest specified element is returned.
 *
 * @returns the ThemeWidget if found, otherwise NULL.
 */
ThemeWidget *rofi_theme_find_widget ( const char *name, const char *state, gboolean exact );

/**
 * @param widget The widget to find the property on.
 * @param type   The %PropertyType to find.
 * @param property The property to find.
 * @param exact  If the property should only be found on this widget, or on parents if not found.
 *
 * Find the property on the widget. If not exact, the parents are searched recursively until match is found.
 *
 * @returns the Property if found, otherwise NULL.
 */
Property *rofi_theme_find_property ( ThemeWidget *widget, PropertyType type, const char *property, gboolean exact );

#endif
