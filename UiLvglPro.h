#ifndef MP3_PLAYER_UI_LVGL_PRO_H
#define MP3_PLAYER_UI_LVGL_PRO_H

#include "../Hardware.h"
#include "../Model.h"

// Bridge layer for LVGL Pro generated screens.
// Start with stubs, then wire generated ui_init/ui_screen pointers here.
bool lvglProInit();
lv_obj_t *lvglProScreenForState(AppState state);
void lvglProSyncFromModel();

#endif
