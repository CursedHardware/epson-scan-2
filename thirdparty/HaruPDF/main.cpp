#include <iostream>
#include <fstream>
#include "hpdf.h"
struct MatchPathSeparator
{
    bool operator()( char ch ) const
    {
        return ch == '/';
    }
};

std::string basename( std::string const& pathname )
{
    return std::string( 
        std::find_if( pathname.rbegin(), pathname.rend(),
                      MatchPathSeparator() ).base(),
        pathname.end() );
}
std::streampos GetFleSize( const char* filePath ){

    std::streampos fsize = 0;
    std::ifstream file( filePath, std::ios::binary );

    fsize = file.tellg();
    file.seekg( 0, std::ios::end );
    fsize = file.tellg() - fsize;
    file.close();

    return fsize;
}

void error_handler  (HPDF_STATUS error_no,
                     HPDF_STATUS detail_no,
                     void *user_data)
{
    std::cout <<  "error2" <<  std::endl;
}


int main(int argc, char* argv[])
{
    std::cout << "start" << std::endl;
    HPDF_Doc  pdf;

    pdf = HPDF_New (error_handler, NULL);
    if (!pdf) {
        std::cout <<  "error1 " <<  std::endl;
        return -1;
    }

    { // ADD Page
        int imageCount = argc - 1;
        if (imageCount < 0)
        {
            std::cout << "argument error" << std::endl;
            return -1;
        }
        for (int i = 0; i < imageCount; i++)
        {
            const char* fileName = argv[i + 1];

            int pageCount = 0;
            int width = 0;
            int height = 0;
            bool isJpeg = 0;

            // filename is pagecount_x_y.jpg format
            std::string fileNameAsString(fileName);
            if(fileNameAsString.substr(fileNameAsString.find_last_of(".") + 1) == "jpg") {
                sscanf(basename(fileName).c_str(), "%d_%d_%d.jpg", &pageCount, &width, &height);
                isJpeg = true;
            }else{
                sscanf(basename(fileName).c_str(), "%d_%d_%d.raw", &pageCount, &width, &height);
                isJpeg = false;
            }

            std::cout << "width " << width << " height " << height << std::endl;

            HPDF_Page page;
            HPDF_Destination dst;

            page = HPDF_AddPage (pdf);

            HPDF_Page_SetWidth (page, width);
            HPDF_Page_SetHeight (page, height);

            //dst = HPDF_Page_CreateDestination (page);
            //HPDF_Destination_SetXYZ (dst, 0, HPDF_Page_GetHeight (page), 1);
           // HPDF_SetOpenAction(pdf, dst);
            HPDF_SetCompressionMode (pdf, HPDF_COMP_ALL);

            HPDF_Image image;
            if (isJpeg)
            {
                std::cout << "will create jpeg image" << std::endl;
                image = HPDF_LoadJpegImageFromFile (pdf, fileName);
            }else{
                std::cout << "will create raw image" << std::endl;
                
                int fileSize = (int)GetFleSize(fileName); 
                
                std::ifstream inputfile(fileName);
                if (!inputfile)
                {
                    std::cout << "no input file" << std::endl;
                    return 0;
                }
                unsigned char* bytes = (unsigned char*)malloc(fileSize);
                memset(bytes, 0, fileSize);
                if (bytes)
                {
                    inputfile.read((char*)bytes, fileSize);
                    std::cout << "do write image: " << fileSize << " " << inputfile.gcount() << std::endl;
                    image = HPDF_Image_LoadRaw1BitImageFromMem(pdf, (const HPDF_BYTE *)bytes, width, height, (width + 7) / 8 , false, true);
                    free(bytes);
                }
            }
            std::cout << "HPDF_Image_GetWidth (image) " << HPDF_Image_GetWidth (image) << "HPDF_Image_GetHeight (image)" << HPDF_Image_GetHeight (image) << std::endl;
            HPDF_Page_DrawImage (page, image, 0, 0, HPDF_Image_GetWidth (image), HPDF_Image_GetHeight (image));
        }
    }

    { // Save
        HPDF_SaveToFile (pdf, "/tmp/a.pdf");
        HPDF_Free (pdf);
    }

    std::cout << "end" << std::endl;

}
