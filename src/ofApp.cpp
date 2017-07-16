#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	// listen on the given port
	cout << "listening for osc messages on port " << PORT << "\n";
	receiver.setup(PORT);

	current_msg_string = 0;
	mouseX = 0;
	mouseY = 0;
	mouseButtonState = "";
	numPoints = 17;
	shapeRad = 3;
	ofBackground(0, 0, 0);

}

//--------------------------------------------------------------
void ofApp::update(){

	// hide old messages
	for(int i = 0; i < NUM_MSG_STRINGS; i++){
		if(timers[i] < ofGetElapsedTimef()){
			msg_strings[i] = "";
		}
	}

	// check for waiting messages
	while(receiver.hasWaitingMessages()){
		// get the next message
		ofxOscMessage m;
		receiver.getNextMessage(m);
        
		// check for mouse moved message
		if(m.getAddress() == "/mouse/position"){
			// both the arguments are int32's
			mouseX = m.getArgAsInt32(0);
			mouseY = m.getArgAsInt32(1);
		}
		// check for mouse button message
		else if(m.getAddress() == "/mouse/button"){
			// the single argument is a string
			mouseButtonState = m.getArgAsString(0);
		}
        // check for an image being sent (note: the size of the image depends greatly on your network buffer sizes - if an image is too big the message won't come through ) 
        else if(m.getAddress() == "/image" ){
            ofBuffer buffer = m.getArgAsBlob(0);
            receivedImage.load(buffer);
        }
		else{
			// unrecognized message: display on the bottom of the screen
			string msg_string;
			msg_string = m.getAddress();
			
			std::string addr = m.getAddress();
			cout << "address: " << addr << endl;
			if(addr == "/3/xy1")  {
				numPoints = m.getArgAsFloat(0);
				shapeRad = m.getArgAsFloat(1);
				// for(int i = 0; i < m.getNumArgs(); i++){
				// float outnum = m.getArgAsFloat(i);
				// if(i == 0) {
				// 	numPoints = outnum;
				// }
				// if(i == 0) {
				// 	shapeRad = outnum;
				// }
				// cout << "output: " << i << " " << ofToString(outnum) << endl;
				// }
			}

            if(addr == "/3/xy2") {
              scale = m.getArgAsFloat(0) * 500;
              theta = m.getArgAsFloat(1);
            }

			
			msg_string += ": ";
			for(int i = 0; i < m.getNumArgs(); i++){
				// get the argument type
				msg_string += m.getArgTypeName(i);
				
				msg_string += ":";
				// display the argument - make sure we get the right type
				if(m.getArgType(i) == OFXOSC_TYPE_INT32){
					msg_string += ofToString(m.getArgAsInt32(i));
				}
				else if(m.getArgType(i) == OFXOSC_TYPE_FLOAT){
					msg_string += ofToString(m.getArgAsFloat(i));
				}
				else if(m.getArgType(i) == OFXOSC_TYPE_STRING){
					msg_string += m.getArgAsString(i);
				}
				else{
					msg_string += "unknown";
				}
			}
			// add to the list of strings to display
			msg_strings[current_msg_string] = msg_string;
			timers[current_msg_string] = ofGetElapsedTimef() + 5.0f;
			current_msg_string = (current_msg_string + 1) % NUM_MSG_STRINGS;
			// clear the next line
			msg_strings[current_msg_string] = "";
		}

	}
}

void drawTriangle(float scale, float theta) {
  // phi (angle of orientation) 0 is "up"
  // θ (theta) (spread of triangle)  0 < θ < π
  // hight is how tall it is
  // int s = 100;
  // float theta = (TWO_PI/2)/3;
  int s = scale;
  theta = theta * TWO_PI/2;
  ofBeginShape();
  // ofScale(1.0,2.0,1.0);
  float p0x = 100;
  float p0y = 100;
  ofVertex(p0x, p0y);
  float p1x = p0x + sin(theta/2) * s;
  float p1y = p0y + +cos(theta/2) * s;
  cout << "p0: (" <<  p0x << ", " << p0y << "), p1: (" << p1x << ", " << p1y << "), " ;
  // cout << "sin(90) = " << sin(TWO_PI/4) << endl;
  ofVertex(p1x, p1y);
  float p2x = p0x + -sin(theta/2) * s;
  float p2y = p0y + +cos(theta/2) * s;
  cout << "p2: (" << p2x << ", " << p2y << ")" << endl;
  cout << "What are you!: sin(" << theta << "/2) = " << sin(theta/2) << endl;
  ofVertex(p2x,p2y);
  ofEndShape();
  
}

void drawStartBurst(float in1, float in2) {
	float xPct = (float)(in1); // / (float)(ofGetWidth());
	float yPct = (float)(in2); // / (float)(ofGetHeight());
	int nTips = 5 + xPct * 60;
	int nStarPts = nTips * 2;
	float angleChangePerPt = TWO_PI / (float)nStarPts;
	float innerRadius = 0 + yPct*80;
	float outerRadius = 80;
	float origx = 525;
	float origy = 100;
	float angle = 0;

	ofSetHexColor(0x847b7d);
	ofBeginShape();
	for (int i = 0; i < nStarPts; i++){
		if (i % 2 == 0) {
			// inside point:
			float x = origx + innerRadius * cos(angle);
			float y = origy + innerRadius * sin(angle);
			ofVertex(x,y);
		} else {
			// outside point
			float x = origx + outerRadius * cos(angle);
			float y = origy + outerRadius * sin(angle);
			ofVertex(x,y);
		}
		angle += angleChangePerPt;
	}
	ofEndShape();
	}

// void ofApp::printProperties(ofImage receivedImage){
	
	// cout << "receivedImage " << receivedImage.current_msg_string << "\n"; }

//--------------------------------------------------------------
void ofApp::draw(){

	drawStartBurst(numPoints, shapeRad);

    drawTriangle(scale, theta);

	ofSetHexColor(0xf9043a);
	string buf;
	buf = "listening for osc messages on port" + ofToString(PORT);
	ofDrawBitmapString(buf, 10, 20);

    
    if(receivedImage.getWidth() > 0){
        //ofDrawBitmapString("Image:", 10, 160);
        receivedImage.draw(10, 180);
				//printProperties(receivedImage);
		}
		
		
	// draw mouse state
	buf = "mouse: " + ofToString(mouseX, 4) +  " " + ofToString(mouseY, 4);
	ofDrawBitmapString(buf, 430, 20);
	ofDrawBitmapString(mouseButtonState, 580, 20);

	for(int i = 0; i < NUM_MSG_STRINGS; i++){
		ofDrawBitmapString(msg_strings[i], 10, 40 + 15 * i);
	}

  


}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
// edited to effect the shape drawn
void ofApp::mouseMoved(int x, int y){
	// for (int i = 0; i < nCurveVertices; i++){
	// 	float diffx = x - curveVertices[i].x;
	// 	float diffy = y - curveVertices[i].y;
	// 	float dist = sqrt(diffx*diffx + diffy*diffy);
	// 	if (dist < curveVertices[i].radius){
	// 		curveVertices[i].bOver = true;
	// 	} else {
	// 		curveVertices[i].bOver = false;
	// 	}	
	// }
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
	// 	for (int i = 0; i < nCurveVertices; i++){
	// 	if (curveVertices[i].bBeingDragged == true){
	// 		curveVertices[i].x = x;
	// 		curveVertices[i].y = y;
	// 	}
	// }
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){

}
