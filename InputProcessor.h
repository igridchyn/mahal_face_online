//
//  InputProvider.h
//  opencv_fd_demo
//
//  Created by Igor Gridchyn on 10/7/12.
//  Copyright (c) 2012 Igor Gridchyn. All rights reserved.
//

#ifndef __opencv_fd_demo__InputProvider__
#define __opencv_fd_demo__InputProvider__

#include <iostream>
#include <stdio.h>
#include "FrameProcessor.h"
#include "cv.h"

class InputProcessor : public FrameProcessor
{
public:
    virtual cv::Ptr<Frame> Process(cv::Ptr<Frame> frame) = 0;
    virtual ~InputProcessor() {};
};

class VideoStreamInputProcessor : public InputProcessor
{
    CvCapture* capture_ = 0;
    
public:
    virtual cv::Ptr<Frame> Process(cv::Ptr<Frame> frame);
    VideoStreamInputProcessor(int deviceNumber);
    VideoStreamInputProcessor(std::string videoFileName);
    virtual ~VideoStreamInputProcessor();
};

class PhotoInputProcessor : public InputProcessor
{
public:
    virtual cv::Ptr<Frame> Process(cv::Ptr<Frame> frame);
    virtual ~PhotoInputProcessor() {};
};

class PhotoFileListInputProcessor : public InputProcessor
{
    FILE* photoListFile;
    
public:
    virtual cv::Ptr<Frame> Process(cv::Ptr<Frame> frame);
    PhotoFileListInputProcessor(std::string PhotoListFilePath);
    virtual ~PhotoFileListInputProcessor();
};

#endif /* defined(__opencv_fd_demo__InputProvider__) */
