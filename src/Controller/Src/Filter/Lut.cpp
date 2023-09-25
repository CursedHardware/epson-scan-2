#include "Lut.hpp"
#include "Utility.hpp"
#include <stdlib.h>
#include <utils/PathUtils.h>
#include <utils/ESFile.h>
#include <math.h>

namespace epsonscan
{
#define MAXPIXELINDEX 255

void BuildGammaTable(	int				brightness,
						int				contrast,
						int				gamma,	//gamma x 100 (0.5->50)
						unsigned char* GammaR,
                        unsigned char* GammaG,
						unsigned char* GammaB)
{
	unsigned char	map[256];
	LONG			val;
	int				Smc, Sc, divisor, delta, Br;
	int				i;
	unsigned char	value;

	int				Gammaval;
	double			fx, fy;
	double			expo;
	double			fa, fb;


	brightness *= 10;
	contrast *= 10;
	brightness = (std::max)(-999, (std::min)(999, brightness));
	contrast = (std::max)(-999, (std::min)(999, contrast));


	//	make a 256 table for transformation
	//	D(M,N) = 255 * ( d(m,n) - ( s+c )) / (h-c) - (s+c) ) + b

	delta	= 255;
	Sc		= contrast;
	if( Sc < -1000 || Sc > 1000 ) {
		Sc = 0;
	}
	Sc = Sc * 127 / 1000;  // scale to (-127 > 127)	from -1000 to 1000
	
	Smc		= Sc;
	divisor	= delta - 2 * Sc;
	Br		= brightness;
	if( Br < -1000 || Br > 1000 ) {
		Br = 0;
	}

	Br = Br * 127 / 1000;	// scale to (-127 > 127) from -1000 to 1000
	for( i = 0; i < 256; i ++ ) {
		val		= 255L *( (LONG)i - Smc ) / (divisor) + Br;
		value	= (unsigned char)val;
		if( val < 0 ) {
			value = 0;
		}
		if( val > MAXPIXELINDEX ) {
			value = MAXPIXELINDEX;
		}
		map[i] = value;
	}

	for( i = 0; i < 256; i++ ) {
		GammaR[i]	= map[ GammaR[i] ];
		GammaG[i]	= map[ GammaG[i] ];
		GammaB[i]	= map[ GammaB[i] ];
	}

	Gammaval = gamma;
	if( Gammaval < 50 || Gammaval > 300 ) {
		return;		// out of range, shouldn't happen
	}

	if( Gammaval == 220 ) {
		return;		// no change
	}

	expo = 220.0 /* = BaseGamma */ / (double)Gammaval;
	for ( i = 0; i < 256; i ++) {
		fx = (double)(i);
		fa = fx/255.0;
		fb = pow( fa, expo );
		fy = 255.0 * fb;
		if ( fy > 255.0 ) {
			fy = 255.0;
		}
		map[i] = (int)(fy+0.5);			
	}

	for( i = 0; i < 256; i++ ) {
		GammaR[i]	= map[ GammaR[i] ];
		GammaG[i]	= map[ GammaG[i] ];
		GammaB[i]	= map[ GammaB[i] ];
	}

}


	void stringToGamma(std::string string,unsigned char* gamma)
	{
		std::stringstream stream{string};
		std::string buf;

		int i = 0;	
		while(std::getline(stream, buf, ',') && i < 256 )
		{
			gamma[i] = atoi(buf.c_str());
			i++;
		}
	}


#define ADF_BASETABLE_R "0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,3,4,5,7,8,10,12,14,16,18,20,22,25,27,29,31,33,35,37,39,40,42,44,46,48,50,51,52,54,56,57,59,61,63,64,66,68,69,71,73,74,76,78,79,81,82,84,86,87,89,90,92,94,95,97,98,100,101,103,104,106,107,109,110,112,113,115,116,118,119,121,122,124,125,127,128,130,131,133,134,135,137,138,140,141,144,145,146,148,149,151,152,153,155,156,158,159,160,162,163,165,166,167,169,170,171,173,174,176,177,178,180,181,182,184,185,186,188,189,190,192,193,194,196,197,198,200,201,202,204,205,206,208,209,210,212,213,214,215,217,218,219,221,222,223,225,226,227,228,230,231,232,233,235,236,237,239,240,241,242,244,245,246,247,249,250,251,252,254,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255"
#define ADF_BASETABLE_G "0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,3,4,5,7,8,10,12,14,16,18,20,22,25,27,29,31,33,35,37,39,40,42,44,46,48,50,51,52,54,56,57,59,61,63,64,66,68,69,71,73,74,76,78,79,81,82,84,86,87,89,90,92,94,95,97,98,100,101,103,104,106,107,109,110,112,113,115,116,118,119,121,122,124,125,127,128,130,131,133,134,135,137,138,140,141,144,145,146,148,149,151,152,153,155,156,158,159,160,162,163,165,166,167,169,170,171,173,174,176,177,178,180,181,182,184,185,186,188,189,190,192,193,194,196,197,198,200,201,202,204,205,206,208,209,210,212,213,214,215,217,218,219,221,222,223,225,226,227,228,230,231,232,233,235,236,237,239,240,241,242,244,245,246,247,249,250,251,252,254,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255"
#define ADF_BASETABLE_B "0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,3,4,5,7,8,10,12,14,17,19,21,23,26,28,30,32,34,36,38,40,41,43,45,47,49,51,52,54,56,58,59,61,63,65,66,68,70,71,73,75,76,78,80,81,83,84,86,88,89,91,92,94,96,97,99,100,102,103,105,106,108,109,111,112,114,115,117,118,120,121,123,124,126,127,129,130,132,133,135,136,137,139,140,142,143,145,146,147,149,150,152,153,154,156,157,159,160,161,163,164,166,167,168,170,171,172,174,175,177,178,179,181,182,183,185,186,187,189,190,191,193,194,195,197,198,199,201,202,203,205,206,207,209,209,210,212,213,214,215,217,218,219,221,222,223,225,226,227,228,230,231,232,233,235,236,237,239,240,241,242,244,245,246,247,249,250,251,252,254,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255"


#define SDE_INVERSEGAMMAREVISION(x,y)	(255.*pow(((x)/255.), y))

void BuildBaseTable(unsigned char* GammaR,
                    unsigned char* GammaG,
					unsigned char* GammaB,
					std::shared_ptr<KeyMgr> keyMgr,
					  std::shared_ptr<ModelInfo> modelInfo,
					  bool autoJudgedColor
					)
{
	ESNumber kind = kPrvHRD_Kind_SF_Document;
	modelInfo->GetValue(kPrvHRD_Kind, kind);

	if ((keyMgr->GetValueInt(kSDIFunctionalUnitKey) == kSDIFunctionalUnitDocumentFeeder  || kind == kPrvHRD_Kind_SF_Document) &&
	    keyMgr->GetValueInt(kSDIColorTypeKey) != kSDIColorTypeMono1 &&
		!(keyMgr->GetValueInt(kSDIColorTypeKey) == kSDIColorTypeAuto && autoJudgedColor == false)
		)
	{
		stringToGamma(std::string(ADF_BASETABLE_R), GammaR);
		stringToGamma(std::string(ADF_BASETABLE_G), GammaG);
		stringToGamma(std::string(ADF_BASETABLE_B), GammaB);
	}else{

		double gammaConvert = 1.0;

		if (keyMgr->GetValueInt(kSDIColorTypeKey) == kSDIColorTypeMono1 || autoJudgedColor == false)
		{
			// 18 to 10
			gammaConvert = 1.8; 
		}else{
			// 18 to 22
			gammaConvert = 1.8/2.2; 
		}

		for (int i = 0; i < 256; i++)
		{
			GammaR[i] = SDE_INVERSEGAMMAREVISION(i, gammaConvert);
			GammaG[i] = SDE_INVERSEGAMMAREVISION(i, gammaConvert);
			GammaB[i] = SDE_INVERSEGAMMAREVISION(i, gammaConvert);
		}
	}
}



	void Lut::ProcessLUT(ESImageInfo& imageInfo, 
				         ES_CMN_FUNCS::BUFFER::CESHeapBuffer& inDataBuf,
						 std::shared_ptr<KeyMgr> keyMgr,
						  std::shared_ptr<ModelInfo> modelInfo,
						  bool autoJudgedColor
						 )
    {
        unsigned char GammaR[256];
        unsigned char GammaG[256];
        unsigned char GammaB[256];

		BuildBaseTable(GammaR, GammaG, GammaB, keyMgr, modelInfo, autoJudgedColor);

		if (keyMgr->GetValueInt(kSDIColorTypeKey) != kSDIColorTypeMono1 &&
			!(keyMgr->GetValueInt(kSDIColorTypeKey) == kSDIColorTypeAuto && autoJudgedColor == false))
		{
    	    BuildGammaTable( keyMgr->GetValueInt(kSDIBrightnessKey),
							 keyMgr->GetValueInt(kSDIContrastKey), 
							 keyMgr->GetValueInt(kSDIGammaKey) * 10,
							 GammaR, 
							 GammaG, 
							 GammaB);
		}


		ProcessLUT(imageInfo, inDataBuf, GammaR, GammaG, GammaB );
	
    }

	void Lut::ProcessLUT(ESImageInfo& imageInfo, 
						ES_CMN_FUNCS::BUFFER::CESHeapBuffer& inDataBuf,
                	    unsigned char* gammaR,
                    	unsigned char* gammaG,
                    	unsigned char* gammaB 
                    )
	{
        ESNumber nImageWidth      = ES_IMAGE_INFO::GetESImageWidth(imageInfo);
        ESNumber nImggeHeight     = ES_IMAGE_INFO::GetESImageHeight(imageInfo);
        ESNumber nSamplesPerPixel = ES_IMAGE_INFO::GetESImageSamplesPerPixel(imageInfo);
        ESNumber nBytesPerRow     = ES_IMAGE_INFO::GetESImageBytesPerRow(imageInfo);

        UInt8* pInBytes = inDataBuf.GetBufferPtr();
	    ES_IMAGE_INFO::ESImageColorType eColorType = ES_IMAGE_INFO::GetESImageColorType(imageInfo);

        for (ESNumber i = 0; i < nImggeHeight; i++) {

            for( ESNumber j = 0; j < nImageWidth; j++ ) {
                
                uint32_t nIndex =  nBytesPerRow * i + j * nSamplesPerPixel;
                
                if( eColorType == ES_IMAGE_INFO::kESImageColorTypeRGB ) {
                    pInBytes[nIndex]     = gammaR[pInBytes[nIndex]];
                    pInBytes[nIndex + 1] = gammaG[pInBytes[nIndex + 1]];
                    pInBytes[nIndex + 2] = gammaB[pInBytes[nIndex + 2]];
                }else if( eColorType == ES_IMAGE_INFO::kESImageColorTypeGray ) {
                    pInBytes[nIndex]     = gammaG[pInBytes[nIndex]];
                }else{
                    assert(FALSE);
                }
            }
        }
	}

	bool Lut::isFitPluginAvailable()
	{
		std::string pluginRoot = Filter::GetPluginRootDir();
		ESString fitplugin;
		ES_CMN_FUNCS::PATH::ES_CombinePath(fitplugin, pluginRoot, ES_STRING("fitplugin"));
			
		return ES_CMN_FUNCS::PATH::ES_IsExistFile(fitplugin.c_str());	
	}

	bool Lut::isSKipLut()
	{
		//
		//  Text enhance + color mode, lut should be skipped
		//
		return (keyMgr_->GetValueInt(kSDITextEnhanceKey) != kSDITextEnhanceLevelNone &&
		       ( (keyMgr_->GetValueInt(kSDIColorTypeKey) == kSDIColorTypeAuto && autoJudgedColor_)  ||
			      keyMgr_->GetValueInt(kSDIColorTypeKey) == kSDIColorTypeRGB24 ||
				  keyMgr_->GetValueInt(kSDIColorTypeKey) == kSDIColorTypeMono8) );			   

	}


	void Lut::DoProcess(ESImageInfo& imageInfo, ES_CMN_FUNCS::BUFFER::CESHeapBuffer& inDataBuf)
    {
		if (isSKipLut() == false && 
			ES_IMAGE_INFO::GetESImageBitsPerSample(imageInfo) != 16 &&
			ES_IMAGE_INFO::GetESImageColorType(imageInfo) != ES_IMAGE_INFO::kESImageColorTypeMono)
		{
            SDI_TRACE_LOG("backgroundRemoval Level = %d", keyMgr_->GetValueInt(kSDIBackgroundRemovalKey) );

            if (keyMgr_->GetValueInt(kSDIBackgroundRemovalKey) != kSDIBackgroundRemovalNone && isFitPluginAvailable())
            {
                ProcessLUTUsingFit(imageInfo,
                                    inDataBuf,
                                    keyMgr_->GetValueInt(kSDIBrightnessKey),
                                    keyMgr_->GetValueInt(kSDIContrastKey),
                                    keyMgr_->GetValueInt(kSDIGammaKey),
                                    (SDIBackgroundRemoval)keyMgr_->GetValueInt(kSDIBackgroundRemovalKey),
                                    ES_CMN_FUNCS::PATH::ES_GetWorkTempPath()
                                    );
            }else{
                ProcessLUT(imageInfo,inDataBuf, keyMgr_, modelInfo_, autoJudgedColor_);
            }
		}
    }

	static const int kMaxBuf = 5000;

	void Lut::ProcessLUTUsingFit(ESImageInfo& imageInfo, 
					ES_CMN_FUNCS::BUFFER::CESHeapBuffer& inDataBuf,
					ESNumber brightness, 
					ESNumber contrast,
                    ESNumber gamma, 	//gamma x 10 (0.5->5S)
					SDIBackgroundRemoval bgRemoval,
					std::string workFolder
                    )
	{
		std::string pluginRoot = Filter::GetPluginRootDir();
		ESString fitplugin;
		ES_CMN_FUNCS::PATH::ES_CombinePath(fitplugin, pluginRoot, ES_STRING("fitplugin"));

		if(ES_CMN_FUNCS::PATH::ES_IsExistFile(fitplugin.c_str()))
		{
			auto inFile = CESFile::CreateTempFileInstanceWithPrefix(workFolder,  "FITInput", CESFile::ES_OPEN_MODE_WRITE_PLUS);
			if (inFile == nullptr)
			{
				SDI_TRACE_LOG("FITInput file creates error");
				return ;
			}

			if(inFile->WriteData(inDataBuf) == false)
			{
				SDI_TRACE_LOG("WriteData fails");

				if(ES_CMN_FUNCS::PATH::ES_IsExistFile(inFile->GetFileName()))
			    {
					remove(inFile->GetFileName().c_str());
      		    }
				delete inFile;
				inFile = nullptr;

				return ;
			}
			inFile->Flush();
			inFile->CloseFile();


			std::stringstream fitExecCommand;
			fitExecCommand << fitplugin;
			fitExecCommand << " ";
			fitExecCommand << brightness;
			fitExecCommand << " ";
			fitExecCommand << contrast;
			fitExecCommand << " ";
			fitExecCommand << gamma;
			fitExecCommand << " ";
			fitExecCommand << ES_IMAGE_INFO::GetESImageWidth(imageInfo);
			fitExecCommand << " ";
			fitExecCommand << ES_IMAGE_INFO::GetESImageHeight(imageInfo);
			fitExecCommand << " ";
			fitExecCommand << ES_IMAGE_INFO::GetESImageSamplesPerPixel(imageInfo);
			fitExecCommand << " ";
			fitExecCommand << (bgRemoval == kSDIBackgroundRemovalNormal ? 0 : 1);
			fitExecCommand << " ";
			fitExecCommand << inFile->GetFileName();

	        unsigned char GammaR[256] = {};
     	    unsigned char GammaG[256] = {};
       	    unsigned char GammaB[256] = {};

			SDI_TRACE_LOG("fitExecCommand = %s", fitExecCommand.str().c_str());

			FILE* gid = popen(fitExecCommand.str().c_str(), "r");
			if (gid)
			{
				SDI_TRACE_LOG("process init success");

				char buf[kMaxBuf];
				memset(buf, 0, kMaxBuf);
				if(fgets(buf, kMaxBuf, gid) != NULL)
				{
					std::string str = buf;
					SDI_TRACE_LOG("gamma red = %s", str.c_str());
					stringToGamma(str, GammaR);
				}

				memset(buf, 0, kMaxBuf);			
				if(fgets(buf, kMaxBuf, gid) != NULL)
				{
					std::string str = buf;
					SDI_TRACE_LOG("gamma green = %s", str.c_str());
					stringToGamma(str, GammaG);
				}

				memset(buf, 0, kMaxBuf);			
				if(fgets(buf, kMaxBuf, gid) != NULL)
				{
					std::string str = buf;
					SDI_TRACE_LOG("gamma blue = %s", str.c_str());
					stringToGamma(str, GammaB);
				}

				pclose(gid);
			}


			if(ES_CMN_FUNCS::PATH::ES_IsExistFile(inFile->GetFileName()))
		    {
					remove(inFile->GetFileName().c_str());
      		}
			delete inFile;
			inFile = nullptr;

			ProcessLUT(imageInfo, inDataBuf, GammaR, GammaG, GammaB );
		}
	}
}
