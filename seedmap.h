#ifndef SEEDMAP_H
#define SEEDMAP_H

#include <QList>
#include <QHash>
#include <opencv/cv.h>

#include "patch.h"
#include "transformmap.h"

class SeedMap
{
public:
    int _patchW;
    int _patchH;
    int _xgrid;
    int _ygrid;
    int _width;
    int _height;

    float _error;

    QList<Patch*> _seeds;
    QList<Patch*> _matches;

    QList<Transform*> _transforms;

    AlbumWidget* _debugAlbum;
    AlbumWidget* _debugAlbumR;

    IplImage* _sourceImage;

    QHash<QString, IplImage*> _debugImages;

    SeedMap( IplImage* image, int w, int h, int xgrid, int ygrid );
    void setImage(IplImage* image);

    Patch* at(int x, int y);

    void testPatch(int x, int y);
    void match(Patch& patch);

    // debug
    IplImage* reconstructIpl();
    IplImage* epitomeIpl();
    IplImage* meanIpl();
    IplImage* orientIpl(float delta=1.0f);

};

#endif // SEEDMAP_H