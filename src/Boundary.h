#include "ofMain.h"
#include "ofxBullet.h"

#pragma once
class Boundary
{
public:
	Boundary(void);
	~Boundary(void);

	ofxBulletWorldRigid			world;
	vector <ofxBulletBox*>		bounds;
	float						boundsWidth;

	ofVec3f startLoc;
	ofPoint dimens;
	float hwidth;
	float depth;
	float hdepth;
};

