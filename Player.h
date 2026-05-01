#ifndef MP3_PLAYER_PLAYER_H
#define MP3_PLAYER_PLAYER_H

#include "Model.h"

bool startTrack(size_t index, int32_t startAtSecond = -1);
void playNextTrack();
void playPreviousTrack();
void togglePlayPause();
void stopPlayback();
bool seekToSecond(uint32_t second);

void setSleepTimer(SleepTimerMode mode);
void cycleSleepTimer();
void applyEqMode();
void initAudioCallbacks();

#endif
