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
    gridPos = _pos;
    generateGrids();
}

void EachGrid::generateGrids() {
    float outerSide = 2 * margin + side;

    //----------CALCULATE THE PATH FOR OUTER RECTANGLE------------//
    rectPath.rectangle(128 + outerSide * (gridPos % 3), outerSide * (floor(gridPos / 3)), outerSide, outerSide);
    rectPath.setFillColor(ofColor::blue);
    rectPath.setFilled(true);
    rectPath.setStrokeWidth(0);
    
    //----------CALCULATE THE PATH FOR INTERNAL RECTANGLE------------//
    internalPath.rectangle(128 + outerSide * (gridPos % 3) + margin, outerSide * (floor(gridPos / 3)) + margin, side, side);
    internalPath.setFillColor(ofColor::red);
    internalPath.setFilled(true);
    internalPath.setStrokeWidth(0);
    
}

void EachGrid::update() {
    //rectPath.setFillColor(ofColor::green);
}

void EachGrid::draw() {
    //----------DRAW THE OUTER RECTANGLE BY PATH------------//
    rectPath.draw();
    
    //----------DRAW THE INTERNAL RECTANGLE------------//
    internalPath.draw();
}

int EachGrid::isIn(ofVec2f point) {
    ofLogNotice() << point;
    if (point.x >= originalPos.x
        && point.x <= originalPos.x + side
        && point.y >= originalPos.y
        && point.y <= originalPos.y + side) {
        rectPath.setFillColor(ofColor::green);
        //ofLogNotice() << "XXXXXXXXXXXXXXXXXXXXXXXXXX";
    } else {
        rectPath.setFillColor(ofColor::blue);
        //ofLogNotice() << "JJJJJJJJJJJJ";
    }
    return gridPos;
}
