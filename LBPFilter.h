//
//  LBPFilter.h
//  opencv_fd_demo
//
//  Created by Igor Gridchyn on 10/4/12.
//  Copyright (c) 2012 Igor Gridchyn. All rights reserved.
//

#ifndef __opencv_fd_demo__LBPFilter__
#define __opencv_fd_demo__LBPFilter__

#include <iostream>
#include "cv.h"
#include "highgui.h"

class LBPFilter
{
public:
    static void filterImage(IplImage* src, IplImage* dst);
    static void filterImageCLBP(IplImage* src, IplImage* dst);
    static void GetLBPHistogramFeatures(IplImage* lbpImg, unsigned char* feats, CvSize windowSize, const int bins);
};

#endif /* defined(__opencv_fd_demo__LBPFilter__) */
