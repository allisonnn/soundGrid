#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
    ofSetLogLevel(OF_LOG_VERBOSE);
    
    // enable depth->video image calibration
    kinect.setRegistration(true);
    
    kinect.init();
    //kinect.init(true); // shows infrared instead of RGB video image
    //kinect.init(false, false); // disable video image (faster fps)
    
    kinect.open();		// opens first available kinect
    //kinect.open(1);	// open a kinect by id, starting with 0 (sorted by serial # lexicographically))
    //kinect.open("A00362A08602047A");	// open a kinect using it's unique serial #
    
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
    
    nearThreshold = 132;
    farThreshold = 128;
    bThreshWithOpenCV = true;
    
    ofSetFrameRate(60);
    
    // zero the tilt on startup
    angle = 3;
    kinect.setCameraTiltAngle(angle);
    
    kpt.loadCalibration("calibration_data/calibration.xml");
    
    //sounds
    planet0.load("sounds/1085.mp3");
    planet1.load("sounds/1085.mp3");
    planet2.load("sounds/Violet.mp3");
    
    //osc
    sender.setup(IP_ADDRESS, PORT);
    receiver.setup(12000);
    
    // grid
    for(int i=0; i<NGRIDS; i++){
        grid[i].setup(i);
    }
}

//--------------------------------------------------------------
void ofApp::update() {
    
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
        contourFinder.setMaxArea(7000);
        contourFinder.findContours(grayImage);
        
        // find contours which are between the size of 20 pixels and 1/3 the w*h pixels.
        // also, find holes is set to true so we will get interior contours as well....
        //contourFinder.findContours(grayImage, 50, (kinect.width*kinect.height)/25, 1, false);

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
        grid[i].update();
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
void ofApp::draw() {
    
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
    
    //analyze position
    
//    if (contourFinder.blobs.size() > 0) {
//        kinectPoint = contourFinder.blobs[0].centroid;
//        ofSetColor(255, 0, 0);
//        ofDrawCircle(kinectPoint, 5.0);
//        
//        ofVec3f worldPoint = kinect.getWorldCoordinateAt(kinectPoint.x, kinectPoint.y);
//        ofVec2f projectedPoint = kpt.getProjectedPoint(worldPoint);
//        
//        ofLogNotice() << kinectPoint;
//        
//        for (int i = 0; i < NGRIDS; i++) {
//            int nowGrid = grid[i].isIn(ofVec2f (projectedPoint.x * 1024, projectedPoint.y * 768));
//        }
//    }
}

//--------------------------------------------------------------
void ofApp::drawSecondWindow (ofEventArgs & args) {
    ofSetBackgroundColor(0, 0, 0);
    for(int i=0; i<NGRIDS; i++){
        grid[i].draw();
    }
    
    RectTracker& tracker = contourFinder.getTracker();
    for(int i = 0; i < contourFinder.size(); i++) {
        vector<cv::Point> points = contourFinder.getContour(i);
        int label = contourFinder.getLabel(i);
        ofPoint center = toOf(contourFinder.getCenter(i));
        int age = tracker.getAge(label);
        
        // map contour using calibration and draw to main window
//        ofBeginShape();
//        ofFill();
        ofSetColor(ofColor::green);
//        for (int j=0; j<points.size(); j++) {
            ofVec3f worldPoint = kinect.getWorldCoordinateAt(center.x, center.y);
            ofVec2f projectedPoint = kpt.getProjectedPoint(worldPoint);
            ofDrawCircle(1024 * projectedPoint.x, 768 * projectedPoint.y, 50);
        for (int i = 0; i < NGRIDS; i++) {
            ofVec2f point = ofVec2f (projectedPoint.x * 1024, projectedPoint.y * 768);
            if (point.x >= 128
                && point.x <= 128 + grid[i].side
                && point.y >= 0
                && point.y <= 0 + grid[i].side) {
                grid[i].rectPath.setFillColor(ofColor::green);
                ofLogNotice() << "XXXXXXXXXXXXXXXXXXXXXXXXXX";
            } else {
                grid[i].rectPath.setFillColor(ofColor::blue);
                //ofLogNotice() << "JJJJJJJJJJJJ";
            }
        }
//        }
//        ofEndShape();
        return;
    }
}

//--------------------------------------------------------------
void ofApp::exit() {
    kinect.setCameraTiltAngle(3); // zero the tilt on exit
    kinect.close();
}

//--------------------------------------------------------------
void ofApp::keyPressed (int key) {
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
void ofApp::mouseDragged(int x, int y, int button)
{
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button)
{
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button)
{
    
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h)
{
    
}
