# LVGL Pro UI Baseline

This folder now includes a starter XML structure mapped to current app states in `Model.h` and `Ui.cpp`.

## Screens

- `screens/boot.xml`
- `screens/now_playing.xml`
- `screens/search.xml`
- `screens/library.xml`
- `screens/favorites.xml`
- `screens/screensaver.xml`
- `screens/error.xml`

## Reusable Building Blocks

- `components/top_status.xml`
- `widgets/action_button/widget.xml`

## Next Step

Wire these generated objects in `UiLvglPro.cpp`:

- call generated `ui_init()` in `lvglProInit()`
- map `AppState` to generated screen pointers in `lvglProScreenForState()`
- update labels/sliders from model in `lvglProSyncFromModel()`
