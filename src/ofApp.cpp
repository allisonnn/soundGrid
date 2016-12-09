#include "ofApp.h"

const int N = 360;		//Number of bands in spectrum
float spectrum[ N ];	//Smoothed spectrum values

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

    nearThreshold = NEARTHRESHOLD;
    farThreshold = FARTHRESHOLD;

    ofSetFrameRate(60);

    kpt.loadCalibration("calibration_data/calibration.xml");

    //sounds
    for (int i = 0; i < NPLANETS; i++) {
        
            sounds[i].load("sounds/" + to_string(i) + ".mp3");
            if (i != 4) {
                sounds[i].setLoop(true);
            } else {
                sounds[i].setLoop(false);
            }
    }

    // grid
    gridBG.load("sprites/gridBackground.png");
    front_grid.load("sprites/front_grid.png");
    front_glow.load("sprites/front_glow.png");
    headline.load("sprites/Headline.png");
    instruction.load("sprites/Instruction.png");
    code.load("sprites/ProbeCode.png");
    radioWave.load("sprites/Radiowave.png");
    frame.load("sprites/Frame.png");
    ring.load("sprites/Ring.png");
    star_back.load("sprites/Stars.png");
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
    
    // not start showing animation on front screen
    animation = false;
    for (int i = 0; i < 9; i++) {
        video[i].load("videos/" + to_string(i) + ".mp4");
        planet_name[i].load("sprites/planets_name/" + to_string(i) + ".png");
    }
    //load the dot animation file
    for (int i = 0; i < 41; i++)
    {
        dot[i].load("sprites/dot/" + to_string(i) + ".png");
    }
    //set currentposition = 4 to avoid show the first vedio at the beginning
    currentPosition = 4;
    video[currentPosition].play();
    probe.load("videos/Probe.mp4");
    probe.play();
    up = false;
    timer = 0;
    dotCur = 0;

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
        float x = ofMap(i, 20, 0, 420, 420 + 280);
        float y = ofMap(spectrum[i], 0, 1, 334, 334 + 100);
        if (x >= 420 && x <= 420 + 20 ) {
            y = 384;
        }
        if (x >= 420 && x <= 420 + 280) {
            waveform.addVertex(x, y);
        }
    }

    //update video
    video[currentPosition].update();
    probe.update();
    
    //update timer for flash instruction
    if(state == "play" && currentPosition == 4)
    {
        if(currentPosition != originalPosition)
        {
            timer = 0;
            up = false;
        }
        blinkTimer();
        
    }
    if(currentPosition != 4)
    {
        up = false;
    }
    
    // update dot animation
    if(state == "play" && currentPosition != 4)
    {
        if(currentPosition != originalPosition)
        {
            dotCur = 0;
        }
        else if(currentPosition == originalPosition)
        {
            if(dotCur < 40)
            {
                dotCur++;
                
            }
            else
            {
                dotCur = 0;
            }
        }
    }
    
    

}

//--------------------------------------------------------------
void ofApp::draw()
{
    ofSetColor(255, 255, 255);


    // draw from the live kinect
    kinect.drawDepth(kinect.width + 10, 0, kinect.width/2, kinect.height/2);
    kinect.draw(kinect.width + 10, kinect.height/2 + 10, kinect.width/2, kinect.height/2);

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

    reportStream << "set near threshold " << nearThreshold << " (press: + -)" << endl
    << "set far threshold " << farThreshold << " (press: < >) " << endl
    << "press c to restart " << endl;

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
    

    //restart condition
    if (stillWaiting && state == "play") {
        if (ofGetElapsedTimef() - startTimeForNoContour >= 5) {
            state = "start";
        } else {
            currentPosition = 4;
        }
    } else if (stillWaiting == false && state == "play" && currentPosition < 0) {
        startTimeForNoContour = ofGetElapsedTimef();
        stillWaiting = true;
    }

    
    countTimerForAlert();
    for(int i = 0; i < contourFinder.size(); i++) {
        vector<cv::Point> points = contourFinder.getContour(i);
        int label = contourFinder.getLabel(i);
        ofPoint center = toOf(contourFinder.getCenter(i));
        int age = tracker.getAge(label);

        ofVec3f worldPoint = kinect.getWorldCoordinateAt(center.x, center.y);
        ofVec2f projectedPoint = kpt.getProjectedPoint(worldPoint);

        ofVec2f point = ofVec2f (projectedPoint.x * GROUND_PROJECTOR_RESOLUTION_X, projectedPoint.y * GROUND_PROJECTOR_RESOLUTION_Y);
        checkPoint(point);
        return;
    }


    //=======UNCOMMENT THIS PART TO TEST RESPONDING GRIDS========
    drawDot();
}

void ofApp::drawDot()
{
    int x = ofGetMouseX();
    int y = ofGetMouseY();

    ofDrawCircle(x, y, 5);
    checkPoint(ofVec2f (x, y));
}

void ofApp::drawFrontWindow(ofEventArgs& args)
{
    //add animation part according to the currentposition
    ofBackground( 0, 0, 0, 128 );
    star_back.draw(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
    
    //before play state
    frame.draw(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
    probe.draw(350, 314, 70, 140);
    front_grid.draw(700, 284, 200, 200);
    headline.draw(290, 95, 430, 40);
    ring.draw(100, 284, 200, 200);
    //give player instruction to step on grid
    if(up == true)
    {
        instruction.draw(300, 125, 400, 50 );
        
    }
    
    //after step on any grid
    if(animation == true)
    {
        //avoid the repeat playing static video without animation
        if(!video[currentPosition].isPlaying())
        {
            
            video[currentPosition].play();
            
        }
        //put draw outside to avoid disppear
        video[currentPosition].draw(100, 284, 200, 200);
        planet_name[currentPosition].draw(150, 550, 100, 25);
        front_glow.draw(700 + ((currentPosition % 3) * 200 / 3), 284 + (floor(currentPosition / 3) * 200 / 3) , 200 / 3, 200/ 3 );
        if(currentPosition != 4)
        {
            radioWave.draw(250, 284, 150, 200);
            code.draw(375, 553, 410, 86);
            dot[dotCur].draw(0,0,ofGetWindowWidth(), ofGetWindowHeight());
            ofSetColor(0, 255, 0);
            waveform.draw();
            ofSetColor(255, 255, 255);
        }
        
    }
    
    if(currentPosition == 4)
    {
        front_glow.draw(700 + 200 / 3, 284 + 200 / 3, 200 / 3, 200 / 3);
        
    }
}

//--------------------------------------------------------------
void ofApp::exit()
{
    kinect.close();
}

//--------------------------------------------------------------
void ofApp::keyPressed (int key)
{
    switch (key) {
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

        case 'c':
            state = "start";
            played = false;
            break;
    }
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
                stillWaiting = false;
                currentPosition = cp;
            } else {
                
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
            playSound();
            grids[currentPosition].light();
            animation = true;
        }
    }
}

void ofApp::playSound()
{
    if (!sounds[currentPosition].isPlaying()) {
        if (currentPosition == 4) {
            if (played == false) {
                sounds[currentPosition].play();
                played = true;
            }
        } else {
            sounds[currentPosition].play();
        }
    }
}

void ofApp::stopSound()
{
    for (int i = 0; i < NPLANETS; i ++) {
        if (sounds[i].isPlaying()) {
            sounds[i].stop();
        }
    }
}

void ofApp::countTimerForAlert()
{
    
    float curTime = ofGetElapsedTimef();
    float dt = curTime - startTimeForAlertTimer;
    float aniTime = 0.5;
    int r;

    if (dt >= TIME_ALERT) {
        //Glowing randomly!!
        r = (int)floor(dt / aniTime);
        if (r != oldRForAlert) {
            do {
                posForAlert = ofRandom(0, NGRIDS);
            } while (posForAlert == currentPosition || posForAlert == 4);
            oldRForAlert = r;
        }
        grids[posForAlert].glow(curTime);
    }
}

//not put any local varible to time, will cause problem
void ofApp::blinkTimer()
{
    
    //timer
    if(timer <= 0)
    {
        up = true;
    }
    if (timer >= 1)
    {
        up = false;
    }
    if(up == true)
    {
        timer += ofGetLastFrameTime() * 10;
    }
    if (up == false)
    {
        timer -= ofGetLastFrameTime() * 10;
    }
}
