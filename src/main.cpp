#include "ofMain.h"
#include "testApp.h"

void GetDesktopResolution(int& horizontal, int& vertical){
   RECT desktop;
   // Get a handle to the desktop window
   const HWND hDesktop = GetDesktopWindow();
   // Get the size of screen to the variable desktop
   GetWindowRect(hDesktop, &desktop);
   // The top left corner will have coordinates (0,0)
   // and the bottom right corner will have coordinates
   // (horizontal, vertical)
   horizontal = desktop.right;
   vertical = desktop.bottom;
}
//========================================================================
int main( ){
	int horizontalscreen = 0;
	int verticalscreen = 0;
	GetDesktopResolution(horizontalscreen,verticalscreen);

	ofSetupOpenGL(horizontalscreen * 0.75,verticalscreen * 0.75, OF_FULLSCREEN);			// <-------- setup the GL context

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp( new testApp());

}
