//
//  InputProvider.cpp
//  opencv_fd_demo
//
//  Created by Igor Gridchyn on 10/7/12.
//  Copyright (c) 2012 Igor Gridchyn. All rights reserved.
//

#include "InputProcessor.h"

VideoStreamInputProcessor::VideoStreamInputProcessor(int deviceNumber)
{
    capture_ = cvCaptureFromCAM( deviceNumber );
}

VideoStreamInputProcessor::VideoStreamInputProcessor(std::string videoFileName)
{
    capture_ = cvCaptureFromAVI( videoFileName.c_str() );
}

cv::Ptr<Frame> VideoStreamInputProcessor::Process(cv::Ptr<Frame> frame)
{
    if ( !cvGrabFrame(capture_) )
        return NULL;
    
    IplImage* nImage = cvRetrieveFrame(capture_);
    if ( !nImage )
        return NULL;
    
    cv::Ptr<Frame> newFrame = new Frame(nImage);
    return newFrame;
}

VideoStreamInputProcessor::~VideoStreamInputProcessor()
{
    cvReleaseCapture( &capture_ );
}

PhotoFileListInputProcessor::PhotoFileListInputProcessor(std::string photoListFilePath)
{
    photoListFile = fopen( photoListFilePath.c_str(), "rt" );
}

PhotoFileListInputProcessor::~PhotoFileListInputProcessor()
{
    fclose(photoListFile);
}

cv::Ptr<Frame> PhotoFileListInputProcessor::Process(cv::Ptr<Frame> frame)
{
    char buf[1000 + 1];
    
    if ( fgets(buf, 1000, photoListFile) )
    {
        int len = (int)strlen(buf);
        while( len > 0 && isspace(buf[len-1]) )
            len--;
        buf[len] = '\0';
        
        IplImage* image = cvLoadImage( buf, 1 );
        if ( image )
            return new Frame(image);
        else
            return NULL;
    }
    else
        return NULL;
}