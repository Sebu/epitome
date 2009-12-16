#include "cv_ext.h"



IplImage* copyBlock(IplImage *src, IplImage* dest, CvRect roiSrc, CvRect roiDest)
{
    if (roiDest.height == 0) roiDest = roiSrc;
    // change ROI
    cvSetImageROI(src, roiSrc);
    cvSetImageROI(dest, roiDest);

    cvCopy(src, dest);

    // release image ROI
    cvResetImageROI(src);
    cvResetImageROI(dest);

    return dest;
}


IplImage* subImage(IplImage *image, CvRect roi)
{
    IplImage *result;
    cvSetImageROI(image,roi);

    // sub-image
    result = cvCreateImage( cvSize(roi.width, roi.height), image->depth, image->nChannels );
    cvCopy(image,result);
    cvResetImageROI(image); // release image ROI
    return result;
}

float histogramMean(IplImage* img) {

/*
    int hist_size = 256;

    float s_ranges[] = { 0, 255 };
    float* ranges[] = { s_ranges };

    IplImage* red = cvCreateImage( cvSize(img->width, img->height), img->depth, 1 );

    cvSplit(img, red, NULL, NULL, NULL);

    CvHistogram* hist = cvCreateHist( 1, &hist_size, CV_HIST_ARRAY, ranges, 1);
    cvCalcHist( &img, hist, 0, 0 );

    for(int i = 0; i < hist_size; i++ ) {
        float* bins = cvGetHistValue_1D(hist,i);
        mean += bins[0];
        //printf("%f ", bins[0]);
    }
    //printf("\n");
    cvReleaseHist(&hist);
//    mean =  // /= (float)hist_size;
*/

    return cvAvg(img).val[0]; // /256.0f;
}