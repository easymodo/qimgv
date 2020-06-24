#pragma once

#include <QColor>
#include <QPalette>

enum ColorSchemes {
    COLORS_SYSTEM,
    COLORS_LIGHT,
    COLORS_DARK,
    COLORS_DEFAULT
};

struct BaseColorScheme {
    QColor background;
    QColor background_fullscreen;
    QColor text;
    QColor widget;
    QColor widget_border;
    QColor accent;
    QColor folderview;
    QColor folderview_topbar;
    QColor slider_handle;
    QColor overlay_text;
    QColor overlay;
};

class ColorScheme {
public:
    ColorScheme();
    ColorScheme(BaseColorScheme base);
    void setBaseColors(BaseColorScheme base);

    // base
    QColor background;
    QColor background_fullscreen;
    QColor text;
    QColor widget;
    QColor widget_border;
    QColor accent;
    QColor folderview;
    QColor folderview_topbar;
    QColor slider_handle;
    QColor overlay_text;
    QColor overlay;
    // extended
    QColor text_hc2;
    QColor text_hc1;
    QColor text_lc1;
    QColor text_lc2;
    QColor button;
    QColor button_hover;
    QColor button_pressed;
    QColor button_border;
    QColor panel_button_hover;
    QColor panel_button_pressed;
    QColor accent_lc;
    QColor accent_lc2;
    QColor accent_hc;
    QColor folderview_separator;
    QColor slider_groove;
    QColor slider_hover;
    QColor input_field_focus;


private:
    void createColorVariants();
};

class ThemeStore {
public:
    static ColorScheme colorScheme(ColorSchemes name);
};
