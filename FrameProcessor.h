//
//  FrameProvider.h
//  opencv_fd_demo
//
//  Created by Igor Gridchyn on 10/7/12.
//  Copyright (c) 2012 Igor Gridchyn. All rights reserved.
//

#ifndef __opencv_fd_demo__FrameProvider__
#define __opencv_fd_demo__FrameProvider__

#include <iostream>
#include "cv.h"
#include "highgui.h"
#include "Frame.h"

class FrameProcessor
{
public:
    virtual cv::Ptr<Frame> Process(cv::Ptr<Frame> frame) = 0;
    virtual ~FrameProcessor() {};
};

#endif /* defined(__opencv_fd_demo__FrameProvider__) */
