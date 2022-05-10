
// IR140Dlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "IR140.h"
#include "IR140Dlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CIR140Dlg dialog



CIR140Dlg::CIR140Dlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_IR140_DIALOG, pParent)
	, mAcquiringImages(false)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	mDevice = NULL;
	mStream = NULL;
	mPipeline = NULL;

	


}

CIR140Dlg::~CIR140Dlg()
{


}

void CIR140Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CIR140Dlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_CONNECT_BUTTON, &CIR140Dlg::OnBnClickedConnectButton)
	ON_BN_CLICKED(IDC_DISCONNECT_BUTTON, &CIR140Dlg::OnBnClickedDisconnectButton)
	ON_BN_CLICKED(IDC_PLAY, &CIR140Dlg::OnBnClickedPlay)
END_MESSAGE_MAP()


// CIR140Dlg message handlers

BOOL CIR140Dlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowText(_T("140 EO"));




	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon



	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CIR140Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CIR140Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();

	}
	if (!mAcquiringImages)
	{
		HDC hdc = ::GetDC(this->m_hWnd);
		CRect lDisplayRect;
		GetDlgItem(IDC_DISPLAYPOS)->GetClientRect(&lDisplayRect);
		GetDlgItem(IDC_DISPLAYPOS)->ClientToScreen(&lDisplayRect);
		ScreenToClient(&lDisplayRect);
		HBRUSH hbrush = GetSysColorBrush(COLOR_3DDKSHADOW);
		FillRect(hdc, lDisplayRect, hbrush);
		::ReleaseDC(this->m_hWnd, hdc);
		DeleteObject(hbrush);
	}



	//EnableInterface();
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CIR140Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CIR140Dlg::OnBnClickedConnectButton()
{

	CRect lDisplayRect;
	GetDlgItem(IDC_DISPLAYPOS)->GetClientRect(&lDisplayRect);
	GetDlgItem(IDC_DISPLAYPOS)->ClientToScreen(&lDisplayRect);
	ScreenToClient(&lDisplayRect);
	
	//selecting device
	if (PvSelectDevice(&mConnectionID))
	{
		mDevice = ConnectToDevice(mConnectionID);
		if (mDevice != NULL)
		{
			mStream = OpenStream(mConnectionID);
			if (mStream != NULL)
			{
				ConfigureStream(mDevice, mStream);
				mPipeline = CreatePipeline(mDevice, mStream);
				if (mPipeline)
				{
					EnableInterface();
					DParg.aAcquiringImages = true;
					DParg.aDevice = mDevice;
					DParg.aPipeline = mPipeline;
					DParg.aStream = mStream;

					pThread = AfxBeginThread(AcquireImages, &DParg, THREAD_PRIORITY_TIME_CRITICAL, 0);

				}
			}
		}
	}

}



void CIR140Dlg::EnableInterface()
{
    // This method can be called really early or late when the window is not created
    if (GetSafeHwnd() == 0)
    {
        return;
    }




    bool lConnected = (mDevice != NULL) && mDevice->IsConnected();

    GetDlgItem(IDC_CONNECT_BUTTON)->EnableWindow(!lConnected);
    GetDlgItem(IDC_DISCONNECT_BUTTON)->EnableWindow(lConnected);
	GetDlgItem(IDC_PLAY)->EnableWindow(lConnected);
	GetDlgItem(IDC_STOP)->EnableWindow(lConnected);
	GetDlgItem(IDC_COLD)->EnableWindow(lConnected);
	GetDlgItem(IDC_HOT)->EnableWindow(lConnected);
	GetDlgItem(IDC_BP)->EnableWindow(lConnected);
	GetDlgItem(IDC_BPO)->EnableWindow(lConnected);
	GetDlgItem(IDC_OFFSET)->EnableWindow(lConnected);



}






void CIR140Dlg::OnBnClickedDisconnectButton()
{
	

	AfxEndThread(0);
	DParg.aAcquiringImages = false;

	// Stop the pipeline
	mPipeline->Stop();

	delete mPipeline;

	mStream->Close();
	PvStream::Free(mStream);
	mDevice->Disconnect();
	PvDevice::Free(mDevice);


	//// Tell the device to stop sending images.
	//cout << "Sending AcquisitionStop command to the device" << endl;
	//mDeviceParams->ExecuteCommand("AcquisitionStop");

	//// Disable stream after sending the AcquisitionStop command.
	//cout << "Disable streaming on the controller." << endl;
	//mDevice->StreamDisable();

	//// Abort all buffers from the stream, dequeue.
	//cout << "Aborting buffers still in stream" << endl;
	//mStream->AbortQueuedBuffers();
	//while (mStream->GetQueuedBufferCount() > 0)
	//{
	//	PvBuffer* lBuffer = NULL;
	//	PvResult lOperationResult;

	//	mStream->RetrieveBuffer(&lBuffer, &lOperationResult);
	//}

	//ReleaseBuffers();

	//// Now close the stream. Also optional but nice to have.
	//cout << "Closing stream" << endl;
	//mStream->Close();

	//// Disconnect the device. Optional, still nice to have.
	//cout << "Disconnecting device" << endl;
	//mDevice->Disconnect();

	//// Free the objects allocated by PvDevice and PvStream factory methods
	//PvStream::Free(mStream);
	//PvDevice::Free(mDevice);

	//mDisplayThread->StartThread(mStream, mDevice);
}





void CIR140Dlg::OnBnClickedPlay()
{


	



	EnableInterface();

}


BOOL CIR140Dlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_ERASEBKGND)
		return TRUE;

	return CDialog::PreTranslateMessage(pMsg);
}


PvDevice* CIR140Dlg::ConnectToDevice(const PvString& aConnectionID)
{
	PvDevice* lDevice;
	PvResult lResult;

	// Connect to the GigE Vision or USB3 Vision device
	//cout << "Connecting to device." << endl;
	lDevice = PvDevice::CreateAndConnect(aConnectionID, &lResult);
	if (lDevice == NULL)
	{
		//cout << "Unable to connect to device." << endl;
	}

	return lDevice;
}

PvStream* CIR140Dlg::OpenStream(const PvString& aConnectionID)
{
	PvStream* lStream;
	PvResult lResult;

	// Open stream to the GigE Vision or USB3 Vision device
	//cout << "Opening stream from device." << endl;
	lStream = PvStream::CreateAndOpen(aConnectionID, &lResult);
	if (lStream == NULL)
	{
		//cout << "Unable to stream from device." << endl;
	}

	return lStream;
}

void CIR140Dlg::ConfigureStream(PvDevice* aDevice, PvStream* aStream)
{
	// If this is a GigE Vision device, configure GigE Vision specific streaming parameters
	PvDeviceGEV* lDeviceGEV = dynamic_cast<PvDeviceGEV*>(aDevice);
	if (lDeviceGEV != NULL)
	{
		PvStreamGEV* lStreamGEV = static_cast<PvStreamGEV*>(aStream);

		// Negotiate packet size
		lDeviceGEV->NegotiatePacketSize();

		// Configure device streaming destination
		lDeviceGEV->SetStreamDestination(lStreamGEV->GetLocalIPAddress(), lStreamGEV->GetLocalPort());
	}
}

PvPipeline* CIR140Dlg::CreatePipeline(PvDevice* aDevice, PvStream* aStream)
{
	// Create the PvPipeline object
	PvPipeline* lPipeline = new PvPipeline(aStream);

	if (lPipeline != NULL)
	{
		// Reading payload size from device
		uint32_t lSize = aDevice->GetPayloadSize();

		// Set the Buffer count and the Buffer size
		lPipeline->SetBufferCount(BUFFER_COUNT);
		lPipeline->SetBufferSize(lSize);
	}

	return lPipeline;
}


UINT CIR140Dlg::AcquireImages(LPVOID lParam)
{
	CIR140Dlg* This = (CIR140Dlg*)lParam;
	DisplayThreadArg* pArg = (DisplayThreadArg*)lParam;
	BITMAPINFO* bitmapInfo = new BITMAPINFO;
	cv::Mat imageMat8(cv::Size(640, 480), CV_8UC3);




	// Get device parameters need to control streaming
	PvGenParameterArray* lDeviceParams = pArg->aDevice->GetParameters();

	// Map the GenICam AcquisitionStart and AcquisitionStop commands
	PvGenCommand* lStart = dynamic_cast<PvGenCommand*>(lDeviceParams->Get("AcquisitionStart"));
	PvGenCommand* lStop = dynamic_cast<PvGenCommand*>(lDeviceParams->Get("AcquisitionStop"));

	// Note: the pipeline must be initialized before we start acquisition
	//cout << "Starting pipeline" << endl;
	pArg->aPipeline->Start();

	// Get stream parameters
	PvGenParameterArray* lStreamParams = pArg->aStream->GetParameters();

	// Map a few GenICam stream stats counters
	PvGenFloat* lFrameRate = dynamic_cast<PvGenFloat*>(lStreamParams->Get("AcquisitionRate"));
	PvGenFloat* lBandwidth = dynamic_cast<PvGenFloat*>(lStreamParams->Get("Bandwidth"));

	// Enable streaming and send the AcquisitionStart command
	//cout << "Enabling streaming and sending AcquisitionStart command." << endl;
	pArg->aDevice->StreamEnable();
	lStart->Execute();

	char lDoodle[] = "|\\-|-/";
	int lDoodleIndex = 0;
	double lFrameRateVal = 0.0;
	double lBandwidthVal = 0.0;

	// Acquire images until the user instructs us to stop.
	//cout << endl << "<press a key to stop streaming>" << endl;
	while (pArg->aAcquiringImages)
	{
		PvBuffer* lBuffer = NULL;
		PvResult lOperationResult;

		// Retrieve next buffer
		PvResult lResult = pArg->aPipeline->RetrieveNextBuffer(&lBuffer, 1000, &lOperationResult);
		if (lResult.IsOK())
		{
			if (lOperationResult.IsOK())
			{
				PvPayloadType lType;

				//
				// We now have a valid buffer. This is where you would typically process the buffer.
				// -----------------------------------------------------------------------------------------
				// ...

				lFrameRate->GetValue(lFrameRateVal);
				lBandwidth->GetValue(lBandwidthVal);

				// If the buffer contains an image, display width and height.
				uint32_t lWidth = 0, lHeight = 0;
				lType = lBuffer->GetPayloadType();

				//cout << fixed << setprecision(1);
				//cout << lDoodle[lDoodleIndex];
				//cout << " BlockID: " << uppercase << hex << setfill('0') << setw(16) << lBuffer->GetBlockID();
				if (lType == PvPayloadTypeImage)
				{
					// Get image specific buffer interface.
					PvImage* lImage = lBuffer->GetImage();
					lImage->Alloc(640, 480, PvPixelMono16);
					lWidth = lImage->GetWidth();
					lHeight = lImage->GetHeight();
					unsigned char* pimg = lImage->GetDataPointer();

					CRect lDisplayRect;
					AfxGetApp()->m_pMainWnd->GetDlgItem(IDC_DISPLAYPOS)->GetClientRect(&lDisplayRect);
					AfxGetApp()->m_pMainWnd->GetDlgItem(IDC_DISPLAYPOS)->ClientToScreen(&lDisplayRect);
					AfxGetApp()->m_pMainWnd->ScreenToClient(&lDisplayRect);

					HDC hDC = AfxGetApp()->m_pMainWnd->GetDC()->GetSafeHdc();
					
					cv::Mat imageMat(480, 640, CV_16U, pimg, cv::Mat::AUTO_STEP);
					cv::normalize(imageMat, imageMat, 0, 65535, cv::NORM_MINMAX);
					
					double min_val = 0;
					double max_val = 0;

					cv::Point minLoc;
					cv::Point maxLoc;


					minMaxLoc(imageMat, &min_val, &max_val);

					int range = 0;
					range = max_val - min_val;
					if (range == 0)
						range = 1;

					imageMat.convertTo(imageMat8, CV_8UC3, 255.0 / range, -(min_val) * 255.0 / range);

					This->setBitInfo(bitmapInfo, imageMat8);
					SetStretchBltMode(hDC, HALFTONE);
					int tgapH = (lDisplayRect.Height() - 480) / 2;
					StretchDIBits(hDC, lDisplayRect.left, lDisplayRect.top + tgapH / 4, lDisplayRect.Width(), lDisplayRect.Height() - tgapH / 2, 0, 0, imageMat.cols, imageMat.rows, imageMat8.data, bitmapInfo, DIB_RGB_COLORS, SRCCOPY);


				}
				else {
					//cout << " (buffer does not contain image)";
				}
				//cout << "  " << lFrameRateVal << " FPS  " << (lBandwidthVal / 1000000.0) << " Mb/s   \r";
			}
			else
			{
				// Non OK operational result
				//cout << lDoodle[lDoodleIndex] << " " << lOperationResult.GetCodeString().GetAscii() << "\r";
			}

			// Release the buffer back to the pipeline
			pArg->aPipeline->ReleaseBuffer(lBuffer);
		}
		else
		{
			// Retrieve buffer failure
			//cout << lDoodle[lDoodleIndex] << " " << lResult.GetCodeString().GetAscii() << "\r";
		}

		//++lDoodleIndex %= 6;
	}

	

	

	lStop->Execute();

	// Disable streaming on the device
	pArg->aDevice->StreamDisable();

	
	//AfxEndThread(0);

	// Stop the pipeline
	pArg->aPipeline->Stop();
	
	delete pArg->aPipeline;

	pArg->aStream->Close();
	PvStream::Free(pArg->aStream);
	pArg->aDevice->Disconnect();
	PvDevice::Free(pArg->aDevice);
	
	return 0;
}
