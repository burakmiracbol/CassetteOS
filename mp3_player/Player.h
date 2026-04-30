#ifndef MP3_PLAYER_PLAYER_H
#define MP3_PLAYER_PLAYER_H

#include "Model.h"

bool startTrack(size_t index);
void playNextTrack();
void playPreviousTrack();
void togglePlayPause();
void stopPlayback();

void setSleepTimer(SleepTimerMode mode);
void cycleSleepTimer();

#endif
