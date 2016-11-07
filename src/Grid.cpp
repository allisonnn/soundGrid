//
//  Grid.cpp
//  soundGrid
//
//  Created by Xiao Chen on 2016-11-02.
//
//

#include "Grid.h"

Grid::Grid() {
}

void Grid::setup(int _pos) {
    gridPos = _pos;
    mainMargin = (PROJECTOR_RESOLUTION_X - PROJECTOR_RESOLUTION_Y) / 2;
    generateGrids();
}

void Grid::generateGrids() {
    float outerSide = 2 * margin + side;

    //----------CALCULATE THE PATH FOR OUTER RECTANGLE------------//
    rectPath.rectangle(mainMargin + outerSide * (gridPos % 3), outerSide * (floor(gridPos / 3)), outerSide, outerSide);
    rectPath.setFillColor(ofColor::blue);
    rectPath.setFilled(true);
    rectPath.setStrokeWidth(0);
    
    //----------CALCULATE THE PATH FOR INTERNAL RECTANGLE------------//
    originalPos.x = mainMargin + outerSide * (gridPos % 3) + margin;
    originalPos.y = outerSide * (floor(gridPos / 3)) + margin;
    internalPath.rectangle(mainMargin + outerSide * (gridPos % 3) + margin, outerSide * (floor(gridPos / 3)) + margin, side, side);
    internalPath.setFillColor(ofColor::red);
    internalPath.setFilled(true);
    internalPath.setStrokeWidth(0);
    
}

void Grid::update() {
    //rectPath.setFillColor(ofColor::green);
}

void Grid::draw() {
    //----------DRAW THE OUTER RECTANGLE BY PATH------------//
    rectPath.draw();
    
    //----------DRAW THE INTERNAL RECTANGLE------------//
    internalPath.draw();
}

int Grid::isIn(ofVec2f point) {
    ofLogNotice() << point.x << "teetetet" << originalPos.x;
    if (point.x >= originalPos.x
        && point.x <= originalPos.x + side
        && point.y >= originalPos.y
        && point.y <= originalPos.y + side) {
        rectPath.setFillColor(ofColor::green);
        //ofLog() << gridPos << "XXXXXXXXXXXXXXXXXXXXXXXXXX" << endl;
    } else {
        rectPath.setFillColor(ofColor::blue);
        //ofLogNotice() << "JJJJJJJJJJJJ";
    }
    return gridPos;
}
