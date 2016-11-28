#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxCv.h"
#include "ofxKinect.h"
#include "ofxKinectProjectorToolkit.h"
#include "ofxOsc.h"
#include "Grid.h"

#define GROUND_PROJECTOR_RESOLUTION_X 1024
#define GROUND_PROJECTOR_RESOLUTION_Y 768
#define FRONT_PROJECTOR_RESOLUTION_X 1024
#define FRONT_PROJECTOR_RESOLUTION_Y 768
#define NGRIDS 9
#define TIME_DELAY 1
#define TIME_ALERT 3
#define NPLANETS 9

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
    void exitGroundWindow(ofEventArgs& args);
    
    void drawPointCloud();
    
    void keyPressed(int key);
    void mouseMoved(int x, int y);
    void mouseMovedGroundWindow(ofMouseEventArgs& args);
    void windowResized(int w, int h);
    
    void sendMessage(string m);
    
    ofxKinect kinect;
    ofxKinectProjectorToolkit kpt;
    ofxCv::ContourFinder contourFinder;
    
    ofxCvColorImage colorImg;
    
    ofxCvGrayscaleImage grayImage; // grayscale depth image
    ofxCvGrayscaleImage grayThreshNear; // the near thresholded image
    ofxCvGrayscaleImage grayThreshFar; // the far thresholded image
    
    bool bThreshWithOpenCV;
    
    int nearThreshold;
    int farThreshold;
    
    int angle;
    
    ofPoint kinectPoint;
    ofPoint projectorPoint;
    
    //sounds
    ofSoundPlayer sounds [NPLANETS];
    
    //osc
    ofxOscSender sender;
    ofxOscReceiver receiver;
    
    //grid
    Grid grids[NGRIDS];
    ofImage gridBG;
    
    ofPath cursor;
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
