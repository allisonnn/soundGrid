//
//  eachGrid.cpp
//  soundGrid
//
//  Created by Xiao Chen on 2016-11-02.
//
//

#include "eachGrid.h"

EachGrid::EachGrid() {
}

void EachGrid::setup(int _pos) {
    float outerSide = 2 * margin + side;
    
    gridPos = _pos;
    rectPath.moveTo(128, 0);
    rectPath.lineTo(128 + outerSide, 0);
    rectPath.lineTo(128 + outerSide, outerSide);
    rectPath.lineTo(128, outerSide);
    rectPath.close();
    rectPath.setFillColor(ofColor::blue);
    rectPath.setFilled(true);
    rectPath.setStrokeWidth(0);
}

void EachGrid::update() {
    
}

void EachGrid::draw() {
    float outerSide = 2 * margin + side;
    
    //----------DRAW THE RECTANGLE BY PATH------------//
    
    
    
    //----------DRAW THE OUTER RECTANGLE------------//
    
//    if (gridPos%3 == 0) {
//        outerRect.x = 128;
//    } else if (gridPos%3 == 1) {
//        outerRect.x = 128 + outerSide;
//    } else {
//        outerRect.x = 128 + outerSide * 2;
//    }
//    
//    if (gridPos <= 2) {
//        outerRect.y = 0;
//    } else if (gridPos <= 5) {
//        outerRect.y = outerSide;
//    } else {
//        outerRect.y = outerSide * 2;
//    }
//
//    outerRect.width = 2 * margin + side;
//    outerRect.height = 2 * margin + side;
//    
//    ofSetColor(255, 0, 0);
//    ofDrawRectangle(outerRect);
    
    //----------DRAW THE INTERNAL RECTANGLE------------//
    ofRectangle internalRect;
    
    if (gridPos%3 == 0) {
        internalRect.x = 128 + margin;
        if (gridPos == 0) {
            rectPath.draw();
        }
    } else if (gridPos % 3 == 1) {
        internalRect.x = 128 + outerSide + margin;
    } else {
        internalRect.x = 128 + outerSide * 2 + margin;
    }
    
    if (gridPos <= 2) {
        internalRect.y = margin;
    } else if (gridPos <= 5) {
        internalRect.y = outerSide + margin;
    } else {
        internalRect.y = outerSide * 2 + margin;
    }
    
    originalPos.x = internalRect.x;
    originalPos.y = internalRect.y;
    
    internalRect.width = side;
    internalRect.height = side;
    
    ofSetColor(255, 0, 0);
    
    ofDrawRectangle(internalRect);
}

int EachGrid::isIn(ofVec2f point) {
    if (point.x >= originalPos.x && point.x <= originalPos.x + side && point.y >= originalPos.y && point.y <= originalPos.y + side) {
        rectPath.setFillColor(ofColor::green);
    } else {
        rectPath.setFillColor(ofColor::blue);
    }
    return gridPos;
}
