#include "AudioAnalyser.h"
#define FFTArray 8192

AudioAnalyser::AudioAnalyser(void)
{
	//Load in sounds
	song.loadSound("sounds/default.mp3");

	fftSmoothed = new float[FFTArray];
	for (int i = 0; i < FFTArray; i++){
		fftSmoothed[i] = 0;
	}

	FFTBands = 128; //128
	BassAverage = 0;
	BassBandAmount = 7;
}

float AudioAnalyser::update(void)
{
	float *val = ofSoundGetSpectrum(FFTBands);		//Request values for FFT

	BassTotal = 0;
	for (int i = 0;i < FFTBands; i++)
	{
		//Decay to 0
		fftSmoothed[i] *= 0.97f;
		
		//Take the greater value; the smoothed value or the incoming value
		if (fftSmoothed[i] < val[i])
			fftSmoothed[i] = val[i];


		if (i >= 0 && i <= BassBandAmount)
		{
			BassTotal += fftSmoothed[i];
		}
	}
	BassAverage = BassTotal / BassBandAmount;

	return BassAverage;
}

void AudioAnalyser::draw(void)
{
	ofFill();

	//FFT Graphing
	float width = (float)(20 * FFTBands) / FFTBands; //20
	for (int FFTDraw = 0; FFTDraw < FFTBands; FFTDraw++){
		ofSetColor(ofRandom(0, 255), 0, 0);
		//Draw bars
		//Vertical bars- Down
		//ofRect(FFTDraw*width, ofGetWindowHeight(), width, -(fftSmoothed[FFTDraw] * 175));
		
		//Horizontal bars- Right
		ofRect(ofGetWindowWidth(), (ofGetWindowHeight() * 0.98) - (FFTDraw*width), -(fftSmoothed[FFTDraw] * 200), width);
		//Horizontal bars- Left
		ofRect(0, (ofGetWindowHeight() * 0.98) - (FFTDraw*width), +(fftSmoothed[FFTDraw] * 200), width);
	}
}

void AudioAnalyser::processOpenFileSelection(ofFileDialogResult openFileResult){
	cout << ("getName(): " + openFileResult.getName() + "\n");
	cout << ("getPath(): " + openFileResult.getPath() + "\n");

	ofFile file (openFileResult.getPath());

	if (file.exists()) {
		cout << "The file exists - now checking the type via file extension \n";
		string fileExtension = ofToUpper(file.getExtension());

		if (fileExtension == "MP3" || fileExtension == "WAV") {
			cout << "Valid file type, loading.. \n";
			song.loadSound(openFileResult.getPath());
			cout << "Ready to play \n";
		}
		else{
			cout << "Invalid file type \n";
		}
	}
}

void AudioAnalyser::songPlay(void)
{
	song.play();
}

void AudioAnalyser::songStop(void)
{
	song.stop();
}

void AudioAnalyser::setPaused(bool pause)
{
	song.setPaused(pause);
}

AudioAnalyser::~AudioAnalyser(void)
{
}
