//
//  Frame.h
//  opencv_fd_demo
//
//  Created by Igor Gridchyn on 10/7/12.
//  Copyright (c) 2012 Igor Gridchyn. All rights reserved.
//

#ifndef __opencv_fd_demo__Frame__
#define __opencv_fd_demo__Frame__

#include "cv.h"
#include "highgui.h"
#include "Face.h"

class Frame
{
public:
    cv::Ptr<Face> faces_;
    IplImage* frameImage_;
    
    Frame(IplImage* initImage)
        :frameImage_(initImage)
    {};
    
    ~Frame();
};

#include <iostream>

#endif /* defined(__opencv_fd_demo__Frame__) */
