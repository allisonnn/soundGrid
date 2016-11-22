//
//  Loader.h
//  soundGrid
//
//  Created by Xiao Chen on 2016-11-20.
//
//

#ifndef _LOADER
#define _LOADER

#include "ofMain.h"

class Loader: public ofThread{
    
    public:
    void load();
    
    Loader();
};

#endif /* Loader_h */
