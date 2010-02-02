#ifndef SEED_H
#define SEED_H

#include <opencv/cv.h>

#include "orientationhistogram.h"
#include "match.h"

class Match;

class Patch
{
private:
    cv::Scalar histMean;
    std::vector<cv::Point2f> pointsSrc;


public:
    int x_, y_, w_, h_;
    int id;

    float scale;
    bool transformed;


    std::vector<Match*> overlapedMatches;
    Polygon hull;

    bool sharesMatches;
    std::vector<Match*>* matches;

    cv::Mat sourceImage_;
    cv::Mat sourceGray_;

    cv::Mat patchImage;
    cv::Mat grayPatch;
    float variance;
    OrientHist* orientHist;

    Patch(cv::Mat& sourceImage, cv::Mat& sourceGray, int x, int  y, int w, int h, float);

    bool isPatch();


    void findFeatures();
    float reconError(Match*);
    bool trackFeatures(Match*);
    Match* match(Patch&, float);

    void resetMatches();
    void copyMatches();

    cv::Scalar getHistMean() { return histMean; }
    void setHistMean(cv::Scalar _hist_mean) { this->histMean = _hist_mean; }

    void deserialize(std::ifstream&);
    void serialize(std::ofstream&);

};

#endif // SEED_H
