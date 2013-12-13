#pragma once

#include "ofMain.h"
#include "ofxBullet.h"
#include "PulseBall.h"
#include "audioAnalyser.h"

class testApp : public ofBaseApp{

public:
	void setup();
	void update();
	void draw();
	
	void onCollision(ofxBulletCollisionData& cdata);
	void mousePickEvent( ofxBulletMousePickEvent &e );

	void keyPressed  (int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
	
	//Boundary Stuff
	ofxBulletWorldRigid			world;
	vector <ofxBulletBox*>		bounds;
	float						boundsWidth;
	
	void						setupBoundary();
	void						setupTargets();
	vector<ofxBulletBaseShape*> targets;

	ofLight						light;
	
	btBoxShape*					boxShape;
	vector<ofxBulletBaseShape*> shapes;
	vector<bool>				bColliding;
	int							mousePickIndex;
	ofVec3f						mousePickPos;
	int							mouseZ;
	
	bool						modeToggled;
	bool						pullForce;
	bool						spacebarHeld;
	
	ofCamera					camera;

	//KtB Stuff
	//Storage for mouse position to measure incremental change  
    ofVec2f lastMouse;
	//Slows rotation to 1 = 1 degree per pixel
	float	dampen;

	PulseBall	pulseBall;
	float		sizeMultiplier;

	AudioAnalyser	audioAnalyser;
	bool			musicPaused;

	//UI
	void userInterface();
};
