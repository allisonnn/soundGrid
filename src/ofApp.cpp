#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
    ofSetLogLevel(OF_LOG_VERBOSE);
    
    // enable depth->video image calibration
    kinect.setRegistration(true);
    
    kinect.init();
    kinect.open();
    
    // print the intrinsic IR sensor values
    if(kinect.isConnected()) {
        ofLogNotice() << "sensor-emitter dist: " << kinect.getSensorEmitterDistance() << "cm";
        ofLogNotice() << "sensor-camera dist:  " << kinect.getSensorCameraDistance() << "cm";
        ofLogNotice() << "zero plane pixel size: " << kinect.getZeroPlanePixelSize() << "mm";
        ofLogNotice() << "zero plane dist: " << kinect.getZeroPlaneDistance() << "mm";
    }
    
    colorImg.allocate(kinect.width, kinect.height);
    grayImage.allocate(kinect.width, kinect.height);
    grayThreshNear.allocate(kinect.width, kinect.height);
    grayThreshFar.allocate(kinect.width, kinect.height);
    
    nearThreshold = 140.25;
    farThreshold = 128.775;
    bThreshWithOpenCV = true;
    
    ofSetFrameRate(60);
    
    // zero the tilt on startup
    angle = 3;
    kinect.setCameraTiltAngle(angle);
    
    kpt.loadCalibration("calibration_data/calibration.xml");
    
    //sounds
//    planet0.load("sounds/1085.mp3");
//    planet1.load("sounds/1085.mp3");
//    planet2.load("sounds/Violet.mp3");
    
//    for (int i = 0; i < planet0.size(); i++) {
//        planet0[i].load("sounds/" + to_string(i) + ".mp3");
//    }
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            sounds[i][j].load("sounds/" + to_string(i) + "_" + to_string(j) + ".mp3");
            sounds[i][j].setLoop(true);
        }
    }
    
    //osc
    sender.setup(IP_ADDRESS, PORT);
    receiver.setup(12000);
    
    // grid
    for(int i=0; i<NGRIDS; i++){
        grids[i].setup(i);
    }
    
    // fonts
    font.load("fonts/BEBAS.ttf", 42);
    
    // state
    state = "start";
    start.load("sprites/start.png");
}

//--------------------------------------------------------------
void ofApp::update()
{
    
    ofBackground(100, 100, 100);
    
    kinect.update();
    ofSoundUpdate();
    
    // there is a new frame and we are connected
    if(kinect.isFrameNew()) {
        
        // load grayscale depth image from the kinect source
        grayImage.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height);
        
        grayThreshNear = grayImage;
        grayThreshFar = grayImage;
        grayThreshNear.threshold(nearThreshold, true);
        grayThreshFar.threshold(farThreshold);
        cvAnd(grayThreshNear.getCvImage(), grayThreshFar.getCvImage(), grayImage.getCvImage(), NULL);
        
        // update the cv images
        grayImage.flagImageChanged();
        
        contourFinder.setMinArea(1000);
        contourFinder.setMaxArea(70000);
        contourFinder.setSortBySize(true);
        contourFinder.findContours(grayImage);

    }
    
    //osc receiver
    while (receiver.hasWaitingMessages()) {
        ofxOscMessage receivedMessage;
        receiver.getNextMessage(&receivedMessage);
        
        if (receivedMessage.getAddress() == "/digital/8") {
            int digital = receivedMessage.getArgAsInt(0);
            ofLogNotice() << digital;
        }
    }
    
    // grids
    for (int i = 0; i < NGRIDS; i++ ) {
        grids[i].update();
    }
    
}

void ofApp::sendMessage(string m) {
    ofxOscMessage message;
    message.setAddress("/songName");
    message.addStringArg(m);
    sender.sendMessage(message);
    ofLogNotice() << m;
}

//--------------------------------------------------------------
void ofApp::draw()
{
    ofSetColor(255, 255, 255);
    

    // draw from the live kinect
    kinect.drawDepth(kinect.width + 10, 0, kinect.width/2, kinect.height/2);
    kinect.draw(kinect.width + 10, kinect.height/2 + 10, kinect.width/2, kinect.height/2);
    
    //grayImage.draw(0, 0, kinect.width, kinect.height);
        
    //https://forum.openframeworks.cc/t/opencv-problem-finding-blob-centroid/16949/4
    grayImage.draw(0, 0);
    contourFinder.draw();
    
    
    // draw instructions
    ofSetColor(255, 255, 255);
    stringstream reportStream;
    
    if(kinect.hasAccelControl()) {
        reportStream << "accel is: " << ofToString(kinect.getMksAccel().x, 2) << " / "
        << ofToString(kinect.getMksAccel().y, 2) << " / "
        << ofToString(kinect.getMksAccel().z, 2) << endl;
    } else {
        reportStream << "Note: this is a newer Xbox Kinect or Kinect For Windows device," << endl
        << "motor / led / accel controls are not currently supported" << endl << endl;
    }
    
//    reportStream << "using opencv threshold = " << bThreshWithOpenCV <<" (press spacebar)" << endl
//    << "set near threshold " << nearThreshold << " (press: + -)" << endl
//    << "set far threshold " << farThreshold << " (press: < >) num blobs found " << contourFinder.nBlobs
//    << ", fps: " << ofGetFrameRate() << endl
//    << "press c to close the connection and o to open it again, connection is: " << kinect.isConnected() << endl;
    
    if(kinect.hasCamTiltControl()) {
        reportStream << "press UP and DOWN to change the tilt angle: " << angle << " degrees" << endl
        << "press 1-5 & 0 to change the led mode" << endl;
    }
    
    ofDrawBitmapString(reportStream.str(), 20, 652);
}

//--------------------------------------------------------------
void ofApp::drawGroundWindow (ofEventArgs & args)
{
    ofSetBackgroundColor(0, 0, 0);
    
    if (state == "start") {
        start.draw(364, 236, 236, 236);
    } else if (state == "play") {
        for(int i=0; i<NGRIDS; i++){
            grids[i].draw();
        }
    
        RectTracker& tracker = contourFinder.getTracker();
        for(int i = 0; i < contourFinder.size(); i++) {
            vector<cv::Point> points = contourFinder.getContour(i);
            int label = contourFinder.getLabel(i);
            ofPoint center = toOf(contourFinder.getCenter(i));
            int age = tracker.getAge(label);
    
            ofSetColor(ofColor::green);
            ofVec3f worldPoint = kinect.getWorldCoordinateAt(center.x, center.y);
            ofVec2f projectedPoint = kpt.getProjectedPoint(worldPoint);
            ofDrawCircle(GROUND_PROJECTOR_RESOLUTION_X * projectedPoint.x, GROUND_PROJECTOR_RESOLUTION_Y * projectedPoint.y, 50);
    
            ofVec2f point = ofVec2f (projectedPoint.x * GROUND_PROJECTOR_RESOLUTION_X, projectedPoint.y * GROUND_PROJECTOR_RESOLUTION_Y);
            checkPoint(point);
        }
    
    }
    
    //=======UNCOMMENT THIS PART TO TEST RESPONDING GRIDS========
    //drawDot();
    
}

void ofApp::drawDot()
{
    int x = ofGetMouseX();
    int y = ofGetMouseY();
    
    // TODO:: why uncomment it will cover the screen
    //ofSetColor(0, 0, 230);
    ofDrawCircle(x, y, 5);
    checkPoint(ofVec2f (x, y));
}

//--------------------------------------------------------------
void ofApp::exit()
{
    kinect.setCameraTiltAngle(3); // zero the tilt on exit
    kinect.close();
}

//--------------------------------------------------------------
void ofApp::exitGroundWindow(ofEventArgs &args)
{
    
}

//--------------------------------------------------------------
void ofApp::keyPressed (int key)
{
    switch (key) {
        case ' ':
            bThreshWithOpenCV = !bThreshWithOpenCV;
            break;
            
        case '>':
        case '.':
            farThreshold ++;
            if (farThreshold > 255) farThreshold = 255;
            break;
            
        case '<':
        case ',':
            farThreshold --;
            if (farThreshold < 0) farThreshold = 0;
            break;
            
        case '+':
        case '=':
            nearThreshold ++;
            if (nearThreshold > 255) nearThreshold = 255;
            break;
            
        case '-':
            nearThreshold --;
            if (nearThreshold < 0) nearThreshold = 0;
            break;
            
        case 'w':
            kinect.enableDepthNearValueWhite(!kinect.isDepthNearValueWhite());
            break;
            
        case 'o':
            kinect.setCameraTiltAngle(angle); // go back to prev tilt
            kinect.open();
            break;
            
        case 'c':
            kinect.setCameraTiltAngle(0); // zero the tilt
            kinect.close();
            break;
            
        case '1':
            kinect.setLed(ofxKinect::LED_GREEN);
            break;
            
        case '2':
            kinect.setLed(ofxKinect::LED_YELLOW);
            break;
            
        case '3':
            kinect.setLed(ofxKinect::LED_RED);
            break;
            
        case '4':
//            kinect.setLed(ofxKinect::LED_BLINK_GREEN);
            sendMessage("hahahahah");
            break;
            
        case '5':
//            kinect.setLed(ofxKinect::LED_BLINK_YELLOW_RED);
            ofLogNotice() << "QQQQQQ" << kinectPoint;
            break;
            
        case '0':
            kinect.setLed(ofxKinect::LED_OFF);
            break;
            
        case OF_KEY_UP:
            angle++;
            if(angle>30) angle=30;
            kinect.setCameraTiltAngle(angle);
            break;
            
        case OF_KEY_DOWN:
            angle--;
            if(angle<-30) angle=-30;
            kinect.setCameraTiltAngle(angle);
            break;
    }
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y)
{
    
}

void ofApp::mouseMovedGroundWindow (ofMouseEventArgs& args)
{

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h)
{
    
}

//--------------------------------------------------------------
void ofApp::checkPoint(ofVec2f point)
{
    int cp;
    float dt;

    
    if (state == "start") {
        
        if (point.x >= (128 + 236 + 20) && point.x <= (128 + 236 + 20 + 216) && point.y >= (236 + 20) && point.y <= (236 + 20 + 216)) {
            cp = 1;
        } else {
            cp = 0;
        }
        
        if (originalPosition != cp) {
            originalPosition = cp;
            
            if (cp == 1) {
                startTime = ofGetElapsedTimef();
            }
            
        } else if (cp == 1 && originalPosition == cp) {
            dt = ofGetElapsedTimef() - startTime;
            if (dt >= TIME_DELAY) {
                state = "play";
            }
        }

    } else if (state == "play") {
        for (int i = 0; i < NGRIDS; i++) {
            cp = grids[i].getCurrentPosition(point);
            
            if (cp >= 0) {
                currentPosition = cp;
            }
        }
        
        // Leave grids reset timer
        if (cp == -2) {
            for (int i = 0; i < NGRIDS; i++) {
                grids[i].reset();
            }
            stopSound();
            originalPosition = -2;
            
            // New timer
        } else if (originalPosition != currentPosition) {
            stopSound();
            startTime = ofGetElapsedTimef();
            originalPosition = currentPosition;
            
            // Still there
        } else if (originalPosition == currentPosition) {
            dt = ofGetElapsedTimef() - startTime;
            
            if(dt >= TIME_DELAY) {
                playSound();
                grids[currentPosition].light();
            }
            
        }
    }
}

void ofApp::playSound()
{
    if (!sounds[currentPosition][0].isPlaying()) {
        sounds[currentPosition][0].play();
    }
}

void ofApp::stopSound()
{
    for (int i = 0; i < 5; i ++) {
        for (int j = 0; j < 5; j++) {
            if (sounds[i][j].isPlaying()) {
                sounds[i][j].stop();
            }
        }
    }
}