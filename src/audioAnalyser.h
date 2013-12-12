#include "ofMain.h"

#pragma once
class AudioAnalyser
{
public:
	AudioAnalyser(void);
	~AudioAnalyser(void);

	//Music Holder + Analysis
	ofSoundPlayer song;

	float *fftSmoothed;
	int FFTBands;
	float FFTDecay;
	float BassTotal;
	float BassAverage;
	int BassBandAmount;

	//Methods
	float update();
	void draw();
	void songPlay();
	void songStop();
	void setPaused(bool pause);

	//Song Selection
	void processOpenFileSelection(ofFileDialogResult openFileResult);
};

