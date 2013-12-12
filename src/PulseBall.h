#include "ofMain.h"

#pragma once
class PulseBall
{
public:
	PulseBall(void);
	~PulseBall(void);

	//Attributes
	float currentRadius;
	float tempRadius;
	float maxRadius;
	ofColor colour;
	ofQuaternion curRot; //Current state of the rotation

	//Methods
	void draw(float sizeMultiplier);
};

