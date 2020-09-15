//
//  GaborFilter.cpp
//  opencv_fd_demo
//
//  Created by Igor Gridchyn on 10/2/12.
//  Copyright (c) 2012 Igor Gridchyn. All rights reserved.
//

#include "GaborFilter.h"

cv::Mat getGaborKernelCV( cv::Size ksize, double sigma, double theta,
                            double lambd, double gamma, double psi, int ktype, bool real = true)
{
    double sigma_x = sigma;
    double sigma_y = sigma/gamma;
    int nstds = 3;
    int xmin, xmax, ymin, ymax;
    double c = cos(theta), s = sin(theta);

    if( ksize.width > 0 )
        xmax = ksize.width/2;
    else
        xmax = std::max(fabs(nstds*sigma_x*c), fabs(nstds*sigma_y*s));

    if( ksize.height > 0 )
        ymax = ksize.height/2;
    else
        ymax = std::max(fabs(nstds*sigma_x*s), fabs(nstds*sigma_y*c));

    xmin = -xmax;
    ymin = -ymax;

    cv::Mat kernel(ymax - ymin + 1, xmax - xmin + 1, ktype);
    double scale = 1/(2*CV_PI*sigma_x*sigma_y);
    double ex = -0.5/(sigma_x*sigma_x);
    double ey = -0.5/(sigma_y*sigma_y);
    double cscale = CV_PI*2/lambd;

    for( int y = ymin; y <= ymax; y++ )
        for( int x = xmin; x <= xmax; x++ )
        {
            double xr = x*c + y*s;
            double yr = -x*s + y*c;
            
            double v = 0.0;
            if (real)
                v = scale*exp(ex*xr*xr + ey*yr*yr) * cos( cscale*xr + psi ) ;
            else
                v = scale*exp(ex*xr*xr + ey*yr*yr) * sin( cscale*xr + psi ) ;
                
            if( ktype == CV_32F )
                kernel.at<float>(ymax - y, xmax - x) = (float)v;
            else
                kernel.at<double>(ymax - y, xmax - x) = v;
        }
   
    return kernel;
}

GaborKernelParams::GaborKernelParams(int kernel_size, int pos_sima, double pos_lm, int pos_th, int pos_psi, double scale)
    : kernel_size_(kernel_size)
    , pos_sigma_(pos_sima)
    , pos_lm_(pos_lm)
    , pos_th_(pos_th)
    , pos_psi_(pos_psi)
    , scale_(scale)
{}

void GaborFilter::filterImage(IplImage* src, IplImage* dst, cv::Mat realKernel, cv::Mat imgKernel)
{
    CvMat realKern = realKernel;
    CvMat imgKern = imgKernel;
    
    IplImage* imgImage = cvCreateImage(cvSize(dst->width, dst->height), dst->depth, dst->nChannels);
    cvFilter2D(src, dst, &realKern);
    //return;
    cvFilter2D(src, imgImage, &imgKern);

    cv::Mat rePart(src->height, src->width, CV_32FC1);
    cv::Mat imPart(src->height, src->width, CV_32FC1);
    cv::Mat srcUChar(src);
    cv::Mat srcFloat(src->height, src->width, CV_32FC1);
    srcUChar.convertTo(srcFloat, CV_32FC1);
    cv::filter2D(srcFloat, rePart, -1, realKernel);
    cv::filter2D(srcFloat, imPart, -1, imgKernel);
    
    unsigned char* data = (unsigned char*)dst->imageData;
//    unsigned char* idata = (unsigned char*)imgImage->imageData;
    
    for (int y = 0; y < dst->height; ++y){
         for (int x = 0; x < dst->width; ++x) {
//             *data *= 10;
//             ++data;
//         }
//    }
             
//             int rm = (int)*data * (int)*data;
//             int im = (int)*idata * (int)*idata;
//             *data = (unsigned char)sqrt(rm + im);
//             ++data;
//             ++idata;
             
             float rm = rePart.at<float>(y,x);
             rm *= rm;
             float im = imPart.at<float>(y,x);
             im *= im;
             *data = (unsigned char)sqrt((rm + im) / 2.0) * 10;
             ++data;
        }
    }
    
    cvReleaseImage( &imgImage );
}

// kernel size, sigma, lambda, theta, psi
const GaborKernelParams GaborFilter::DEFAULT_KERNEL_PARAMS(5, 3, 0.5, 90, 0, 1);

cv::Mat GaborFilter::mkKernel(GaborKernelParams kernelParams, bool real)
{
    // params: kernel size (3 * sigma if 0), sigma, theta, lmbda (freq), gamma (x/y), psi, type, imaginary or real
    // psi = CV_PI*0.5 ?
    cv::Mat ocvGabor = getGaborKernelCV(cv::Size(0,0), kernelParams.pos_sigma_, kernelParams.pos_th_, 5, 1, 0, CV_32F, real);
    float rsum = cv::sum(ocvGabor)[0];
    std::cout << ocvGabor.cols << " cols, sum=" << rsum << ", real=" << real << "\n";
    if (rsum < 0.01)
        return ocvGabor;
    return ocvGabor - cv::Mat(ocvGabor.rows, ocvGabor.cols, CV_32FC1, rsum / (ocvGabor.rows * ocvGabor.cols));
    
    int hks = ( kernelParams.kernel_size_ - 1 )/2;
    double theta = kernelParams.pos_th_ * CV_PI/180;
    double psi = kernelParams.pos_psi_ * CV_PI/180;
    double del = 0.367;
    //double lmbd = kernelParams.pos_lm_;
    //double sigma = (double)kernelParams.pos_sigma_ / kernelParams.kernel_size_;
    int kscale = kernelParams.scale_;
    
    double x_theta;
    double y_theta;
    cv::Mat kernel(kernelParams.kernel_size_, kernelParams.kernel_size_, CV_32F);
    
    double fmax = 2.5;
    double fu = fmax / pow(2.0, (float)kscale / 2);
    double fus = fu * fu;
    
    double sum = 0.0;
    
    for (int y=-hks; y<=hks; y++)
    {
        for (int x=-hks; x<=hks; x++)
        {
            x_theta = x * del * cos( theta ) + y * del * sin( theta );
            y_theta = - x * del * sin( theta ) + y * del * cos( theta );
            
            //kernel.at<float>( hks + y , hks + x ) = (float)exp(-0.5*(pow(x_theta,2)+pow(y_theta,2))/pow(sigma,2)) * (imaginary ? sin(sqrt(scale) * 2 * CV_PI*x_theta / lmbd + psi) : cos(sqrt(scale) * 2 * CV_PI*x_theta / lmbd + psi));
            
            kernel.at<float>( hks + y , hks + x ) = 1.0 / (CV_PI * 2) * fus * (float)exp(-0.5*fus*(pow(x_theta,2)+pow(y_theta,2))) * (real ? cos( 2 * CV_PI*x_theta * fu + psi) : sin(2 * CV_PI*x_theta * fu + psi));
            
            sum += kernel.at<float>( hks + y , hks + x );
        }
    }
    
    for (int y=-hks; y<=hks; y++)
    {
        for (int x=-hks; x<=hks; x++)
        {
            kernel.at<float>( hks + y , hks + x ) /= sum;
        }
    }
    
    return kernel;
}