//
//  main.cpp
//  opencv_fd_demo
//
//  Created by Igor Gridchyn on 9/28/12.
//  Copyright (c) 2012 Igor Gridchyn. All rights reserved.
//

#include "cv.h"
#include "highgui.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <float.h>
#include <limits.h>
#include <time.h>
#include <ctype.h>
#include <vector>
#include <queue>

#include "consts.h"
#include <time.h>

#include "GaborFilter.h"
#include "LBPFilter.h"

#include "InputProcessor.h"
#include "Frame.h"

// Create memory for calculations
static CvMemStorage* storage = 0;
static CvMemStorage* eyesStorage = 0;

// Create a new Haar classifier
static CvHaarClassifierCascade* cascade = 0;
static CvHaarClassifierCascade* eyesCascade = 0;

// Function prototype for detecting and drawing an object from an image
void detect_and_draw( IplImage* image , float fps);

// Create a string that contains the cascade name
const char* cascade_name =
"haarcascade_frontalface_alt.xml";
/*    "haarcascade_profileface.xml";*/

std::vector<GaborKernelParams> gaborKernelsSet;
int gaborKernelIndex = 0;

std::queue< std::vector< unsigned char > > prevFaceFeatures;
const uint8_t faceDelay = 40;
double similarity = .0;

cv::Ptr<InputProcessor> input;

void ShowKernel()
{
    cvNamedWindow( "Kernel", 2 );
    GaborKernelParams par = GaborFilter::DEFAULT_KERNEL_PARAMS;
    cv::Mat kernel = GaborFilter::mkKernel(par, true);
    cv::Mat Lkernel(par.kernel_size_*20, par.kernel_size_*20, CV_32F);
    cv::resize(kernel, Lkernel, Lkernel.size(), 0, 0, cv::INTER_AREA);
    double mn, mx;

    IplImage kimg = Lkernel;
    cv::minMaxIdx(Lkernel, &mn, &mx);

    Lkernel -= mn;
    Lkernel *= 255 / (mx - mn);
    
    cvShowImage("Kernel", &kimg);
    cvSaveImage(VIDEO_OUT_PATH, &kimg);
}

// Main function, defines the entry point for the program.
int main( int argc, char** argv )
{
    ShowKernel();
    
    // Images to capture the frame from video or camera or from file
    IplImage *frame, *frame_copy = 0;
    
    // Used for calculations
    int optlen = strlen("--cascade=");
    
    // Input file name for avi or image file.
    const char* input_name;
    
    // Check for the correct usage of the command line
    if( argc > 1 && strncmp( argv[1], "--cascade=", optlen ) == 0 )
    {
        cascade_name = argv[1] + optlen;
        input_name = argc > 2 ? argv[2] : 0;
    }
    else
    {
        fprintf( stderr,
                "Usage: facedetect --cascade=\"<cascade_path>\" [filename|camera_index]\n" );
        return -1;
        /*input_name = argc > 1 ? argv[1] : 0;*/
    }
    
    float sc = GABOR_SCALE_START;
    GaborKernelParams baseParams = GaborFilter::DEFAULT_KERNEL_PARAMS;
    for (int sci = 0; sci < 5; sc *= GABOR_SCALE_STEP, sci++) {
        baseParams.pos_sigma_ = sc;
        for (int mu=0; mu<360; mu+=45) {
            baseParams.pos_th_ = mu;            
            gaborKernelsSet.push_back(baseParams);
        }
    }
    
    // Load the HaarClassifierCascade
    cascade = (CvHaarClassifierCascade*)cvLoad( cascade_name, 0, 0, 0 );
    eyesCascade = (CvHaarClassifierCascade*)cvLoad("/usr/local/share/OpenCV/haarcascades/haarcascade_eye.xml");
    
    // Check whether the cascade has loaded successfully. Else report and error and quit
    if( !cascade )
    {
        fprintf( stderr, "ERROR: Could not load classifier cascade\n" );
        return -1;
    }
    
    // Allocate the memory storage
    storage = cvCreateMemStorage(0);
    eyesStorage = cvCreateMemStorage(0);
    
    //input = new PhotoFileListInputProcessor("/Users/igridchyn/Dropbox/IST_Austria/Rotations/FR_Learning/data/grab/list.txt");
    
    // Find whether to detect the object from file or from camera.
    if( !input_name || (isdigit(input_name[0]) && input_name[1] == '\0') )
        input = new VideoStreamInputProcessor( !input_name ? 0 : input_name[0] - '0' );
    else
        input = new VideoStreamInputProcessor ( input_name );
    
    // Create a new named window with title: result
    cvNamedWindow( "result", 1 );
    
    // Find if the capture is loaded successfully or not.
    
    
    unsigned long start;
    unsigned int frameCnt = 0;
    unsigned long durations[FPS_FRAMES];
    unsigned long movingTotal = 0;
    float movingAverage = 0;
    unsigned long duration = 0;
    
    // won't work on osx until opencv recompiled with ffmpeg
//    CvVideoWriter *writer = 0;
//    int isColor = 1;
//    int fps     = 25;  // or 30
//    int frameW  = 1280; // 744 for firewire cameras
//    int frameH  = 720; // 480 for firewire cameras
//    writer=cvCreateVideoWriter(VIDEO_OUT_PATH, CV_FOURCC('I','Y','U','V'),
//                               fps,cvSize(frameW,frameH),isColor);
    
    
    
    cv::Ptr<Frame> cframe;
    while ( (cframe = input->Process(NULL)) != NULL )
    {        
        frame = cframe->frameImage_;
        
        // Allocate framecopy as the same size of the frame
        if( !frame_copy )
            frame_copy = cvCreateImage( cvSize(frame->width,frame->height),
                                       IPL_DEPTH_8U, frame->nChannels );
        
        // Check the origin of image. If top left, copy the image frame to frame_copy.
        if( frame->origin == IPL_ORIGIN_TL )
            cvCopy( frame, frame_copy, 0 );
        // Else flip and copy the image
        else
            cvFlip( frame, frame_copy, 0 );
        
//        char imgPath[200];
//        const char *sesname = "Sriram";
//        sprintf(imgPath, "%s_%s_%d.png", VIDEO_OUT_PATH, sesname, frameCnt);
//        cvSaveImage(imgPath, frame_copy);
        
        if (frameCnt % 2)
        {
            start = clock();
            // Call the function to detect and draw the face
            detect_and_draw( frame_copy , movingAverage);
            duration = clock() - start;
        }
        else
        {
            // use frame with previous detection
            //cvShowImage( "result", frame_copy );
        }
            
        // Wait for a while before proceeding to the next frame
        char key = cvWaitKey( 10 );
        if (key == 61) // '='
            EYE_CONFIDENCE_THOLD *= 1.25;
        else if (key == 45) // '-'
            EYE_CONFIDENCE_THOLD *= 0.8;
        else if( key == 27 ) // 'ESC'
            break;
        else if( key == 114 ) // 'r'
            retinexEnabled = ! retinexEnabled;
        else if (key == 103) // 'g'
            gaborEnabled = ! gaborEnabled;
        else if (key == 110) // 'n'
            gaborKernelIndex = (gaborKernelIndex + 1) % gaborKernelsSet.size();
        else if (key == 108) // 'l'
            lbpEnabled = ! lbpEnabled;
        
        // measure FPS
        
        if (frameCnt < FPS_FRAMES) {
            durations[frameCnt % FPS_FRAMES] = duration;
            movingTotal += duration;
        }
        else {
            movingAverage = (float)FPS_FRAMES / movingTotal * 1000000;
            
            movingTotal -= durations[frameCnt % FPS_FRAMES];
            durations[frameCnt % FPS_FRAMES] = duration;
            movingTotal += duration;
        }
        
        frameCnt++;
        
            //cvWriteFrame(writer, frame);      // add the frame to the file
            //std::cout << "frame " << frameCnt << " ";
    }
    
    cvReleaseImage( &frame_copy );
    
    // Destroy the window previously created with filename: "result"
    //cvReleaseVideoWriter(&writer);
    cvDestroyWindow("result");
    
    // return 0 to indicate successfull execution of the program
    return 0;
}

void SingleScaleRetinex(IplImage* src, IplImage* dst)
{
    cvSmooth(src, dst, CV_GAUSSIAN, 11, 11, 80.0);
    
    unsigned char *srcData= reinterpret_cast<unsigned char *>(src->imageData);
    unsigned char *dstData= reinterpret_cast<unsigned char *>(dst->imageData);
    
    int step = src->widthStep;
    
    cv::Mat retout = cvCreateMat(src->height, src->width, CV_32FC1);
    
    for (int i = 0; i < src->height; i++) {
        //const float* roData = retout->ptr<float>(i);
        for (int j = 0; j< src->width; j+= src->nChannels) {
            if ( !srcData[j] || !dstData[j] )
                retout.at<float>(i, j) = 0;
            else
                retout.at<float>(i, j) = log(srcData[j]) - log(dstData[j]);
            //std::cout << retout.at<float>(i, j)<< "\n";
        }
        srcData += step;
        dstData += step;
    }
    
    double rmin, rmax;
    cv::minMaxLoc(retout, &rmin, &rmax);
    
    retout = (retout - rmin) / (rmax - rmin) * 255;
    
    dstData= reinterpret_cast<unsigned char *>(dst->imageData);
    for (int i = 0; i < src->height; i++) {
        //const float* roData = retout->ptr<float>(i);
        for (int j = 0; j< src->width; j+= src->nChannels) {
            dstData[j] = (int)retout.at<float>(i, j);
        }
        dstData += step;
    }
}

double similarityMeasure(const std::vector<uint8_t>& feats1, const std::vector<uint8_t>& feats2)
{
    assert(feats1.size() == feats2.size());
    
    uint64_t sum = 0;
    
    for (int i = 0; i < feats1.size(); ++i) {
        //sum += std::min<uint8_t>(feats1[i], feats2[i]);
        sum += std::abs(feats1[i] - feats2[i]);
    }
    
    return (double) sum / feats1.size();
}

// Function to detect and draw any faces that is present in an image
void detect_and_draw( IplImage* img , float fps)
{
    int fdScale = 4;
    int edScale = 2;
    
    // Create a new image based on the input image
    IplImage* fdImg = cvCreateImage( cvSize( img->width / fdScale, img->height / fdScale), 8, 3 );
    
    // Create two points to represent the face locations
    CvPoint fpt1, fpt2, pt1, pt2;
    
    // Clear the memory storage which was used before
    cvClearMemStorage( storage );
    
    CvRect allImgRect = cvRect( 0, 0, img->width, img->height );

    
    // Find whether the cascade is loaded, to find the faces. If yes, then:
    if( cascade )
    {
        // There can be more than one face in an image. So create a growable sequence of faces.
        // Detect the objects and store them in the sequence
        cvResize(img, fdImg);
        CvSeq* faces = cvHaarDetectObjects( fdImg, cascade, storage,
                                           faceScaleStep, FACE_MIN_NEIGHBOURS, CV_HAAR_DO_CANNY_PRUNING,
                                           cvSize(min_face_size, min_face_size));
        
        // Loop the number of faces found.
        for( int i = 0; i < (faces ? faces->total : 0); i++ )
        {            
            // Create a new rectangle for drawing the face
            CvRect* fr = (CvRect*)cvGetSeqElem( faces, i );
            
            CvObjectDetection* fdet = (CvObjectDetection*) cvGetSeqElem( faces, i );
            if (fdet->score < FACE_CONFIDENCE_THOLD)
                continue;
//            
//            fr->width *= fdScale;
//            fr->height *= fdScale;
//            fr->x *= fdScale;
//            fr->y *= fdScale;
            
            fr->x += fr->width * FACE_RECT_CUT_FRACTION;
            fr->width *= (1.0 - 2 * FACE_RECT_CUT_FRACTION);
            
            CvRect origRect = *fr;
            origRect.height *= fdScale;
            origRect.width *= fdScale;
            origRect.x *= fdScale;
            origRect.y *= fdScale;
            
            // Find the dimensions of the face,and scale it if necessary
            fpt1.x = fr->x * fdScale;
            fpt2.x = (fr->x+fr->width) * fdScale;
            fpt1.y = fr->y * fdScale;
            fpt2.y = (fr->y+fr->height) * fdScale;
            
            // detect eyes
            cvClearMemStorage( eyesStorage );
            cvSetImageROI(img, origRect);
            cvSetImageROI(fdImg, *fr);
            
            IplImage* edImg = cvCreateImage( cvSize( cvGetSize( img ).width / edScale, cvGetSize( img ).height / edScale), 8, 3 );
            cvResize(img, edImg);

            CvSeq* eyes = cvHaarDetectObjects(edImg, eyesCascade, eyesStorage, eyesScaleStep, EYE_MIN_NEIGHBOURS, CV_HAAR_DO_CANNY_PRUNING, cvSize(min_eye_size, min_eye_size));
            
            if( eyes->total == 0 )
            {
                cvSetImageROI(img, allImgRect);
                cvReleaseImage( &edImg );
                continue;
            }
            
//           cv::bilateralFilter(faceMat, faceMatCopy, 5, 150, 150);

            IplImage* imgCopy = cvCreateImage( cvSize( img->roi->width, img->roi->height) , IPL_DEPTH_8U, 1);
            cvCvtColor(img, imgCopy, CV_RGB2GRAY);            
            //IplImage* imgCopySmothed = cvCreateImage( cvSize( img->roi->width, img->roi->height) , IPL_DEPTH_8U, 1);
            //cvSmooth(imgCopy, imgCopySmothed, CV_BILATERAL, 9, 9, 300.0f, 300.0f);
            // gaussian can be done in-place
            //cvCvtColor(imgCopySmothed, img, CV_GRAY2RGB);
           
            IplImage* imgResized = cvCreateImage( cvSize(faceWidth, faceHeight) , IPL_DEPTH_8U, 1);
            IplImage* imgResized2 = cvCreateImage( cvSize(faceWidth, faceHeight) , IPL_DEPTH_8U, 1);
            IplImage* imgRetinex = cvCreateImage( cvSize(faceWidth, faceHeight) , IPL_DEPTH_8U, 1);
            
//            IplImage* imgResized = cvCreateImage( cvSize( img->roi->width, img->roi->height) , IPL_DEPTH_8U, 1);
//            IplImage* imgResized2 = cvCreateImage( cvSize( img->roi->width, img->roi->height) , IPL_DEPTH_8U, 1);
//            IplImage* imgRetinex = cvCreateImage( cvSize( img->roi->width, img->roi->height) , IPL_DEPTH_8U, 1);
            
            cvResize(imgCopy, imgResized);
            
            if(retinexEnabled)
                SingleScaleRetinex(imgResized, imgRetinex);
            else
                cvCopy(imgResized, imgRetinex);
            
            // extract facial features
            CvSize blockWindowSize = cvSize(8, 8);
            const int histBins = 16;
            std::vector<unsigned char> faceFeatures;
            uint32_t perFilterSize = imgResized2->width / blockWindowSize.width * imgResized2->height / blockWindowSize.height * histBins;
            faceFeatures.resize(gaborKernelsSet.size() * perFilterSize);
            
            //for (int g = 0; g < gaborKernelsSet.size(); ++g) {
            for (int g = gaborKernelIndex; g <= gaborKernelIndex; ++g) {
                if (gaborEnabled) {
                    GaborFilter::filterImage(imgRetinex, imgResized, GaborFilter::mkKernel(gaborKernelsSet[g], true), GaborFilter::mkKernel(gaborKernelsSet[g], false));
                }
                else
                    cvCopy(imgRetinex, imgResized);
                
//                if (lbpEnabled)
//                    LBPFilter::filterImage(imgResized, imgResized2);
//                else
//                    cvCopy(imgResized, imgResized2);
//                
//                LBPFilter::GetLBPHistogramFeatures(imgResized2, &faceFeatures[g * perFilterSize], blockWindowSize, histBins);
                
                if ( g == gaborKernelIndex )
                    cvResize(imgResized, imgCopy, CV_INTER_AREA);
            }
            
            prevFaceFeatures.push(faceFeatures);
            
//            if ( prevFaceFeatures.size() >= faceDelay)
//            {
//                similarity = similarityMeasure(faceFeatures, prevFaceFeatures.front());
//                prevFaceFeatures.pop();
//            }
            
            // debug
//            std::cout << faceFeatures.size() << ": ";
//            for (int i = 0; i < 100; ++i) {
//                std::cout << (int)faceFeatures[4500+i] << ", ";
//            }
            
            //cvResize(imgResized2, imgCopy, CV_INTER_AREA);
            
            //cvSmooth(imgCopy, imgCopySmothed, CV_GAUSSIAN, (int)GAUSSIAN_PARAMS[0], (int)GAUSSIAN_PARAMS[1], GAUSSIAN_PARAMS[2]);
            //cvCopy(imgCopy, imgCopySmothed);
            
            cvCvtColor(imgCopy, img, CV_GRAY2RGB);
            
            cvReleaseImage( &imgCopy );
            //cvReleaseImage( &imgCopySmothed );
            cvReleaseImage( &imgResized );
            cvReleaseImage( &imgResized2 );
            cvReleaseImage( &imgRetinex );
            
            for (int j = 0; j < (eyes ? eyes->total : 0); j++) {
                CvRect* er = (CvRect*) cvGetSeqElem( eyes, j );
                
                CvObjectDetection* det = (CvObjectDetection*) cvGetSeqElem( eyes, j );
                //std::cout << det->score << "\n";
                
                if (det->score < EYE_CONFIDENCE_THOLD || er->height * edScale > fr->height * fdScale / 3 || er->y * edScale> fr->height * fdScale / 2)
                    continue;
                
                pt1.x = er->x * edScale;
                pt2.x = (er->x+er->width) * edScale;
                pt1.y = er->y * edScale;
                pt2.y = (er->y+er->height) * edScale;
                
                // Draw the rectangle in the input image
                cvRectangle( img, pt1, pt2, CV_RGB(0,255,0), 3, 8, 0 );
            }
            
            // Draw the rectangle in the input image

            cvSetImageROI(img, allImgRect);
            cvRectangle( img, fpt1, fpt2, CV_RGB(255,0,0), 3, 8, 0 );
            cvReleaseImage( &edImg );
        }
        
    }
    
    cvSetImageROI(img, allImgRect);
    
    CvFont font;
    double hScale=1.0;
    double vScale=1.0;
    int    lineWidth=1;
    cvInitFont(&font,CV_FONT_HERSHEY_SIMPLEX|CV_FONT_ITALIC, hScale,vScale,0,lineWidth);
    char buf[30];
    sprintf(buf, "processing fps = %.1f", fps);
    cvPutText(img, buf, cvPoint(30, 30), &font, cvScalar(200,200,250));
    sprintf(buf, "confidence thold = %.3e", EYE_CONFIDENCE_THOLD);
    cvPutText(img, buf, cvPoint(30, 65), &font, cvScalar(250,200,200));
    sprintf(buf, "similarity = %.1f", similarity);
    cvPutText(img, buf, cvPoint(30, 100), &font, cvScalar(200,250,200));
    
    // Show the image in the window named "result"
    cvShowImage( "result", img );
    
    
    // Release the temp image created.
    cvReleaseImage( &fdImg );
}