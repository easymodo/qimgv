#include "imagelib.h"

ImageLib::ImageLib() {

}

void ImageLib::scale(QImage *dest, const QImage *source, QSize destSize, bool smooth) {
    Qt::TransformationMode mode = smooth ? Qt::SmoothTransformation : Qt::FastTransformation;
    *dest = source->scaled(destSize.width(), destSize.height(), Qt::IgnoreAspectRatio, mode);
}

inline unsigned char saturate( float x )
{
    return x>255.0f?255:x<0.0f?0:(uchar)x;
}

inline float cubicInterpolate(float A, float B, float C, float D, float t, float tt, float ttt)
{
    float a = (-A + (3.0f*B) - (3.0f*C) + D) / 2.0f;
    float b = A - (5.0f*B) / 2.0f + 2.0f*C - D / 2.0f;
    float c = (-A + C) / 2.0f;
    float d = B;

    return a*ttt + b*tt + c*t + d;
}

inline const uchar* getPixel(const uchar* bits, int width, int height, int x, int y, int channels) {
    return bits + (x<0?0:(x>=width?width-1:x))*channels + (y<0?0:(y>=height?height-1:y))*channels*width;
}
/*
void ImageLib::bicubicScale(QPixmap *outPixmap, const QImage* in, int destWidth, int destHeight) {
    QImage *out = new QImage(destWidth,destHeight,in->format());

    const uchar *p = in->bits();
    uchar *outP = out->bits();

    int channels = in->bytesPerLine() / in->width();

    const uchar *sp[4][4]; // 4x4 sampling area [x][y]
    float col[4];
    float dx2, dx3, dy2, dy3;

    const float tx = (float)(in->width() - 1) / destWidth;
    const float ty = (float)(in->height() - 1) / destHeight;

    for(int i=0; i<destHeight; i++) {
        const int y = (int)(ty * i);
        const float dy = ty*i - y;
        dy2 = dy*dy;
        dy3 = dy2*dy;
        for(int j=0; j<destWidth; j++) {
            const int x = (int)(tx * j);
            const float dx = tx*j - x;
            for(int n = 0; n < 4; n++) {
                sp[0][n] = getPixel(p, in->width(), in->height(),
                                        x-1, y-1+n, channels);
                sp[1][n] = getPixel(p, in->width(), in->height(),
                                        x, y-1+n, channels);
                sp[2][n] = getPixel(p, in->width(), in->height(),
                                        x+1, y-1+n, channels);
                sp[3][n] = getPixel(p, in->width(), in->height(),
                                        x+2, y-1+n, channels);
            }
            dx2 = dx*dx;
            dx3 = dx2*dx;
            for(int k=0; k<channels; k++) {
                col[0] = cubicInterpolate(*(sp[0][0]+k), *(sp[1][0]+k),
                                      *(sp[2][0]+k), *(sp[3][0]+k), dx, dx2, dx3);
                col[1] = cubicInterpolate(*(sp[0][1]+k), *(sp[1][1]+k),
                                      *(sp[2][1]+k), *(sp[3][1]+k), dx, dx2, dx3);
                col[2] = cubicInterpolate(*(sp[0][2]+k), *(sp[1][2]+k),
                                      *(sp[2][2]+k), *(sp[3][2]+k), dx, dx2, dx3);
                col[3] = cubicInterpolate(*(sp[0][3]+k), *(sp[1][3]+k),
                                      *(sp[2][3]+k), *(sp[3][3]+k), dx, dx2, dx3);
                *(outP + i*out->bytesPerLine() + j*channels + k) =
                        saturate(cubicInterpolate(col[0],col[1],col[2],col[3], dy, dy2, dy3));
            }
        }
    }
    *outPixmap = QPixmap::fromImage(*out);
    delete out;
}


*/
