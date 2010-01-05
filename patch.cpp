#include "patch.h"
#include "transformmap.h"
#include "cv_ext.h"


float Patch::reconError(Transform* t) {

    Patch* other = t->seed;

    cv::Mat diff; //( this->_w, this->_h, this->patchImage.type());

    // reconstruct
    t->colorScale = 1.0f;
    cv::Mat reconstruction( t->reconstruct() );


    // 4.1 translation, color scale
    // atm actually only contrast scale
    // drop when over bright
    for(int i; i<4; i++)
        t->colorScale[i] = this->getHistMean()[i] / histogramMean(reconstruction)[i];
    if(t->colorScale[0]>1.25f) return 100.0f;
    // correct color scale

   reconstruction *= t->colorScale[0];

    // sqaure distance
    diff = cv::abs(this->patchImage - reconstruction);

    cv::pow(diff, 2, diff);

    float sum = (float)cv::sum(diff).val[0];

    cv::Mat mean( this->_w, this->_h, this->patchImage.type(), this->getHistMean() );
    cv::Mat varianceMap = cv::abs( patchImage - mean );

    cv::pow(varianceMap, 2, varianceMap);
    float variance = (float)cv::sum(varianceMap).val[0];

    float alpha = 1.0f; // 0 <= alpha <= 2
    float beta  = .02f;


    // reconError =  ( cvSum(mul).val[0] ) / ( pow( variance , alpha) + beta );
    return sum / variance;
}

void Patch::findFeatures() {

    cv::Mat grayImage;
    cv::cvtColor(patchImage, grayImage, CV_BGR2GRAY);

    cv::goodFeaturesToTrack(grayImage, pointsSrc, count, .01, .01);

}
bool Patch::trackFeatures(Transform* t) {

    if(pointsSrc.size()<3) {
//                std::cout << "bad patch" << std::endl;
        return true;
    }

    std::vector<cv::Point2f>    pointsDest;
    std::vector<uchar>          status;
    std::vector<float>          err;

    cv::Mat rotated(t->rotate());
    cv::Mat result( _w, _h, patchImage.type() );

    copyBlock(rotated, result, cvRect(t->_seedX, t->_seedY, _w, _h), cvRect(0, 0, _w, _h));

    // gray required?
    cv::Mat grayPatch;
    cv::cvtColor(patchImage, grayPatch, CV_BGR2GRAY);

    cv::Mat grayResult;
    cv::cvtColor(result, grayResult, CV_BGR2GRAY);

    cv::calcOpticalFlowPyrLK( grayPatch, grayResult,
                              pointsSrc, pointsDest,
                              status, err,
                              cv::Size(3,3), 1, cv::TermCriteria(cv::TermCriteria::COUNT+cv::TermCriteria::EPS, 30, 0.1));
    //                            cvTermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 40, .1 ), 0 );

    cv::Point2f srcTri[3], destTri[3];

    // TODO: rewrite status is now a vector
    int index = 0;
    for(unsigned int i = 0; i < status.size(); i++) {
        if(status[i]) {
            srcTri[index].x = pointsSrc[i].x + t->_seedX;
            srcTri[index].y = pointsSrc[i].y + t->_seedY;
            destTri[index].x = pointsDest[i].x + t->_seedX;
            destTri[index].y = pointsDest[i].y + t->_seedY;
            index++;
        }
        if (index>2) break;
    }
    if (index<2) return true;

    t->warpMat = cv::getAffineTransform(destTri, srcTri);

    return true;

}
Transform* Patch::match(Patch& other, float error) {

    Transform* transform = new Transform(_x, _y, other._x, other._y, &other);

    // 4.1 rotation, orientation/gradient histogram
    float orientation = this->orientHist->minDiff(other.orientHist);

    // apply initial rotation
    cv::Point2f center( other._x+(_w/2), other._y+(_h/2) );
    transform->rotMat = cv::getRotationMatrix2D(center, orientation, 1.0f);

    // 4.1 KLT matching
    if (!trackFeatures(transform)) { delete transform; return 0; }


    // 4 reconstruction error
    float reconstuctionError =  reconError(transform);

    if (reconstuctionError > error) { delete transform; return 0; }

    std::cout << _x << " " << _y << " " <<  "color: " << transform->colorScale[0] << "\t\t orient.: " << orientation << "\t\t error: " << reconstuctionError;
    std::cout << " " << other.scale;
    if(_x==other._x && _y==other._y) std::cout << "\tfound myself!";
    std::cout << std::endl;

    return transform;
}

Patch::Patch(cv::Mat& _sourceImage, int x, int  y, int w, int h):
        histMean(0.0f), _x(x), _y(y), _w(w), _h(h), count(4), sourceImage(_sourceImage)
{
    patchImage = sourceImage(cv::Rect(_x,_y,_w,_h));
    orientHist = new OrientHist(patchImage, 36);
    setHistMean( histogramMean(patchImage) );
}
