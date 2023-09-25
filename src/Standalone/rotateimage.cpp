#include "rotateimage.h"
#include <QGraphicsScene>

RotateImage::RotateImage(ColorType type)
{
    m_type = type;
}

void RotateImage::Rotate(std::string image_path, int angle, ImageFormat format, bool multiTiffEnabled, int OFDEnabled)
{
    std::string file_format;

    if (format == kSDIImageFormatJPEG)
    {
        file_format = "JPG";
    }else if (format == ImageFormatColorJpegMonoRaw) //PDF
    {
        if(m_type == ColorTypeMono1){
            file_format = "PBM";
        }else {
            file_format = "JPG";
        }
    }else if (format == kSDIImageFormatPNM && !multiTiffEnabled)
    {
        file_format = "PPM";
    }else if (format == kSDIImageFormatTIFF)
    {
        file_format = "TIFF";
    }else if (format == kSDIImageFormatPNG)
    {
        file_format = "PNG";
    }else if (format == kSDIImageFormatPNM && multiTiffEnabled) //Multi-TIFF
    {
        file_format = "PPM";
    }else if (format == kSDIImageFormatPNM && OFDEnabled)
    {
        file_format = "OFD";
    }

    QImage srcImg(image_path.c_str());
    QPoint center = srcImg.rect().center();
    QMatrix matrix;
    matrix.translate(center.x(), center.y());
    matrix.rotate(angle);
    QImage dstImg = srcImg.transformed(matrix);
    //QPixmap dstPix = QPixmap::fromImage(dstImg);
    dstImg.save(QString::fromStdString(image_path), file_format.c_str(), -1);
}
