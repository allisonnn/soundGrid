//
//  eachGrid.h
//  soundGrid
//
//  Created by Xiao Chen on 2016-11-02.
//
//

#ifndef _eachGrid
#define _eachGrid

#include "ofMain.h"

class EachGrid {
    
public:
    
    void setup(int _pos);	// setup method, use this to setup your object's initial state
    void generateGrids();
    void update();  // update method, used to refresh your objects properties
    void draw();    // draw method, this where you'll do the object's drawing
    int isIn(ofVec2f point);
    
    // variables
    float x;        // position
    float y;
    float speedY;   // speed and direction
    float speedX;
    int dim;        // size
    ofColor color;  // color using ofColor type
    
    EachGrid();
    
//private:
    
    float margin = 20;
    float side = 216;
    int gridPos;
    
    ofRectangle outerRect;
    ofPath rectPath;
    ofPath internalPath;
    ofVec2f originalPos;
};

#endif /* eachGrid_h */
