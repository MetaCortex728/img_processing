
// IR140Dlg.h : header file
//

#pragma once
#include "resource.h"
#include <world.hpp>
#include "opencv.hpp"

#include <PvSampleUtils.h>
#include <PvDevice.h>
#include <PvDeviceGEV.h>
#include <PvDeviceU3V.h>
#include <PvStream.h>
#include <PvStreamGEV.h>
#include <PvStreamU3V.h>
#include <PvPipeline.h>
#include <PvBuffer.h>

#include <vector>

struct DisplayThreadArg
{
	PvDevice* aDevice;
	PvStream* aStream;
	PvPipeline* aPipeline;
	bool aAcquiringImages;

};

#define BUFFER_COUNT ( 2 )
#define imgHeight 640
#define imgWidth 480
#define imgSize (imgHeight*imgWidth)

// CIR140Dlg dialog
class CIR140Dlg : public CDialog
{
// Construction
public:
	CIR140Dlg(CWnd* pParent = nullptr);	// standard constructor
	virtual ~CIR140Dlg();
// Dialog Data

	enum { IDD = IDD_IR140_DIALOG };


protected:

	PvDevice* ConnectToDevice(const PvString& aConnectionID);
	PvStream* OpenStream(const PvString& aConnectionID);
	void ConfigureStream(PvDevice* aDevice, PvStream* aStream);
	PvPipeline* CreatePipeline(PvDevice* aDevice, PvStream* aStream);
	static UINT AcquireImages(LPVOID lParam);
	//void AcquireImages(PvDevice* aDevice, PvStream* aStream, PvPipeline* aPipeline);
	void EnableInterface();
	CButton mPlayButton;
	CButton mStopButton;

	PvDevice* mDevice;
	PvStream* mStream;
	PvPipeline* mPipeline;
	PvString mConnectionID;
	DisplayThreadArg DParg;
	CWinThread* pThread;

	bool mAcquiringImages;

	void setBitInfo(BITMAPINFO* bInfo, cv::Mat srcmat)
	{
		bInfo->bmiHeader.biYPelsPerMeter = 0;
		bInfo->bmiHeader.biBitCount = srcmat.channels() * 8;
		bInfo->bmiHeader.biWidth = srcmat.cols;
		bInfo->bmiHeader.biHeight = srcmat.rows;
		bInfo->bmiHeader.biPlanes = 1;
		bInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bInfo->bmiHeader.biCompression = BI_RGB;
		bInfo->bmiHeader.biClrImportant = 0;
		bInfo->bmiHeader.biClrUsed = 0;
		bInfo->bmiHeader.biSizeImage = 0;
		bInfo->bmiHeader.biXPelsPerMeter = 0;

		for (int i = 0; i < 256; i++)
		{
			bInfo->bmiColors[i].rgbRed = i;
			bInfo->bmiColors[i].rgbGreen = i;
			bInfo->bmiColors[i].rgbBlue = i;
			bInfo->bmiColors[i].rgbReserved = 0;


		}

	};


	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:

	bool IsAcquiringImages() const { return mAcquiringImages; }
	afx_msg void OnBnClickedConnectButton();
	afx_msg void OnBnClickedDisconnectButton();
	afx_msg void OnBnClickedPlay();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
