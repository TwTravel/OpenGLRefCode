#include <ringlib\ringdib.h>
#include "gol_proc.h"
#include "testdllres.h"
#include "testdll.h"

HINSTANCE hInst;
RDib rdib;
RECT rt;
int t,l,r,b;
IMAGEPROCSTR info;
COLORREF* lpRData=NULL;

BOOL InitApplication(HINSTANCE hInstance)
{
	WNDCLASS  wc;

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = MainWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPICON));
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = MAKEINTRESOURCE(IDM_MENU);
	wc.lpszClassName = "MyApp"; // Name used in call to CreateWindow.

	return (RegisterClass(&wc));

}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND  hWnd;

	hWnd = CreateWindow(
		"MyApp",
		"ISee图象处理模块测试",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,//horizontal position.
		CW_USEDEFAULT,//vertical position.
		CW_USEDEFAULT,// Default width.
		CW_USEDEFAULT,// Default height.
		NULL,
		NULL,// Use the window class menu.
		hInstance,
		NULL
		);
	if (!hWnd)
		return (FALSE);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	return (TRUE);
}

#pragma argsused
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hNull,LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	hInst=hInstance;

	if (!InitApplication(hInstance))
		return (FALSE);

	if (!InitInstance(hInstance, nCmdShow))
		return (FALSE);

	while (GetMessage(&msg,NULL,0,0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (msg.wParam);
}

#pragma argsused
LRESULT APIENTRY MainWndProc(HWND hWnd,UINT uMsg,UINT wParam,LONG lParam)
{
	switch(uMsg)
	{
		case WM_CREATE:
         InitRingLib(hInst,hWnd);
         DIBRegFormat(GIF);
         DIBRegFormat(JPG);
         DIBRegFormat(PCX);
      	info.psLineAddr=info.pdLineAddr=NULL;
         break;
		case WM_PAINT:
         if(!rdib.Paint())
		 		PaintNull(hWnd);
			break;
      case MYWM_ICON:
      {
      	info.hParentWnd=hWnd;
         info.comm=wParam;
		   info._psbdata=(unsigned char*)rdib.m_Bits;
		   info.sImageInfo.width=rdib.m_width;
      	info.sImageInfo.height=rdib.m_height;
		   info.sImageInfo.bitperpix=32;
         info.sImageInfo.byteperline=rdib.m_width*32;

         if(info.psLineAddr==NULL)
         {
         	info.psLineAddr=(unsigned long**)New(info.sImageInfo.height*sizeof(long));
				if(info.psLineAddr==NULL)
				{
   				Errmsg("内存错误");
         	   break;
				}
   		}

         for(int i=0;i<info.sImageInfo.height;i++)
   			info.psLineAddr[i]=(unsigned long*)rdib.m_Bits+info.sImageInfo.byteperline*i;

         LPSTR szDllname;
         if(wParam > PCM_PROP)
         	szDllname = "flt_prop.dll";
         if(wParam > PCM_MASS)
         	szDllname = "flt_mass.dll";
         if(wParam > PCM_STEFF)
         	szDllname = "flt_steff.dll";
         if(wParam > PCM_ANIEFF)
         	szDllname = "flt_anieff.dll";

      	if(RunDll(szDllname,&info)==PROCERR_SUCCESS && info.result==PR_SUCCESS)
         {
		     	lpRData=rdib.m_Bits;
      	   rdib.m_width=info.dImageInfo.width;
		      rdib.m_height=info.dImageInfo.height;
      	   rdib.m_Bits=(COLORREF*)info._pdbdata;

		      GetClientRect(hWnd,&rt);
      	   t=0;l=0;r=rt.right;b=rt.bottom;
		      if(r>rdib.m_width)
      	     	r=rdib.m_width;
         	if(b>rdib.m_height)
		        	b=rdib.m_height;
      	   rdib.SetSrc(0,0,r,b);
         	rdib.SetDest(0,0,r,b);

		      rdib.Draw();

          	info.psLineAddr=(unsigned long **)Del(info.psLineAddr);
            info.psLineAddr=info.pdLineAddr;
            info.pdLineAddr=NULL;
         }

         break;
		}
		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case CM_OPEN:
            	lpRData=(COLORREF*)Del(lpRData);
               info.psLineAddr=(unsigned long **)Del(info.psLineAddr);

            	if(rdib.Select())
         		{
         			GetClientRect(hWnd,&rt);
		            t=0;l=0;r=rt.right;b=rt.bottom;
      		      if(r>rdib.m_width)
            			r=rdib.m_width;
		            if(b>rdib.m_height)
      		      	b=rdib.m_height;

            		rdib.SetSrc(0,0,r,b);
		            rdib.SetDest(0,0,r,b);
      		      rdib.Draw();
               }
               break;
            case CM_ROTATE:
            	lpRData=(COLORREF*)Del(lpRData);
               SendMessage(hWnd,MYWM_ICON,PCM_ROTATE,0);
               break;
            case CM_GREYSCALE:
            	lpRData=(COLORREF*)Del(lpRData);
            	SendMessage(hWnd,MYWM_ICON,PCM_GREYSCALE,0);
            	break;
            case CM_RESIZE:
            	lpRData=(COLORREF*)Del(lpRData);
            	SendMessage(hWnd,MYWM_ICON,PCM_RESIZE,0);
            	break;
            case CM_SPLIT:
            	lpRData=(COLORREF*)Del(lpRData);
            	SendMessage(hWnd,MYWM_ICON,PCM_SPLIT,0);
            	break;
            case CM_PERLINNOISE:
            	lpRData=(COLORREF*)Del(lpRData);
            	SendMessage(hWnd,MYWM_ICON,PCM_PERLINNOISE,0);
            	break;
            case CM_GAME_PINTU:
					lpRData=(COLORREF*)Del(lpRData);
            	SendMessage(hWnd,MYWM_ICON,PCM_GAME_PINTU,0);
            	break;
			}
			break;
      case WM_RBUTTONDOWN:
      	SendMessage(hWnd,MYWM_ICON,PCM_GETWRITERMESS,0);
/*      	rdib.SetSrc(0,0);
         rdib.SetDest(0,0,1880,460);
         rdib.Draw();
*/         break;
      case WM_LBUTTONDOWN:
         break;
		case WM_DESTROY:
   	{
         QuitRingDib();
         Del(lpRData);
         Del(info.psLineAddr);
			PostQuitMessage(0);
      }
		default:
			return (DefWindowProc(hWnd,uMsg,wParam,lParam));
	}
	return 0L;
}
