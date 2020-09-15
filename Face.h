//
//  Face.h
//  opencv_fd_demo
//
//  Created by Igor Gridchyn on 10/2/12.
//  Copyright (c) 2012 Igor Gridchyn. All rights reserved.
//

#ifndef __opencv_fd_demo__Face__
#define __opencv_fd_demo__Face__

#include <iostream>
#include "cv.h"
#include "highgui.h"

class Face
{
    IplImage* image;
    IplImage* normImage;
};

#endif /* defined(__opencv_fd_demo__Face__) */
