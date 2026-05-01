# LVGL Pro Generated Output

Export LVGL Pro generated C/C++ files into this folder.
Recommended flow:
- Design source stays in `/ui` (repo root).
- Build output goes to `/mp3_player/ui/generated`.
- `mp3_player/ui/UiLvglPro.cpp` is the bridge that maps generated screens/widgets to app state.

Keep this folder in git so include paths remain stable.
