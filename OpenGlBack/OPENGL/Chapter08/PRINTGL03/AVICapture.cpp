// CAVICapture.cpp: implementation of the CCAVICapture class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PrintGL.h"
#include "AVICapture.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAVICapture::CAVICapture() :
    width(-1),
    height(-1),
    frameRate(30.0f),
    frameCounter(0),
    capturing(false),
    aviFile(NULL),
    aviStream(NULL),
    compAviStream(NULL),
    image(NULL)
{
    AVIFileInit();
}


CAVICapture::~CAVICapture()
{
    cleanup();
    AVIFileExit();
}


bool CAVICapture::start(CString filename,int w, int h,float fps)
{
    if (capturing)
        return false;

    width = w;
    height = h;
    frameRate = fps;

    if (HIWORD(VideoForWindowsVersion()) < 0x010a)
    {
        // 版本号必须大于1.1
        return false;
    }

    int rowBytes = (width * 3 + 3) & ~0x3;
    image = new unsigned char[rowBytes * height]; 
	//  创建AVI文件
    HRESULT hr = AVIFileOpen(&aviFile,
                             filename,
                             OF_WRITE | OF_CREATE,
                             NULL);
    if (hr != AVIERR_OK)
    {
        MessageBox(NULL,"Erroring creating avi file for capture.","ERROR",MB_OK);
        return false;
    }
	//  AVI文件的头信息
    AVISTREAMINFO info;
    ZeroMemory(&info, sizeof info);
    info.fccType = streamtypeVIDEO;
    info.fccHandler = 0;
    info.dwScale = 1;
    info.dwRate = (DWORD) frameRate;
    info.dwSuggestedBufferSize = rowBytes * height;
    SetRect(&info.rcFrame, 0, 0, width, height);
    hr = AVIFileCreateStream(aviFile, &aviStream, &info);
    if (hr != AVIERR_OK)
    {
        MessageBox(NULL,"Error creating AVI stream.","ERROR",MB_OK);
        cleanup();
        return false;
    }

    // 允许用户选择压缩方式
    AVICOMPRESSOPTIONS options;
    AVICOMPRESSOPTIONS* arrOptions[1] = { &options };
    ZeroMemory(&options, sizeof options);
    if (!AVISaveOptions(NULL, 0, 1, &aviStream, 
                        (LPAVICOMPRESSOPTIONS*) &arrOptions))
    {
         cleanup();
        return false;
    }
    hr = AVIMakeCompressedStream(&compAviStream, aviStream, &options, NULL);
    if (hr != AVIERR_OK)
    {
        MessageBox(NULL,"Error creating compressed AVI stream.", "ERROR",MB_OK);
        cleanup();
        return false;
    }

    BITMAPINFOHEADER bi;
    ZeroMemory(&bi, sizeof bi);
    bi.biSize = sizeof bi;
    bi.biWidth = width;
    bi.biHeight = height;
    bi.biPlanes = 1;
    bi.biBitCount = 24;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = rowBytes * height;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed = 0;
    bi.biClrImportant = 0;
	//  设置数据格式
    hr = AVIStreamSetFormat(compAviStream, 0, &bi, sizeof bi);
    if (hr != AVIERR_OK)
    {
        MessageBox(NULL,"AVIStreamSetFormat failed.","ERROR",MB_OK);
        cleanup();
        return false;
    }

    capturing = true;
    frameCounter = 0;

    return true;
}


bool CAVICapture::end()
{
    capturing = false;
    cleanup();

    return true;
}


bool CAVICapture::captureFrame()
{
    if (!capturing)
        return false;
    // 获得当前视口的尺寸
    int viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    int x = viewport[0] + (viewport[2] - width) / 2;
    int y = viewport[1] + (viewport[3] - height) / 2;
	//  读入屏幕上捕捉区域的像素
    glReadPixels(x, y, width, height,GL_BGR_EXT, GL_UNSIGNED_BYTE,image);
    int rowBytes = (width * 3 + 3) & ~0x3;
    LONG samplesWritten = 0;
    LONG bytesWritten = 0;
	//  写入像素数据到AVI文件中
    HRESULT hr = AVIStreamWrite(compAviStream,
                                frameCounter,
                                1,
                                image,
                                rowBytes * height,
                                AVIIF_KEYFRAME,
                                &samplesWritten,
                                &bytesWritten);
    if (hr != AVIERR_OK)
    {
        MessageBox(NULL,"AVIStreamWrite failed on frame.", "ERROR",MB_OK);
        return false;
    }
    frameCounter++;
    return true;
}


void CAVICapture::cleanup()
{
    if (aviStream != NULL)
    {
        AVIStreamRelease(aviStream);
        aviStream = NULL;
    }
    if (compAviStream != NULL)
    {
        AVIStreamRelease(compAviStream);
        compAviStream = NULL;
    }
    if (aviFile != NULL)
    {
        AVIFileRelease(aviFile);
        aviFile = NULL;
    }
    if (image != NULL)
    {
        delete[] image;
        image = NULL;
    }
}


int CAVICapture::getWidth() const
{
    return width;
}

int CAVICapture::getHeight() const
{
    return height;
}

float CAVICapture::getFrameRate() const
{
    return frameRate;
}


