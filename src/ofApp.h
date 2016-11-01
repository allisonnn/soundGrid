#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxKinect.h"
#include "ofxKinectProjectorToolkit.h"
#include "ofxOsc.h"

#define PROJECTOR_RESOLUTION_X 1280
#define PROJECTOR_RESOLUTION_Y 1024
#define PORT 8001
#define IP_ADDRESS "127.0.0.1"

class ofApp : public ofBaseApp {
public:
    
    void setup();
    void update();
    void draw();
    void drawSecondWindow(ofEventArgs& args);
    void exit();
    
    void drawPointCloud();
    
    void keyPressed(int key);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    
    void sendMessage(string m);
    void logPos(Point p);
    
    ofxKinect kinect;
    ofxKinectProjectorToolkit kpt;
    
    ofxCvColorImage colorImg;
    
    ofxCvGrayscaleImage grayImage; // grayscale depth image
    ofxCvGrayscaleImage grayThreshNear; // the near thresholded image
    ofxCvGrayscaleImage grayThreshFar; // the far thresholded image
    
    ofxCvContourFinder contourFinder;
    
    bool bThreshWithOpenCV;
    
    int nearThreshold;
    int farThreshold;
    
    int angle;
    
    ofPoint point;
    
    //sounds
    ofSoundPlayer planet0;
    ofSoundPlayer planet1;
    ofSoundPlayer planet2;
    
    //osc
    ofxOscSender sender;
    ofxOscReceiver receiver;
};
