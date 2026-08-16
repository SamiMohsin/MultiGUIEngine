/**
 * @file theme.h
 * @brief Google Material Design 3 (M3) Color Tokens, Elevation, and Shape System.
 */
#ifndef MG_UI_THEME_H
#define MG_UI_THEME_H

#include "mg/render/rhi.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Complete Google Material 3 Color Scheme.
 */
typedef struct mg_m3_color_scheme {
    mg_color_t primary;
    mg_color_t on_primary;
    mg_color_t primary_container;
    mg_color_t on_primary_container;

    mg_color_t secondary;
    mg_color_t on_secondary;
    mg_color_t secondary_container;
    mg_color_t on_secondary_container;

    mg_color_t tertiary;
    mg_color_t on_tertiary;
    mg_color_t tertiary_container;
    mg_color_t on_tertiary_container;

    mg_color_t error;
    mg_color_t on_error;
    mg_color_t error_container;
    mg_color_t on_error_container;

    mg_color_t surface;
    mg_color_t on_surface;
    mg_color_t surface_variant;
    mg_color_t on_surface_variant;
    mg_color_t surface_container;
    mg_color_t surface_container_high;
    mg_color_t surface_container_highest;

    mg_color_t outline;
    mg_color_t outline_variant;
    mg_color_t scrim;
    mg_color_t shadow;
} mg_m3_color_scheme_t;

/**
 * @brief Returns standard Material 3 Dark Theme color scheme.
 */
mg_m3_color_scheme_t mg_m3_theme_dark(void);

/**
 * @brief Returns standard Material 3 Light Theme color scheme.
 */
mg_m3_color_scheme_t mg_m3_theme_light(void);

#ifdef __cplusplus
}
#endif

#endif /* MG_UI_THEME_H */
