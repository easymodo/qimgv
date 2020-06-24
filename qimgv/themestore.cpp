#include "themestore.h"

ColorScheme ThemeStore::colorScheme(ColorSchemes name) {
    BaseColorScheme base;
    QPalette p;
    switch(name) {
        case COLORS_SYSTEM:
            base.folderview_topbar = p.window().color();
            base.widget = p.window().color();
            base.widget_border = p.window().color();
            base.folderview = p.base().color();
            base.text = p.text().color();
            base.accent = p.highlight().color();
            base.slider_handle.setHsv(p.highlight().color().hue(),
                                      qBound(0, p.highlight().color().saturation() - 20, 240),
                                      qBound(0, p.highlight().color().value() - 35, 240));
            break;
        case COLORS_LIGHT:
            // --
            break;
        case COLORS_DARK:
        case COLORS_DEFAULT:
            base.background = "#1a1a1a";
            base.background_fullscreen = "#1a1a1a";
            base.text = "#a4a4a4";
            base.widget = "#252525";
            base.widget_border = "#272727";
            base.accent = "#3371c8";
            base.folderview = "#242424";
            base.folderview_topbar = "#343434";
            base.slider_handle = "#5e5e5e";
            base.overlay_text = "#d2d2d2";
            base.overlay = "#1a1a1a";
            break;
    }
    return ColorScheme(base);
}

//---------------------------------------------------------------------

ColorScheme::ColorScheme() {
}

ColorScheme::ColorScheme(BaseColorScheme base) {
    setBaseColors(base);
}

void ColorScheme::setBaseColors(BaseColorScheme base) {
    background            = base.background;
    background_fullscreen = base.background_fullscreen;
    text                  = base.text;
    widget                = base.widget;
    widget_border         = base.widget_border;
    accent                = base.accent;
    folderview            = base.folderview;
    folderview_topbar     = base.folderview_topbar;
    slider_handle         = base.slider_handle;
    overlay               = base.overlay;
    overlay_text          = base.overlay_text;
    createColorVariants();
}

void ColorScheme::createColorVariants() {
    if(widget.valueF() <= 0.45f) { // dark theme
        // top bar buttons
        panel_button_hover.setHsv(folderview_topbar.hue(), folderview_topbar.saturation(), qMin(folderview_topbar.value() + 30, 255));
        panel_button_pressed.setHsv(folderview_topbar.hue(), folderview_topbar.saturation(), qMin(folderview_topbar.value() + 20, 255));
        folderview_separator.setHsv(folderview.hue(), folderview.saturation(), qMin(folderview.value() + 22, 255));
        // regular buttons - from widget bg
        button.setHsv(widget.hue(), widget.saturation(), qMin(widget.value() + 20, 255));
        button_hover   = QColor(button.lighter(112));
        button_pressed = QColor(button.darker(112));
        button_border  = button;
    } else { // light theme
        // top bar buttons
        panel_button_hover.setHsv(folderview_topbar.hue(), folderview_topbar.saturation(), qMax(folderview_topbar.value() - 40, 0));
        panel_button_pressed.setHsv(folderview_topbar.hue(), folderview_topbar.saturation(), qMax(folderview_topbar.value() - 28, 0));
        folderview_separator.setHsv(folderview.hue(), folderview.saturation(), qMax(folderview.value() - 25, 0));
        // regular buttons - from widget bg
        button.setHsv(widget.hue(), widget.saturation(), qMax(widget.value() - 35, 0));
        button_pressed   = QColor(button.darker(106));
        button_hover = QColor(button.lighter(106));
        button_border  = QColor(button.darker(106));
    }
    // text
    text_hc2 = QColor(text.lighter(118));
    text_hc1 = QColor(text.lighter(110));
    text_lc1 = QColor(text.darker(104));
    text_lc2 = QColor(text.darker(112));
    // misc
    input_field_focus = QColor(accent);
    slider_hover      = QColor(slider_handle.lighter(120));
    slider_groove     = button_hover;
}

