//
//  Grid.cpp
//  soundGrid
//
//  Created by Xiao Chen on 2016-11-02.
//
//

#include "Grid.h"

Grid::Grid()
{
}

void Grid::setup(int _pos)
{
    gridPos = _pos;
    mainMargin = (GROUND_PROJECTOR_RESOLUTION_X - GROUND_PROJECTOR_RESOLUTION_Y) / 2;
    generateGrids();
    
    // fonts
    font.load("fonts/BEBAS.ttf", 32);
    
    gridGlow.load("sprites/gridGlow.png");
    if (gridPos != 4) {
        planetImage.load("sprites/planets/" + to_string(gridPos) + ".png");
        video.load("videos/" + to_string(gridPos) + ".mp4");
        video.play();
    }
}

void Grid::generateGrids()
{
    int outerSide = 2 * margin + side;
    
    // Set the original position for calculations
    originalPos.x = mainMargin + outerSide * (gridPos % 3) + margin;
    originalPos.y = outerSide * (floor(gridPos / 3)) + margin;

    //----------CALCULATE THE PATH FOR OUTER RECTANGLE------------//
    rectPath.rectangle(mainMargin + outerSide * (gridPos % 3), outerSide * (floor(gridPos / 3)), outerSide, outerSide);
    rectPath.setFillColor(ofColor::blue);
    rectPath.setFilled(false);
    rectPath.setStrokeWidth(0);
    
    //----------CALCULATE THE PATH FOR INTERNAL RECTANGLE------------//
    internalPath.rectangle(originalPos.x, originalPos.y, side, side);
    internalPath.setFillColor(ofColor::black);
    internalPath.setFilled(false);
    internalPath.setStrokeWidth(0);
    
}

void Grid::update()
{
    video.update();
}

void Grid::draw()
{
    if (mode == "init") {
        if (gridPos == 4) {
//            ofSetColor(255, 0, 0);
            ofDrawRectangle(mainMargin + (2 * margin + side), 2 * margin + side, 2 * margin + side, 2 * margin + side);
        }
    } else {
        //----------DRAW THE OUTER RECTANGLE BY PATH------------//
        rectPath.draw();
        
        //----------DRAW THE INTERNAL RECTANGLE------------//
        internalPath.draw();
    }
}

int Grid::getCurrentPosition(ofVec2f point)
{
    if (point.x >= originalPos.x
        && point.x <= originalPos.x + side
        && point.y >= originalPos.y
        && point.y <= originalPos.y + side) {
        return gridPos;
    } else if (point.x < mainMargin || point.x > mainMargin + GROUND_PROJECTOR_RESOLUTION_Y) {
        return -2;
    } else {
        reset();
        return -1;
    }
}

void Grid::light()
{
    video.draw(128 + (250 + 9) * (gridPos % 3) + 25, (250 + 9) * floor(gridPos / 3) + 25, 200, 200);
    //gridGlow.draw(128 + (250 + 9) * (gridPos % 3), (250 + 9) * floor(gridPos / 3), 250, 250);
}

void Grid::glow(float startTime)
{
    float dt = ofGetElapsedTimef() - startTime;
    gridGlow.draw(128 + (250 + 9) * (gridPos % 3), (250 + 9) * floor(gridPos / 3), 250, 250);
}

void Grid::getX()
{
    
}

void Grid::reset()
{
//    if(gridPos == 0) {
//        gridGlow.draw(128, 0);
//        video.draw(128 + 25, 25, 200, 200);
//        if (font.isLoaded()) {
//            ofRectangle rect = font.getStringBoundingBox("EARTH", 100, 100);
//            font.drawString("EARTH", (250 - rect.width)/2 + 128, (250 + rect.height) / 2);
//        }
    
    planetImage.draw(128 + (250 + 9) * (gridPos % 3), (250 + 9) * floor(gridPos / 3), 250, 250);
}