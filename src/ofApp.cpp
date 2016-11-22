#include "ofApp.h"

const int N = 360;		//Number of bands in spectrum
float spectrum[ N ];	//Smoothed spectrum values
float Rad = 500;		//Cloud raduis parameter
float Vel = 0.1;		//Cloud points velocity parameter
int bandRad = 2;		//Band index in spectrum, affecting Rad value
int bandVel = 100;		//Band index in spectrum, affecting Vel value

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

    nearThreshold = 230;
    farThreshold = 128.75;
    bThreshWithOpenCV = true;

    ofSetFrameRate(60);

    // zero the tilt on startup
    //angle = 3;
    //kinect.setCameraTiltAngle(angle);

    kpt.loadCalibration("calibration_data/calibration.xml");

    //sounds
    for (int i = 0; i < NPLANETS; i++) {
        for (int j = 0; j < NTRACKS; j++) {
            sounds[i][j].load("sounds/" + to_string(i) + "_" + to_string(j) + ".mp3");
            if (i != 4) {
                sounds[i][j].setLoop(true);
            } else {
                sounds[i][j].setLoop(false);
            }
        }
    }

    //osc
    sender.setup(IP_ADDRESS, PORT);
    receiver.setup(12000);

    // grid
    gridBG.load("sprites/gridBackground.png");
    for(int i=0; i<NGRIDS; i++){
        grids[i].setup(i);
    }

    // state
    state = "start";
    start.load("sprites/start.png");


    // sound
    //Set spectrum values to 0
    for (int i=0; i<N; i++) {
        spectrum[i] = 0.0f;
    }

    //Initialize points offsets by random numbers
//    for ( int j=0; j<n; j++ ) {
//        tx[j] = ofRandom( 0, 1000 );
//        ty[j] = ofRandom( 0, 1000 );
//    }
    // not start showing animation on front screen
    animation = false;
    for (int i = 0; i < NGRIDS; i++) {
        video[i].load("videos/" + to_string(i) + ".mp4");
    }
    //set currentposition = 4 to avoid show the first vedio at the beginning
    currentPosition = 4;

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

    // sound
    //Get current spectrum with N bands
    float *val = ofSoundGetSpectrum( N );
    //We should not release memory of val,
    //because it is managed by sound engine

    //Update our smoothed spectrum,
    //by slowly decreasing its values and getting maximum with val
    //So we will have slowly falling peaks in spectrum
    for ( int i=0; i<N; i++ ) {
        spectrum[i] *= 0.97;	//Slow decreasing
        spectrum[i] = max( spectrum[i], val[i] );
    }
    
    waveform.clear();
    for(size_t i = 0; i < N; i++) {
        float x = ofMap(i, 0, 50, 0, FRONT_PROJECTOR_RESOLUTION_X);
        float y = ofMap(spectrum[i], 0, 1, FRONT_PROJECTOR_RESOLUTION_Y/4, FRONT_PROJECTOR_RESOLUTION_Y/4*3);
        waveform.addVertex(x, y);
    }

    //update video
    video[currentPosition].update();
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
        start.draw(128 + 250, 250, 268, 268);
    } else if (state == "play") {
        gridBG.draw(128, 0);
        for(int i=0; i<NGRIDS; i++){
            grids[i].draw();
        }
    }

    RectTracker& tracker = contourFinder.getTracker();
    for(int i = 0; i < contourFinder.size(); i++) {
        vector<cv::Point> points = contourFinder.getContour(i);
        int label = contourFinder.getLabel(i);
        ofPoint center = toOf(contourFinder.getCenter(i));
        int age = tracker.getAge(label);

        //ofSetColor(ofColor::green);
        ofVec3f worldPoint = kinect.getWorldCoordinateAt(center.x, center.y);
        ofVec2f projectedPoint = kpt.getProjectedPoint(worldPoint);
        ofDrawCircle(GROUND_PROJECTOR_RESOLUTION_X * projectedPoint.x, GROUND_PROJECTOR_RESOLUTION_Y * projectedPoint.y, 50);
        //ofLog() << projectedPoint << endl;

        ofVec2f point = ofVec2f (projectedPoint.x * GROUND_PROJECTOR_RESOLUTION_X, projectedPoint.y * GROUND_PROJECTOR_RESOLUTION_Y);
        checkPoint(point);
        return;
    }


    //=======UNCOMMENT THIS PART TO TEST RESPONDING GRIDS========
    drawDot();

    countTimerForAlert();
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

void ofApp::drawFrontWindow(ofEventArgs& args)
{
    // visualize sound part
    for (int i = 0; i < NPLANETS; i++) {
        if (sounds[i][0].isPlaying()) {
            ofBackground( 255, 255, 255 );	//Set up the background

            //Draw spectrum
            ofSetColor( 0, 0, 0 );
//            float soundPos = sounds[i][0].getPosition();
//            
//            for (int j=0; j<N; j++) {
//                ofDrawRectangle( ofMap(soundPos, 0, 1, 0, FRONT_PROJECTOR_RESOLUTION_X) * ofMap(j, 0, N, 0, 1), ofMap(spectrum[j], 0, 1, 0, FRONT_PROJECTOR_RESOLUTION_Y), 3, 3 );
//            }
            waveform.draw();
        }
    }



//    //add animation part according to the currentposition
//    if(animation == true && currentPosition != 4)
//    {
//        if(!video[currentPosition].isPlaying())
//        {
//            ofBackground( 0, 0, 0, 128 );
//            video[currentPosition].play();
//
//        }
//
//
//    }
//
//    //draw has to be outside of condition otherwise only static pictures
//
//    //draw vedio
//    ofBackground( 0, 0 , 0, 128);
//    video[currentPosition].draw((currentPosition % 3) * (ofGetWindowWidth() / 3) + 50, floor(currentPosition / 3) * (ofGetWindowHeight() / 3) + 20);
//    //draw lines
//
//    ofSetColor(255,255,255);
//    ofFill();
//    ofDrawLine(0, ofGetWindowHeight() / 3, ofGetWindowWidth(), ofGetWindowHeight() / 3);
//    ofDrawLine(0, (ofGetWindowHeight() / 3) * 2, ofGetWindowWidth(), (ofGetWindowHeight() / 3) * 2);
//    ofDrawLine(ofGetWindowWidth() / 3, 0, ofGetWindowWidth() / 3, ofGetWindowHeight());
//    ofDrawLine((ofGetWindowWidth() / 3) * 2, 0, (ofGetWindowWidth() / 3) * 2, ofGetWindowHeight());
}

//--------------------------------------------------------------
void ofApp::exit()
{
    //kinect.setCameraTiltAngle(3); // zero the tilt on exit
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

        if (point.x >= (128 + 250) && point.x <= (128 + 250 + 268) && point.y >= 250 && point.y <= (250 + 268)) {
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
                startTimeForAlertTimer = ofGetElapsedTimef();
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
            animation = false;
            startTimeForAlertTimer = ofGetElapsedTimef();

            // New timer when jump into another different grid
        } else if (originalPosition != currentPosition) {
            stopSound();
            startTime = ofGetElapsedTimef();
            originalPosition = currentPosition;
            animation = false;
            startTimeForAlertTimer = ofGetElapsedTimef();

            // Still there
        } else if (originalPosition == currentPosition) {
            dt = ofGetElapsedTimef() - startTime;

            if(dt >= TIME_DELAY) {
                playSound();
                grids[currentPosition].light();
                animation = true;
            }

        }
    }
}

void ofApp::playSound()
{
    if (!sounds[currentPosition][0].isPlaying()) {
        if (currentPosition == 4) {
            if (played == false) {
                sounds[currentPosition][0].play();
                played = true;
            }
        } else {
            sounds[currentPosition][0].play();
        }
    }
}

void ofApp::stopSound()
{
    for (int i = 0; i < NPLANETS; i ++) {
        for (int j = 0; j < NTRACKS; j++) {
            if (sounds[i][j].isPlaying()) {
                sounds[i][j].stop();
            }
        }
    }
}

void ofApp::countTimerForAlert()
{
    
    float curTime = ofGetElapsedTimef();
    float dt = curTime - startTimeForAlertTimer;

    if (dt >= TIME_ALERT) {
        //Glowing randomly!!
        int pos;
        do {
            pos = ofRandom(0, NGRIDS);
        } while (pos == currentPosition || pos == 4);
        grids[pos].glow(curTime);
    }
}

