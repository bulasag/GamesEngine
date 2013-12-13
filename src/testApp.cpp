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

	//Create boundaries + targets
	setupBoundary();

	mousePickIndex	= -1;
	mouseZ = 10;
	modeToggled	= true;
	pullForce = true;
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
	
	//Apply pull/push force to shapes
	if(spacebarHeld) {
        mousePickIndex = -1;
		ofVec3f mouseLoc = camera.screenToWorld( ofVec3f((float)ofGetMouseX(), (float)ofGetMouseY(), 0) );
		mouseLoc.z += mouseZ;
		ofVec3f shapeToCursorDistance;
		for(int i = 0; i < shapes.size(); i++)
		{
			shapeToCursorDistance = mouseLoc - shapes[i]->getPosition();
			shapeToCursorDistance *= 2.f;
			if (!pullForce)
			{
			shapeToCursorDistance *= -1.f;
			}
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
	light.enable();	//OOO GLOWY
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
			if(shapes[i]->getType() == ofxBulletBaseShape::OFX_BULLET_BOX_SHAPE)
			{
				ofSetColor(ofColor::dimGray);
			}
			else
			{
				ofSetColor(ofColor::gray);
			}

			//Highlight picked up objects
			if(mousePickIndex == i)
			{
				ofSetColor(255, 0, 0);
			}
			else if (bColliding[i] == true)
			{
				//Highlight shapes when colliding with other shapes
				if(shapes[i]->getType() == ofxBulletBaseShape::OFX_BULLET_BOX_SHAPE)
				{
					ofSetColor(ofColor::gold);
				}
				else
				{
					ofSetColor(ofColor::orangeRed);
				}
			}
			shapes[i]->draw();
		}

		ofSetColor(ofColor::antiqueWhite);
		for(int i = 0; i < targets.size(); i++)
		{
			targets[i]->draw();
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
//Handle Bullet callback
void testApp::onCollision(ofxBulletCollisionData &cdata) {
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
	float randomSize = ofRandom(1.0, 2.0);
	mouseLoc.z += mouseZ;

	if(key == 'b')	//Spawn new Box
	{
		shapes.push_back( new ofxBulletBox() );
		ii = shapes.size()-1;
		((ofxBulletBox*)shapes[ii])->create(world.world, mouseLoc, randomSize*.2, randomSize*2, randomSize*2, randomSize*2);
		shapes[ii]->setActivationState( DISABLE_DEACTIVATION );
		shapes[ii]->add();
		bColliding.push_back(false);
	}
	if(key == 's')	//Spawn new Sphere
	{
		shapes.push_back( new ofxBulletSphere() );
		ii = shapes.size()-1;
		((ofxBulletSphere*)shapes[ii])->create(world.world, mouseLoc, randomSize*.2, randomSize);
		shapes[ii]->setActivationState( DISABLE_DEACTIVATION );
		shapes[ii]->add();
		bColliding.push_back(false);
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
		pullForce = !pullForce;
	}
	if(key == 't')	//Move MouseZ away from camera
	{
		if (mouseZ < 70)
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
	if(key == 'q')	//Spawn targets
	{
		setupTargets();
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
	//Apply positional changes during drag, multiply to rotate
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
	//Blast
	ofVec3f blast = ofVec3f(0.f,0.f,5000.f);

	for(int i = 0; i < shapes.size(); i++)
	{
		if(mousePickIndex == i)
		{
			shapes[i]->applyCentralForce(blast);
		}
	}

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
	ss << "Push/Pull (F): " << pullForce << endl;
	ss << "MouseZ+ (T): " << mouseZ << endl;
	ss << "MouseZ- (G)" << endl;
	ss << "Spawn Spheres (S)" << endl;
	ss << "Spawn Boxes (B)" << endl;
	ss << "Spawn Targets (Q)" << endl;
	ss << "Play (A)" << endl;
	ss << "Resume/Pause (D)" << endl;
	ss << ofGetMouseX() << " :XY: " << ofGetMouseY() << endl;
	ofDrawBitmapString(ss.str().c_str(), 10, 10);
}

//--------------------------------------------------------------
void testApp::setupBoundary()
{
	ofVec3f startLoc;
	ofPoint dimens;
	boundsWidth = 40.;
	float hwidth = boundsWidth*0.5;
	float depth = 4.;	//Wall Thickness
	float hdepth = depth*.5;

	for(int i = 0; i < 3; i++) {
		bounds.push_back( new ofxBulletBox() );
		if(i == 0) { //Ground
			startLoc.set( 0., hwidth+hdepth, 0.);
			dimens.set(boundsWidth*10, depth, boundsWidth*4);
		} else if (i == 1) { //Back Wall
			startLoc.set(0, 0, (hwidth+hdepth)*4);
			dimens.set(boundsWidth * 10, boundsWidth, depth * 3);
		} else if (i == 2) { //Front Wall
			startLoc.set(0, 0, -hwidth-hdepth);
			dimens.set(boundsWidth, boundsWidth, depth * 3);
		}

		bounds[i]->create( world.world, startLoc, 0., dimens.x, dimens.y, dimens.z );
		bounds[i]->setProperties(.25, .95);	//Restitution(Bounce) and Friction
		bounds[i]->add();
	}
}
//--------------------------------------------------------------
void testApp::setupTargets()
{
	int iii = 0;
	ofVec3f spawnLoc = camera.screenToWorld( ofVec3f((float)ofGetMouseX(), (float)ofGetMouseY(), 0) );
	float targetSize = 4.0;
	spawnLoc.z += 60;

	targets.push_back(new ofxBulletBox());
	iii = targets.size()-1;
	((ofxBulletBox*)targets[iii])->create(world.world, spawnLoc, targetSize*.2, targetSize*2, targetSize*2, targetSize*2);
	targets[iii]->setActivationState( DISABLE_DEACTIVATION );
	targets[iii]->add();
	bColliding.push_back( false );
}