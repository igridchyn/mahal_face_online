//
//  LBPFilter.cpp
//  opencv_fd_demo
//
//  Created by Igor Gridchyn on 10/4/12.
//  Copyright (c) 2012 Igor Gridchyn. All rights reserved.
//

#include "LBPFilter.h"

void LBPFilter::filterImage(IplImage* src, IplImage* dst)
{
    char* p = src->imageData + src->widthStep + 1;
    char* dp = dst->imageData + dst->widthStep + 1;
    
    for (int y = 1; y < src->height - 1; ++y, p += 2, dp += 2) {
        for (int x = 1; x < src->width - 1; ++x, ++p, ++dp) {
            *dp = 0;
            *dp |= *(p - src->widthStep - 1) < *p;
            *dp <<= 1;
            *dp |= *(p - src->widthStep) < *p;
            *dp <<= 1;
            *dp |= *(p - src->widthStep + 1) < *p;
            *dp <<= 1;
            *dp |= *(p + 1) < *p;
            *dp <<= 1;
            *dp |= *(p + src->widthStep + 1) < *p;
            *dp <<= 1;
            *dp |= *(p + src->widthStep) < *p;
            *dp <<= 1;
            *dp |= *(p + src->widthStep - 1) < *p;            
            *dp <<= 1;
            *dp |= *(p - 1) < *p;
        }
    }
    
    dp = dst->imageData + 1;
    p = src->imageData + 1;
    for (int x = 1; x < src->width - 1; ++x, ++p, ++dp) {
        *dp = 0;
        *dp |= *(p + 1) < *p;
        *dp <<= 1;
        *dp |= *(p + src->widthStep + 1) < *p;
        *dp <<= 1;
        *dp |= *(p + src->widthStep) < *p;
        *dp <<= 1;
        *dp |= *(p + src->widthStep - 1) < *p;
        *dp <<= 1;
        *dp |= *(p - 1) < *p;
    }
    
    dp = dst->imageData + ( dst->height - 1 ) * dst->widthStep + 1;
    p = src->imageData + ( src->height - 1 ) * src->widthStep + 1;
    for (int x = 1; x < src->width - 1; ++x, ++p, ++dp) {
        *dp = 0;
        *dp |= *(p - src->widthStep - 1) < *p;
        *dp <<= 1;
        *dp |= *(p - src->widthStep) < *p;
        *dp <<= 1;
        *dp |= *(p - src->widthStep + 1) < *p;
        *dp <<= 1;
        *dp |= *(p + 1) < *p;
        *dp <<= 4;
        *dp |= *(p - 1) < *p;
    }
    
    dp = dst->imageData + dst->widthStep;
    p = src->imageData + src->widthStep;
    for (int y = 1; y < src->height - 1; ++y, dp += dst->widthStep, p += src->widthStep)
    {
        *dp = 0;
        *dp |= *(p - src->widthStep) < *p;
        *dp <<= 1;
        *dp |= *(p - src->widthStep + 1) < *p;
        *dp <<= 1;
        *dp |= *(p + 1) < *p;
        *dp <<= 1;
        *dp |= *(p + src->widthStep + 1) < *p;
        *dp <<= 1;
        *dp |= *(p + src->widthStep) < *p;
        *dp <<= 2;
    }
    
    dp = dst->imageData + 2 * ( dst->widthStep ) - 1;
    p = src->imageData + 2 * ( src->widthStep ) - 1;
    for (int y = 1; y < src->height - 1; ++y, dp += dst->widthStep, p += src->widthStep)
    {
        *dp = 0;
        *dp |= *(p - src->widthStep - 1) < *p;
        *dp <<= 1;
        *dp |= *(p - src->widthStep) < *p;
        *dp <<= 4;
        *dp |= *(p + src->widthStep) < *p;
        *dp <<= 1;
        *dp |= *(p + src->widthStep - 1) < *p;
        *dp <<= 1;
        *dp |= *(p - 1) < *p;
    }
    
    dp = dst->imageData;
    p = src->imageData;
    *dp = ( (*(p+1) < *p) << 4 ) + ( ( *(p + src->widthStep + 1) < *p) << 3 ) + ( ( *(p + src->widthStep) < *p ) << 2 );
    
    dp = dst->imageData + dst->widthStep - 1;
    p = src->imageData + src->widthStep - 1;
    *dp = ( *(p-1) < *p ) + ( (*(p + src->widthStep - 1) < *p) << 1 ) + ( (*(p + src->widthStep) < *p) << 2 );
    
    dp = dst->imageData + ( dst->height - 1 ) * dst->widthStep;
    p = src->imageData + ( src->height - 1 ) * src->widthStep;
    *dp = ( (*(p - src->widthStep) < *p) << 6 ) + ( ( *(p - src->widthStep + 1) < *p) << 5 ) + ( ( *(p + 1) < *p ) << 4 );
    
    dp = dst->imageData + ( dst->height ) * dst->widthStep - 1;
    p = src->imageData + ( src->height ) * src->widthStep - 1;
    *dp = ( *(p-1) < *p ) + ( ( *( p - src->widthStep - 1 ) < *p) << 7 ) +  ( (*(p - src->widthStep) < *p) << 6 );
}


void LBPFilter::filterImageCLBP(IplImage* src, IplImage* dst)
{
    char* p = src->imageData + src->widthStep + 1;
    char* dp = dst->imageData + src->widthStep + 1;
    
    for (int y = 1; y < src->height - 1; ++y, p += 2, dp += 2) {
        for (int x = 1; x < src->width - 1; ++x, ++p, ++dp) {
            *dp |= *(p - src->widthStep - 1) > *(p - src->widthStep);
            *dp <<= 1;
            *dp |= *(p - src->widthStep) > *(p - src->widthStep + 1);
            *dp <<= 1;
            *dp |= *(p - src->widthStep + 1) > *(p + 1);
            *dp <<= 1;
            *dp |= *(p + 1) > *(p + src->widthStep + 1);
            *dp <<= 1;
            *dp |= *(p + src->widthStep + 1) > *(p + src->widthStep) ;
            *dp <<= 1;
            *dp |= *(p + src->widthStep) > *(p + src->widthStep - 1);
            *dp <<= 1;
            *dp |= *(p + src->widthStep - 1) > *(p - 1);
            *dp <<= 1;
            *dp |= *(p - 1) > *(p - src->widthStep - 1);
        }
    }
}

void LBPFilter::GetLBPHistogramFeatures(IplImage* lbpImg, unsigned char* feats, CvSize windowSize, const int bins)
{
    const int binSize = 256 / bins;
    char* imgData = lbpImg->imageData;
    
    for (int wy = 0; wy < lbpImg->height / windowSize.height; wy++) {
        for (int wx = 0; wx < lbpImg->width / windowSize.width; wx++) {
            imgData = lbpImg->imageData + wy * windowSize.height * lbpImg->width + wx * windowSize.width;
            for (int dy = 0; dy < windowSize.height; ++dy) {
                for (int dx = 0; dx < windowSize.width; ++dx) {
                    //std::cout << "*(imgData) = " << (int)*(imgData++) << ", ";
                    ++ *(feats + *((uchar*)(imgData++)) / binSize);
                }
                imgData += lbpImg->widthStep - windowSize.width;
            }
            feats += bins;
        }
    }
}