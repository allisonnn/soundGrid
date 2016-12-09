#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxCv.h"
#include "ofxKinect.h"
#include "ofxKinectProjectorToolkit.h"
#include "Grid.h"

#define GROUND_PROJECTOR_RESOLUTION_X 1024
#define GROUND_PROJECTOR_RESOLUTION_Y 768
#define FRONT_PROJECTOR_RESOLUTION_X 1024
#define FRONT_PROJECTOR_RESOLUTION_Y 768
#define NGRIDS 9
#define TIME_DELAY 1
#define TIME_ALERT 3
#define NPLANETS 9

#define NEARTHRESHOLD 230
#define FARTHRESHOLD 128.75

using namespace ofxCv;
using namespace cv;

class ofApp : public ofBaseApp {
public:
    
    void setup();
    void update();
    void draw();
    void drawGroundWindow(ofEventArgs& args);
    void drawFrontWindow(ofEventArgs& args);
    void exit();
    void keyPressed(int key);
    
    ofxKinect kinect;
    ofxKinectProjectorToolkit kpt;
    ofxCv::ContourFinder contourFinder;
    
    ofxCvColorImage colorImg;
    
    ofxCvGrayscaleImage grayImage; // grayscale depth image
    ofxCvGrayscaleImage grayThreshNear; // the near thresholded image
    ofxCvGrayscaleImage grayThreshFar; // the far thresholded image
    
    int nearThreshold;
    int farThreshold;
    
    ofPoint kinectPoint;
    ofPoint projectorPoint;
    
    //sounds
    ofSoundPlayer sounds [NPLANETS];
    
    //grid
    Grid grids[NGRIDS];
    ofImage gridBG;

    int currentPosition;
    int originalPosition;
    
    void checkPoint(ofVec2f point);
    void playSound();
    void stopSound();
    
    string state;
    
    //add for the animation part
    bool animation;
    ofVideoPlayer video[9];
    ofVideoPlayer probe;
    ofImage instruction;
    ofImage code;
    ofImage planet_name[9];
    ofImage headline;
    ofImage radioWave;
    ofImage frame;
    ofImage front_grid;
    ofImage front_glow;
    ofImage ring;
    ofImage star_back;
    
    ofImage dot[41];

    bool wave = false;
    bool up;
    void blinkTimer();
    float timer;
    int dotCur;

    ofTrueTypeFont font;
    
    ofImage start;
    
    void drawDot();
    
    float startTime;
    bool played = false;

private:
    ofPolyline waveform;
    float startTimeForAlertTimer;
    void countTimerForAlert();
    int posForAlert;
    int oldRForAlert;
    float startTimeForNoContour;
    bool stillWaiting = false;
};
