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
    int outerSide = 2 * margin + side;
    
    // Set the original position for calculations
    originalPos.x = mainMargin + outerSide * (gridPos % 3) + margin;
    originalPos.y = outerSide * (floor(gridPos / 3)) + margin;

    //----------CALCULATE THE PATH FOR OUTER RECTANGLE------------//
    rectPath.rectangle(mainMargin + outerSide * (gridPos % 3), outerSide * (floor(gridPos / 3)), outerSide, outerSide);
    rectPath.setFillColor(ofColor::blue);
    rectPath.setFilled(true);
    rectPath.setStrokeWidth(0);
    
    //----------CALCULATE THE PATH FOR INTERNAL RECTANGLE------------//
    internalPath.rectangle(originalPos.x, originalPos.y, side, side);
    internalPath.setFillColor(ofColor::red);
    internalPath.setFilled(true);
    internalPath.setStrokeWidth(0);
    
}

void Grid::update() {
    
}

void Grid::draw() {
    //----------DRAW THE OUTER RECTANGLE BY PATH------------//
    rectPath.draw();
    
    //----------DRAW THE INTERNAL RECTANGLE------------//
    internalPath.draw();
}

int Grid::getCurrentPosition(ofVec2f point) {
    ofLogNotice() << point.x << "teetetet" << originalPos.x;
    if (point.x >= originalPos.x
        && point.x <= originalPos.x + side
        && point.y >= originalPos.y
        && point.y <= originalPos.y + side) {
        rectPath.setFillColor(ofColor::green);
        return gridPos;
    } else {
        rectPath.setFillColor(ofColor::blue);
        return -1;
    }
}
