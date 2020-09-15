//
//  GaborFilter.h
//  opencv_fd_demo
//
//  Created by Igor Gridchyn on 10/2/12.
//  Copyright (c) 2012 Igor Gridchyn. All rights reserved.
//

#ifndef __opencv_fd_demo__GaborFilter__
#define __opencv_fd_demo__GaborFilter__

#include <iostream>
#include "cv.h"
#include "highgui.h"

struct GaborKernelParams
{
public:
    int kernel_size_;
    int pos_sigma_;
    double pos_lm_;
    int pos_th_;
    int pos_psi_;
    double scale_;
    
public:
    GaborKernelParams(int kernel_size, int pos_sima, double pos_lm, int pos_th, int pos_psi, double scale = 1.0);
};

class GaborFilter
{
public:
    static const GaborKernelParams DEFAULT_KERNEL_PARAMS;
    static cv::Mat mkKernel(GaborKernelParams kernelParams = DEFAULT_KERNEL_PARAMS, bool real = true);
    static void filterImage(IplImage* src, IplImage* dst, cv::Mat kernel = mkKernel(), cv::Mat imgKernel = mkKernel(DEFAULT_KERNEL_PARAMS, true));
};

#endif /* defined(__opencv_fd_demo__GaborFilter__) */
