#include "glwidget.h"

#include <iostream>

void AlbumWidget::validatePos() {
    if (_pos<0) _pos = _values.size()-1;
    else if (_pos == _values.size()) _pos=0;

}

void AlbumWidget::prev() {
    _pos--;
    validatePos();

    makeCurrent();

    unsigned int tex = _values.at(_pos);
    glBindTexture( GL_TEXTURE_2D, tex );
    update();
}

void AlbumWidget::next() {
    _pos++;
    validatePos();

    makeCurrent();
    unsigned int tex = _values.at(_pos);
    glBindTexture( GL_TEXTURE_2D, tex );
    update();
}

int AlbumWidget::fromIpl(IplImage *image, QString caption) {
    makeCurrent();
    if (image==NULL) return -1;

//    setToolTip(caption);

    unsigned int tex;

    if (!_texImages.contains(caption)) {
        glGenTextures(1, &tex);
    } else {
        tex = _texImages[caption];
    }
    _texImages[caption] = tex;

    _values = _texImages.values();
    _pos = _values.size();

    glBindTexture( GL_TEXTURE_2D, tex );


    GLenum input_format;

    switch(image->nChannels) {
        case 1: input_format = GL_LUMINANCE; break;
        case 3: input_format = GL_BGR; break;
        case 4: input_format = GL_BGRA; break;
        default: input_format = GL_BGR; break;
    }

    GLenum channel_depth;

    switch(image->depth) {
        case IPL_DEPTH_8U: channel_depth  = GL_UNSIGNED_BYTE; break;
        case IPL_DEPTH_32F: channel_depth = GL_FLOAT; break;
        default: channel_depth = GL_UNSIGNED_BYTE; break;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->width, image->height, 0, input_format, channel_depth, image->imageData);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);

    return 0;

}
