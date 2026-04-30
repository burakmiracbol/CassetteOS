# ESP32 MP3 Player

Modular Arduino sketch for an ESP32-S3 MP3 player with LVGL UI, SD library scan, I2S audio output, favorites, search, sleep timer, screensaver, and display-off state handling.

## Files

- `mp3_player.ino`: startup sequence and main loop
- `Config.h`: pins, display size, timeouts, optional touch CS
- `Model.*`: app/player state and formatting helpers
- `Hardware.*`: TFT, LVGL port, SD SPI bus, battery, LED ring, timers
- `Storage.*`: SD MP3 scan and favorites persistence
- `Player.*`: playback controls, repeat/shuffle, sleep timer, audio callbacks
- `Ui.*`: LVGL screens and widget callbacks
- `lv_conf.h`: project-local LVGL config so the sketch builds without editing Arduino libraries

## Compile

LVGL + ESP32-audioI2S is larger than the default ESP32-S3 app partition. Use a larger partition scheme:

```powershell
arduino-cli compile --fqbn esp32:esp32:esp32s3:PartitionScheme=huge_app .\mp3_player
```

If your TFT_eSPI setup does not define `TOUCH_CS`, the sketch still compiles but touch input is disabled. Set `MP3_PLAYER_TOUCH_CS` in `Config.h` or enable `TOUCH_CS` in TFT_eSPI's `User_Setup.h`.
