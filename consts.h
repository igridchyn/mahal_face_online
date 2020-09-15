//
//  consts.h
//  opencv_fd_demo
//
//  Created by Igor Gridchyn on 9/28/12.
//  Copyright (c) 2012 Igor Gridchyn. All rights reserved.
//

#ifndef opencv_fd_demo_consts_h
#define opencv_fd_demo_consts_h

int min_face_size = 50;
int min_eye_size = 10;

unsigned int FPS_FRAMES = 30;

// some FPs at 2e-44 (MN = 5)
double EYE_CONFIDENCE_THOLD = 1e-44;
double FACE_CONFIDENCE_THOLD = 5e-46;

// eyes do not merge for 5
const unsigned int EYE_MIN_NEIGHBOURS = 5;

const unsigned int FACE_MIN_NEIGHBOURS = 2;

const char* VIDEO_OUT_PATH = "/Users/igridchyn/Dropbox/IST_Austria/Rotations/FR_Learning/data/grab/frame.png";

//unsigned int faceSize = 256;

const uint16_t faceWidth = 128;
const uint16_t faceHeight = 128;

const float GAUSSIAN_PARAMS[] = {9.0, 9.0, 30.0};

const float faceScaleStep = 1.1;

const float eyesScaleStep = 1.1;

bool retinexEnabled = true;
bool gaborEnabled = true;
bool lbpEnabled = true;

const float GABOR_SCALE_STEP = 1.5;
const float GABOR_SCALE_START = 2.0;

//const float FACE_RECT_CUT_FRACTION = 0.045454545;
const float FACE_RECT_CUT_FRACTION = 0.0;

const int kernelSizes[] = {5, 7, 9, 15, 21};

#endif
