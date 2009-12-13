#ifndef SEED_H
#define SEED_H

#include <opencv/cv.h>

#include "orientationhistogram.h"

class Patch
{
private:
    float _histMean;


public:
    int _x, _y, _w, _h;

    float _scale;

    IplImage* _patchImage;
    IplImage* _sourceImage;
    OrientHist* _orientHist;

    Patch(IplImage* sourceImage, int x=0, int y=0, int w=16, int h=16);

    float reconError(const Patch& other);
    bool match(Patch& patch);

    float histMean() { return _histMean; }
    void setHistMean(float hist_mean) { this->_histMean = hist_mean; }

};

#endif // SEED_H
