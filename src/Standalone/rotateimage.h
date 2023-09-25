#ifndef ROTATEIMAGE_H
#define ROTATEIMAGE_H

#include <QImage>
#include "supervisor.h"

class RotateImage
{
public:
    RotateImage(ColorType type);

    void Rotate(std::string image_path, int angle, ImageFormat format, bool multiTiffEnabled, int OFDEnabled);

private:
    ColorType m_type;

};

#endif // ROTATEIMAGE_H
