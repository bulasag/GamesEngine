#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup() {
	ofSetWindowTitle("Wacky Games Engine Shenanigans");
	ofSetFrameRate(60);
	ofSetVerticalSync(true);
	ofBackground( 10, 10, 10);

	//Camera Setup
	camera.setPosition(ofVec3f(0, -4.f, -40.f));
	camera.lookAt(ofVec3f(0, 0, 0), ofVec3f(0, -1, 0));

	world.setup();
	//Mouse Grab Enable
	world.enableGrabbing();
	ofAddListener(world.MOUSE_PICK_EVENT, this, &testApp::mousePickEvent);

	world.enableCollisionEvents();
	ofAddListener(world.COLLISION_EVENT, this, &testApp::onCollision);
	world.setCamera(&camera);
	world.setGravity(ofVec3f(0, 25., 0));
	light.setPosition(camera.getPosition().x, camera.getPosition().y-14, camera.getPosition().z);

	int ii = 0;

	//Create box to contain stuff
	setupBoundary();

	mousePickIndex	= -1;
	mouseZ = 10;
	modeToggled	= true;
	forceApplied = true;
	spacebarHeld = false;

	//KtB Setup
	//Slow rotation down
	dampen = .4;
	musicPaused = false;

	sizeMultiplier = 1.0;
}

//--------------------------------------------------------------
void testApp::update() {
	
	//No collisions unless told otherwise by onCollision() method
	for(int i = 0; i < shapes.size(); i++) {
		bColliding[i] = false;
	}
	
	//Apply pull/push force
	if(spacebarHeld) {
        mousePickIndex = -1;
		ofVec3f mouseLoc = camera.screenToWorld( ofVec3f((float)ofGetMouseX(), (float)ofGetMouseY(), 0) );
		mouseLoc.z += mouseZ;
		ofVec3f shapeToCursorDistance;
		for(int i = 0; i < shapes.size(); i++) {
			shapeToCursorDistance = mouseLoc - shapes[i]->getPosition();
			shapeToCursorDistance *= 2.f;
			if (!forceApplied) shapeToCursorDistance *= -1.f;
			shapes[i]->applyCentralForce(shapeToCursorDistance);
		}
	}

	world.update();

	//Multiply quaternion, vector, inverse quaternion and it rotates; don't you see?
	//Swirling PulseBall
	ofQuaternion xRot((ofRandom(0, 2))*dampen, ofVec3f(-1,0,0));
    ofQuaternion yRot((ofRandom(0, 2))*dampen, ofVec3f(0,1,0));
	pulseBall.curRot *= xRot*yRot;

	//KtB Updates
	ofSoundUpdate();
	sizeMultiplier = audioAnalyser.update();
	sizeMultiplier *= 0.98f;
}

//--------------------------------------------------------------
void testApp::draw() {
	glEnable( GL_DEPTH_TEST );
	camera.begin();

	//Draw Boundary Box
	ofSetLineWidth(1.f);
	ofSetColor(255, 0, 200);
	ofEnableLighting();
	light.enable();
	if(modeToggled) {
		ofSetColor(100, 100, 100);
		for(int i = 0; i < bounds.size()-1; i++) {
			ofNoFill();
			ofSetColor(ofColor::white);
			bounds[i]->draw();
		}

		//Draw spawned shapes
		ofFill();
		for(int i = 0; i < shapes.size(); i++) {
			if(shapes[i]->getType() == ofxBulletBaseShape::OFX_BULLET_BOX_SHAPE) {
				ofSetColor(ofColor::dimGray);
			} else {
				ofSetColor(ofColor::gray);
			}
			if(mousePickIndex == i) {
				ofSetColor(255, 0, 0);
			} else if (bColliding[i] == true) {
				if(shapes[i]->getType() == ofxBulletBaseShape::OFX_BULLET_BOX_SHAPE) {
					ofSetColor(ofColor::gold);
				} else {
					ofSetColor(ofColor::orangeRed);
				}
			}
			shapes[i]->draw();
		}
	}
	light.disable();
	ofDisableLighting();

	camera.end();
	glDisable(GL_DEPTH_TEST);

	//Exhibit 2
	if (modeToggled == false)
	{
		pulseBall.draw(sizeMultiplier);
	}
		audioAnalyser.draw();

	//Misc
	userInterface();
}

//--------------------------------------------------------------
void testApp::onCollision(ofxBulletCollisionData &cdata) {
	for(int j = 0; j < bounds.size(); j++) {
		if(*bounds[j] == cdata) {
			return;
		}
	}

	for (int i = 0; i < shapes.size(); i++) {
		if(*shapes[i] == cdata) {
			bColliding[i] = true;
		}
	}
}

//--------------------------------------------------------------
void testApp::mousePickEvent(ofxBulletMousePickEvent &e) {
	mousePickIndex = -1;
	for(int i = 0; i < shapes.size(); i++) {
		if(*shapes[i] == e) {
			mousePickIndex = i;
			mousePickPos = e.pickPosWorld;
			break;
		}
	}
}

//--------------------------------------------------------------
void testApp::keyPressed(int key) {
	//BULLET OBJECTS STUFF
	int ii = 0;
	ofVec3f mouseLoc = camera.screenToWorld( ofVec3f((float)ofGetMouseX(), (float)ofGetMouseY(), 0) );
	float rsize = ofRandom(1.0, 2.0);
	mouseLoc.z += mouseZ;

	if(key == 'b')	//Spawn new Box
	{
		shapes.push_back( new ofxBulletBox() );
		ii = shapes.size()-1;
		((ofxBulletBox*)shapes[ii])->create(world.world, mouseLoc, rsize*.2, rsize*2, rsize*2, rsize*2);
		shapes[ii]->setActivationState( DISABLE_DEACTIVATION );
		shapes[ii]->add();
		bColliding.push_back( false );
	}
	if(key == 's')	//Spawn new Sphere
	{
		shapes.push_back( new ofxBulletSphere() );
		ii = shapes.size()-1;
		((ofxBulletSphere*)shapes[ii])->create(world.world, mouseLoc, rsize*.2, rsize);
		shapes[ii]->setActivationState( DISABLE_DEACTIVATION );
		shapes[ii]->add();
		bColliding.push_back( false );
	}
	if(key == 'r')	//Toggle between the exhibits
	{
		modeToggled = !modeToggled;
	}
	if(key == ' ')	//Apply force
	{
		spacebarHeld = true;
	}
	if(key == 'f')	//Toggle between force modes
	{
		forceApplied = !forceApplied;
	}
	if(key == 't')	//Move MouseZ away from camera
	{
		if (mouseZ < 40)
		{
			mouseZ += 10;
		}
	}
	if(key == 'g')	//Move MouseZ towards camera
	{
		if(mouseZ > 10)
		{
			mouseZ -= 10;
		}
	}
	if(key == 'q')	//KABOOM
	{

	}
	

	//MUSIC STUFF
	if(key == 'a')	//Play Song
	{
		audioAnalyser.songPlay();
	}

	if(key == 'd')	//Pause/Resume Song
	{
		if(musicPaused == false)
		{
			audioAnalyser.setPaused(true);
			musicPaused = true;
		}
		else
		{
			audioAnalyser.setPaused(false);
			musicPaused = false;
		}
	}
}

//--------------------------------------------------------------
void testApp::keyReleased(int key)	//Stop force
{
	if(key == ' ')
		spacebarHeld = false;
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button) {
	//Keep track of changes during drag
	//Accumulate them inside of curRot through multiplication
    ofVec2f mouse(x,y);  
    ofQuaternion yRot((x-lastMouse.x)*dampen, ofVec3f(0,1,0));  
    ofQuaternion xRot((y-lastMouse.y)*dampen, ofVec3f(-1,0,0));  
	
	pulseBall.curRot *= yRot*xRot;
    lastMouse = mouse;
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button) {

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
    mousePickIndex = -1;
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo) {

}

//--------------------------------------------------------------
void testApp::userInterface()
{
	//UI Background
	ofEnableAlphaBlending();
	ofSetColor(0, 0, 0, 150);
	ofRect(0, 0, 250, 175);
	ofDisableAlphaBlending();

	//Text
	ofSetColor(255, 255, 255);
	stringstream ss;
	ss << "FPS: " << ofToString(ofGetFrameRate(),0) << endl;
	ss << "No. of Shapes: " << (shapes.size()) << endl;
	ss << "Q+AA / GravityWell Mode (R): " << ofToString(modeToggled, 0) << endl;
	ss << "Mouse Force (Spacebar): " << spacebarHeld << endl;
	ss << "Push/Pull (F): " << forceApplied << endl;
	ss << "MouseZ+ (T): " << mouseZ << endl;
	ss << "MouseZ- (G)" << endl;
	ss << "Spawn Spheres (S)" << endl;
	ss << "Spawn Boxes (B)" << endl;
	ss << "Play (A)" << endl;
	ss << "Resume/Pause (D)" << endl;
	ofDrawBitmapString(ss.str().c_str(), 10, 10);
}

//--------------------------------------------------------------
void testApp::setupBoundary()
{
	ofVec3f startLoc;
	ofPoint dimens;
	boundsWidth = 35.;
	float hwidth = boundsWidth*.5;
	float depth = 2.;
	float hdepth = depth*.5;

	for(int i = 0; i < 6; i++) {
		bounds.push_back( new ofxBulletBox() );
		if(i == 0) { // ground //
			startLoc.set( 0., hwidth+hdepth, 0. );
			dimens.set(boundsWidth, depth, boundsWidth);
		} else if (i == 1) { // back wall //
			startLoc.set(0, 0, hwidth+hdepth);
			dimens.set(boundsWidth, boundsWidth, depth);
		} else if (i == 2) { // right wall //
			startLoc.set(hwidth+hdepth, 0, 0.);
			dimens.set(depth, boundsWidth, boundsWidth);
		} else if (i == 3) { // left wall //
			startLoc.set(-hwidth-hdepth, 0, 0.);
			dimens.set(depth, boundsWidth, boundsWidth);
		} else if (i == 4) { // ceiling //
			startLoc.set(0, -hwidth-hdepth, 0.);
			dimens.set(boundsWidth, depth, boundsWidth);
		} else if (i == 5) { // front wall //
			startLoc.set(0, 0, -hwidth-hdepth);
			dimens.set(boundsWidth, boundsWidth, depth);
		}

		bounds[i]->create( world.world, startLoc, 0., dimens.x, dimens.y, dimens.z );
		bounds[i]->setProperties(.25, .95);
		bounds[i]->add();
	}
}

//--------------------------------------------------------------