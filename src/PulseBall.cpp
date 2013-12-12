#include "PulseBall.h"


PulseBall::PulseBall(void)
{
	maxRadius = 200;
}

void PulseBall::draw(float sizeMultiplier)
{
	//PulseBall's Quaternion Shenanigans

    ofPushMatrix(); //Save coordinates
	
	//Translate to assign center of the screen as 0,0
    ofTranslate(ofGetWidth()/2, ofGetHeight()/2, 40);  

	//A Quaternion is like a vector but with a "w"
    ofVec3f axis;	//To construct one, use an axis and an angle, that's what we do
    float angle;  
    curRot.getRotate(angle, axis);
	curRot.normalize(); //For rotations; it must be a normal, otherwise its pure
	//So we normalise, divide by length, just to be sure (DONE BY OFQUATERNION)

	//Apply quaternion rotation + draw sphere
	ofRotate(angle, axis.x, axis.y, axis.z);
	ofNoFill();

	ofSetColor(255, 255, 255);
	currentRadius = maxRadius * sizeMultiplier;
	ofDrawSphere(0, 0, 0, currentRadius);
	
	ofPopMatrix(); //End Segment
}

PulseBall::~PulseBall(void)
{
}
