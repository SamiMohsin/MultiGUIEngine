/**
 * @file icons.h
 * @brief Built-in Google Material Icons & Procedural Vector Glyphs.
 */
#ifndef MG_UI_ICONS_H
#define MG_UI_ICONS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum mg_icon_id {
    MG_ICON_NONE = 0,
    MG_ICON_ADD,
    MG_ICON_CLOSE,
    MG_ICON_CHECK,
    MG_ICON_MENU,
    MG_ICON_SEARCH,
    MG_ICON_SETTINGS,
    MG_ICON_ARROW_BACK,
    MG_ICON_ARROW_FORWARD,
    MG_ICON_FAVORITE,
    MG_ICON_SHARE,
    MG_ICON_EDIT,
    MG_ICON_DELETE,
    MG_ICON_HOME,
    MG_ICON_INFO,
    MG_ICON_WARNING,
    MG_ICON_REFRESH,
    MG_ICON_MORE_VERT,
    MG_ICON_STAR,
    MG_ICON_CALENDAR,
    MG_ICON_CLOCK
} mg_icon_id_t;

#ifdef __cplusplus
}
#endif

#endif /* MG_UI_ICONS_H */
