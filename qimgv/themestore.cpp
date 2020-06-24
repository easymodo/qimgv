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
            base.slider_handle = p.highlight().color(); // needs to be darker / desaturated?
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
            base.accent = "#4f6a91";
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
    //if(widget.valueF() <= 0.45f) {
    // top bar buttons
    panel_button_hover.setHsv(folderview_topbar.hue(), folderview_topbar.saturation(), qMin(folderview_topbar.value() + 30, 255));
    panel_button_pressed.setHsv(folderview_topbar.hue(), folderview_topbar.saturation(), qMin(folderview_topbar.value() + 20, 255));
    folderview_separator.setHsv(folderview.hue(), folderview.saturation(), qMin(folderview.value() + 22, 255));
    // regular buttons - from widget bg
    button.setHsv(widget.hue(), widget.saturation(), qMin(widget.value() + 20, 255));
    button_hover   = QColor(button.lighter(112));
    button_pressed = QColor(button.darker(112));
    button_border  = QColor(button.darker(145));
    // accents (saturation tweak)
    accent_lc.setHsv(accent.hue(), accent.saturation() * 0.9, accent.value() * 0.8f);
    accent_lc2.setHsv(accent.hue(), accent.saturation() * 0.5, accent.value() * 0.62f);
    accent_hc.setHsv(accent.hue(), qMin(static_cast<int>(accent.saturation() * 1.1), 255), qMin(static_cast<int>(accent.value() * 1.2), 255));
    // text
    text_hc2 = QColor(text.lighter(118));
    text_hc1 = QColor(text.lighter(110));
    text_lc1 = QColor(text.darker(104));
    text_lc2 = QColor(text.darker(112));
    // misc
    input_field_focus = QColor(accent);
    slider_hover      = QColor(slider_handle.lighter(120));
    slider_groove     = folderview;
}

