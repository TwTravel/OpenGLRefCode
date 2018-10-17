// testView.cpp : implementation of the CTestView class
//

#include "stdafx.h"
#include "test.h"

#include "mainfrm.h"
#include <shlobj.h>
#include "testDoc.h"
#include "testView.h"

#include "ce.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTestView

IMPLEMENT_DYNCREATE(CTestView, CView)

BEGIN_MESSAGE_MAP(CTestView, CView)
	//{{AFX_MSG_MAP(CTestView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_COMMAND(ID_FILE_BEGIN, OnFileBegin)
	ON_COMMAND(ID_FILE_PAUSE, OnFilePause)
	ON_COMMAND(ID_FILE_STOP, OnFileStop)
	ON_COMMAND(ID_TOOLS_EXP, OnToolsExp)
	ON_COMMAND(ID_SPE_1, OnSpe1)
	ON_COMMAND(ID_SPE_2, OnSpe2)
	ON_COMMAND(ID_SPE_3, OnSpe3)
	ON_COMMAND(ID_SPE_4, OnSpe4)
	ON_COMMAND(ID_SPE_5, OnSpe5)
	ON_COMMAND(ID_SPE_6, OnSpe6)
	ON_COMMAND(ID_SPE_7, OnSpe7)
	ON_UPDATE_COMMAND_UI(ID_SPE_1, OnUpdateSpe1)
	ON_UPDATE_COMMAND_UI(ID_SPE_2, OnUpdateSpe2)
	ON_UPDATE_COMMAND_UI(ID_SPE_3, OnUpdateSpe3)
	ON_UPDATE_COMMAND_UI(ID_SPE_4, OnUpdateSpe4)
	ON_UPDATE_COMMAND_UI(ID_SPE_5, OnUpdateSpe5)
	ON_UPDATE_COMMAND_UI(ID_SPE_6, OnUpdateSpe6)
	ON_UPDATE_COMMAND_UI(ID_SPE_7, OnUpdateSpe7)
	ON_UPDATE_COMMAND_UI(ID_FILE_BEGIN, OnUpdateFileBegin)
	ON_UPDATE_COMMAND_UI(ID_FILE_PAUSE, OnUpdateFilePause)
	ON_UPDATE_COMMAND_UI(ID_FILE_STOP, OnUpdateFileStop)
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_TOOLS_DIRECTOR, OnToolsDirector)
	ON_COMMAND(ID_FILE_PREV, OnFilePrev)
	ON_COMMAND(ID_FILE_NEXT, OnFileNext)
	//}}AFX_MSG_MAP
	ON_COMMAND_RANGE(TESTPOPMENUREF_BEGIN, TESTPOPMENUREF_END, OnPluginSel)
	ON_UPDATE_COMMAND_UI_RANGE(TESTPOPMENUREF_BEGIN, TESTPOPMENUREF_END, OnUpdatePluginSel)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestView construction/destruction

CTestView::CTestView()
{
	m_hDrawDib = DrawDibOpen();

	lpbmi = (LPBITMAPINFO)::malloc(sizeof(BITMAPINFOHEADER)+sizeof(RGBQUAD)*256);

	m_mark = false;
	pinfo = 0;
}

CTestView::~CTestView()
{
	if (lpbmi)
		free(lpbmi);
	if (m_hDrawDib)
		DrawDibClose(m_hDrawDib);
	if (pinfo)
		isirw_free_INFOSTR(pinfo);
}

BOOL CTestView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

BOOL CTestView::OnEraseBkgnd(CDC* pDC) 
{
	return CView::OnEraseBkgnd(pDC);
}

void CTestView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();

	if (!lpbmi)
		return;

	// 确定当前图像读写插件的接口函数地址
	if (((fn_info=GetDocument()->cpi.get_info_fun_addr()) == 0) || 
		((fn_load=GetDocument()->cpi.get_load_fun_addr()) == 0) || 
		((fn_save=GetDocument()->cpi.get_save_fun_addr()) == 0))
	{
		m_mark = false; return;
	}
	else
		m_mark = true;

	int		i;

	cmenu.Attach(::GetMenu(GetTopLevelFrame()->GetSafeHwnd()));

	if (cpopmenu.CreatePopupMenu())
	{
		for (i=0;i<GetDocument()->cpi.get_plugin_nums();i++)
			cpopmenu.AppendMenu(MF_ENABLED|MF_STRING, TESTPOPMENUREF_BEGIN+i, (LPCTSTR)((GetDocument()->cpi.get_plugin_tab())[i].plugin_info->irwp_name));

		pimenu = cmenu.GetSubMenu(1);
		pimenu->InsertMenu(0, MF_POPUP|MF_BYPOSITION, (UINT)(cpopmenu.m_hMenu), (LPCTSTR)"设置当前插件");
	}
	else
	{
		cmenu.Detach();
		m_mark = false;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CTestView drawing

void CTestView::OnDraw(CDC* pDC)
{

	CTestDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if (m_mark == FALSE)
		return;

	if ((pinfo)&&(lpbmi))
	{
		GetTopLevelFrame()->PostMessage(WM_FILENUM_DISP, (WPARAM)(pDoc->cft.get_curr_index()+1), (LPARAM)(pDoc->cft.get_file_nums()));
		if ((pDoc->cft.get_file_nums()) == 0)
			return;

		GetTopLevelFrame()->PostMessage(WM_INFO_DISP, (WPARAM)(pDoc->cpi.get_cur_plugin_info()->irwp_name), (LPARAM)pinfo);
		pDoc->SetTitle(pDoc->cft.get_file_name(pDoc->cft.get_curr_index()));
		
		StretchDIBits(pDC->GetSafeHdc(), 
			10, 10, lpbmi->bmiHeader.biWidth, abs(lpbmi->bmiHeader.biHeight), 
			0, 0, lpbmi->bmiHeader.biWidth, abs(lpbmi->bmiHeader.biHeight), 
			pinfo->p_bit_data, lpbmi, DIB_RGB_COLORS, SRCCOPY);
		
		/* DrawDib显示函数：
		DrawDibDraw(m_hDrawDib, pDC->GetSafeHdc(), 0, 0, 
			pinfo->width, pinfo->height,
			(LPBITMAPINFOHEADER)lpbmi, 
			(LPVOID)pinfo->p_bit_data, 
			0, 0, pinfo->width, pinfo->height, 
			DDF_HALFTONE);
		*/
	}
}

/////////////////////////////////////////////////////////////////////////////
// CTestView diagnostics

#ifdef _DEBUG
void CTestView::AssertValid() const
{
	CView::AssertValid();
}

void CTestView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CTestDoc* CTestView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTestDoc)));
	return (CTestDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTestView message handlers

int CTestView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	return 0;
}

void CTestView::OnDestroy() 
{
	cmenu.Detach();

	GetDocument()->ctr.destroy(this->GetSafeHwnd());

	CView::OnDestroy();
}

void CTestView::OnTimer(UINT nIDEvent) 
{
	ISADDR			isaddr;
	LPINFOSTR		pinfo_tmp;
	
	int				index, wanmark, imgnum;
	int				bits;
	
	if ((GetDocument()->ctr.get_playmark() == PLAYSTAT_PLAY)||(nIDEvent == (IDT_TIMEOUT+1)))
	{
		if (GetDocument()->cft.get_file_nums() == 0)
			return;

		if (GetDocument()->play_order == 0)
			index = GetDocument()->cft.get_next_index();
		else
			index = GetDocument()->cft.get_prev_index();

		if (m_mark == false)
		{
			CView::OnTimer(nIDEvent); return;
		}

		if (pinfo)
		{
			isirw_free_INFOSTR(pinfo); pinfo = 0;
		}

		// 设置ISeeIO入口参数结构 － isaddr
		isio_open_param(&isaddr, ISEEIO_CONFER_LOC_FILE, (const char *)(GetDocument()->cft.get_file_name(index)), 0, 0);
		// 申请图像数据表
		if ((pinfo=isirw_alloc_INFOSTR()) == 0)
		{
			CView::OnTimer(nIDEvent); return;
		}
		
		if (GetDocument()->cft.get_file_info(index)->played == false)
			wanmark = 1;
		else
			wanmark = 0;

		GetDocument()->cft.get_file_info(index)->played = true;
		
		//###############################################################
		// 在此设置断点可跟踪插件的“读图像信息”功能：
		if ((*fn_info)(&isaddr, pinfo) != ER_SUCCESS)
		{
			GetDocument()->cft.get_file_info(index)->avail  = false;
			isirw_free_INFOSTR(pinfo); 
			pinfo = 0;
			CView::OnTimer(nIDEvent); 
			return;
		}


	
		
		//###############################################################
		// 在此设置断点可跟踪插件的“读图像”功能：
		if ((*fn_load)(&isaddr, pinfo) == ER_SUCCESS)
		{
			bits = pinfo->bitcount;

			imgnum = pinfo->imgnumbers;
			if (imgnum > 1)
			{
				imgnum += 0;					// 子图象测试点
			}

			/* ISee流转WINDOWS平台格式 */
			switch (pinfo->bitcount)
			{
			case	1:
				_1_to_cross(pinfo);				// 象素顺序转换
				break;
			case	2:
			case	3:
				pinfo_tmp = _23to4(pinfo);		// 位深度转换
				isirw_free_INFOSTR(pinfo);
				pinfo = pinfo_tmp;
				pinfo_tmp = 0;
			case	4:
				_4_to_cross(pinfo);
				if (pinfo->order == 0)
					_v_conv(pinfo);				// 图像放置顺序转换
				break;
			case	5:
			case	6:
			case	7:
				pinfo_tmp = _567to8(pinfo);  
				isirw_free_INFOSTR(pinfo);
				pinfo = pinfo_tmp;
				pinfo_tmp = 0;
				break;
			case	8:
				break;
			case	9:
			case	10:
			case	11:
			case	12:
			case	13:
			case	14:
			case	15:
			case	16:
				pinfo_tmp = _9_10_11_12_13_14_15to16(pinfo);
				isirw_free_INFOSTR(pinfo);
				pinfo = pinfo_tmp;
				pinfo_tmp = 0;
				break;		
			case	17:
			case	18:
			case	19:
			case	20:
			case	21:
			case	22:
			case	23:
			case	24:
				pinfo_tmp = _17_18_19_20_21_22_23to24(pinfo);
				isirw_free_INFOSTR(pinfo);
				pinfo = pinfo_tmp;
				pinfo_tmp = 0;
				break;
			case	25:
			case	26:
			case	27:
			case	28:
			case	29:
			case	30:
			case	31:
				pinfo_tmp = _25_26_27_28_29_30_31to32(pinfo);
				isirw_free_INFOSTR(pinfo);
				pinfo = pinfo_tmp;
				pinfo_tmp = 0;
				break;
			case	32:
				if ((pinfo->r_mask == 0xff0000)&&(pinfo->g_mask == 0xff00)&&(pinfo->b_mask == 0xff))
					;
				else
				{
					pinfo_tmp = _25_26_27_28_29_30_31to32(pinfo);
					isirw_free_INFOSTR(pinfo);
					pinfo = pinfo_tmp;
					pinfo_tmp = 0;
				}
			}

			if (!pinfo)
				return;

			/* 制作BMP格式图像 */
			lpbmi->bmiHeader.biBitCount = (unsigned short)pinfo->bitcount;
			lpbmi->bmiHeader.biClrImportant = 0;
			lpbmi->bmiHeader.biClrUsed = 0;
			lpbmi->bmiHeader.biCompression = BI_RGB;
			lpbmi->bmiHeader.biHeight = (pinfo->order==0) ? -(long)pinfo->height:pinfo->height;
			lpbmi->bmiHeader.biPlanes = 1;
			lpbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			lpbmi->bmiHeader.biSizeImage = 0;
			lpbmi->bmiHeader.biWidth = pinfo->width;
			lpbmi->bmiHeader.biXPelsPerMeter = 0;
			lpbmi->bmiHeader.biYPelsPerMeter = 0;
			
			if (pinfo->bitcount <= 8)
				memmove((void*)(lpbmi->bmiColors), (const void *)(pinfo->palette), sizeof(RGBQUAD)*pinfo->pal_count);
			else
			{
				*(unsigned long *)&(lpbmi->bmiColors[0]) = pinfo->r_mask;
				*(unsigned long *)&(lpbmi->bmiColors[1]) = pinfo->g_mask;
				*(unsigned long *)&(lpbmi->bmiColors[2]) = pinfo->b_mask;
			}

			((FILEINFO *)(GetDocument()->cft.get_file_info(GetDocument()->cft.get_curr_index())))->avail = true;
			((FILEINFO *)(GetDocument()->cft.get_file_info(GetDocument()->cft.get_curr_index())))->played= true;

			/* 以下为保存功能测试代码：
			if (pinfo->bitcount == 24)
			{
				ISADDR		isave;
				SAVESTR		save_str;
				char save_buf[MAX_PATH];

				// WINDOWS格式图像转换为ISee流格式：
				switch (pinfo->bitcount)
				{
				case	1:
					_1_to_cross(pinfo);
					break;
				case	4:
					_4_to_cross(pinfo);
					break;
				}

				strcpy((char*)save_buf, (const char *)isaddr.name);
				strcat((char*)save_buf, (const char *)".rle");
				isio_open_param(&isave,  ISEEIO_CONFER_LOC_FILE, (const char *)save_buf, 0, 0);
				save_str.para_value[0] = 1;
				//###############################################################<<<
				// 在此设置断点可跟踪插件的“写图像”功能：
				(*fn_save)(&isave, pinfo, &save_str);
			}
			/**/
				
			GetDocument()->cft.get_file_info(index)->avail  = true;
		}
		else
		{
			GetDocument()->cft.get_file_info(index)->avail  = false;
			isirw_free_INFOSTR(pinfo); 
			pinfo = 0;
			CView::OnTimer(nIDEvent); 
			return;
		}

		this->UpdateWindow();
		this->RedrawWindow();
	}

	CView::OnTimer(nIDEvent);
}

void CTestView::OnFileBegin() 
{
	GetDocument()->ctr.play(this->GetSafeHwnd());
}

void CTestView::OnFilePause() 
{
	GetDocument()->ctr.pause();
}

void CTestView::OnFileStop() 
{
	GetDocument()->ctr.stop(this->GetSafeHwnd());
}

void CTestView::OnToolsExp() 
{
	// TODO: Add your command handler code here
	
}


void CTestView::OnSpe1() 
{
	GetDocument()->ctr.set_timeout(10000, this->GetSafeHwnd());	// 10s
}

void CTestView::OnSpe2() 
{
	GetDocument()->ctr.set_timeout(5000, this->GetSafeHwnd());	// 5s
}

void CTestView::OnSpe3() 
{
	GetDocument()->ctr.set_timeout(3000, this->GetSafeHwnd());	// 3s
}

void CTestView::OnSpe4() 
{
	GetDocument()->ctr.set_timeout(1000, this->GetSafeHwnd());	// 1s
}

void CTestView::OnSpe5() 
{
	GetDocument()->ctr.set_timeout(500, this->GetSafeHwnd());	// 500ms
}

void CTestView::OnSpe6() 
{
	GetDocument()->ctr.set_timeout(300, this->GetSafeHwnd());	// 300ms
}

void CTestView::OnSpe7() 
{
	GetDocument()->ctr.set_timeout(100, this->GetSafeHwnd());	// 100ms
}


void CTestView::OnUpdateSpe1(CCmdUI* pCmdUI) 
{
	if (GetDocument()->ctr.get_timeout() == 10000)
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);
}

void CTestView::OnUpdateSpe2(CCmdUI* pCmdUI) 
{
	if (GetDocument()->ctr.get_timeout() == 5000)
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);
}

void CTestView::OnUpdateSpe3(CCmdUI* pCmdUI) 
{
	if (GetDocument()->ctr.get_timeout() == 3000)
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);
}

void CTestView::OnUpdateSpe4(CCmdUI* pCmdUI) 
{
	if (GetDocument()->ctr.get_timeout() == 1000)
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);
}

void CTestView::OnUpdateSpe5(CCmdUI* pCmdUI) 
{
	if (GetDocument()->ctr.get_timeout() == 500)
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);
}

void CTestView::OnUpdateSpe6(CCmdUI* pCmdUI) 
{
	if (GetDocument()->ctr.get_timeout() == 300)
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);
}

void CTestView::OnUpdateSpe7(CCmdUI* pCmdUI) 
{
	if (GetDocument()->ctr.get_timeout() == 100)
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);
}

void CTestView::OnUpdateFileBegin(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);

	if (GetDocument()->ctr.get_playmark() == PLAYSTAT_STOP)
		pCmdUI->SetCheck(0);
	else if (GetDocument()->ctr.get_playmark() == PLAYSTAT_PAUSE)
		pCmdUI->SetCheck(0);
	else if (GetDocument()->ctr.get_playmark() == PLAYSTAT_PLAY)
		pCmdUI->SetCheck(1);
	else
		ASSERT(0);
}

void CTestView::OnUpdateFilePause(CCmdUI* pCmdUI) 
{
	if (GetDocument()->ctr.get_playmark() == PLAYSTAT_STOP)
	{
		pCmdUI->Enable(FALSE);
		pCmdUI->SetCheck(0);
	}
	else if (GetDocument()->ctr.get_playmark() == PLAYSTAT_PAUSE)
	{
		pCmdUI->Enable(TRUE);
		pCmdUI->SetCheck(1);
	}
	else if (GetDocument()->ctr.get_playmark() == PLAYSTAT_PLAY)
	{
		pCmdUI->Enable(TRUE);
		pCmdUI->SetCheck(0);
	}
}

void CTestView::OnUpdateFileStop(CCmdUI* pCmdUI)
{
	if (GetDocument()->ctr.get_playmark() == PLAYSTAT_STOP)
	{
		pCmdUI->Enable(FALSE);
		pCmdUI->SetCheck(0);
	}
	else if (GetDocument()->ctr.get_playmark() == PLAYSTAT_PAUSE)
	{
		pCmdUI->Enable(TRUE);
		pCmdUI->SetCheck(0);
	}
	else if (GetDocument()->ctr.get_playmark() == PLAYSTAT_PLAY)
	{
		pCmdUI->Enable(TRUE);
		pCmdUI->SetCheck(0);
	}
}

void CTestView::OnPluginSel(UINT nID)
{
	GetDocument()->ctr.stop(this->GetSafeHwnd());

	GetDocument()->cpi.set_cur_plugin_index(nID-TESTPOPMENUREF_BEGIN);
	GetDocument()->cft.disassemble();
	GetDocument()->cft.compages((char*)(LPCTSTR)GetDocument()->cpath, GetDocument()->cpi.get_cur_plugin_info());

	// 确定当前图像读写插件的接口函数地址
	if (((fn_info=GetDocument()->cpi.get_info_fun_addr()) == 0) || 
		((fn_load=GetDocument()->cpi.get_load_fun_addr()) == 0) || 
		((fn_save=GetDocument()->cpi.get_save_fun_addr()) == 0))
		m_mark = false;
	else
		m_mark = true;

	this->UpdateWindow();
	this->RedrawWindow();
}

void CTestView::OnUpdatePluginSel(CCmdUI* pCmdUI)
{
	pimenu = cmenu.GetSubMenu(1)->GetSubMenu(0);

	for (int i=0;i<GetDocument()->cpi.get_plugin_nums();i++)
	{
		if (i == GetDocument()->cpi.get_cur_plugin_index())
			pimenu->CheckMenuItem(i, MF_CHECKED|MF_BYPOSITION);
		else
			pimenu->CheckMenuItem(i, MF_UNCHECKED|MF_BYPOSITION);
	}
}

void CTestView::OnToolsDirector() 
{
	FILE		*fp;
	char		caSubPathName[_MAX_PATH];
	char		caFullPathName[_MAX_PATH];
	BROWSEINFO	brFolder;

	brFolder.hwndOwner = this->m_hWnd;
	brFolder.pidlRoot  = NULL;
	brFolder.pszDisplayName = caSubPathName;
	brFolder.lpszTitle = "请选择图像文件所在的目录：";
	brFolder.ulFlags   = BIF_RETURNONLYFSDIRS;
	brFolder.lpfn      = NULL;

	if (SHGetPathFromIDList(SHBrowseForFolder(&brFolder), caFullPathName))
	{
		GetDocument()->ctr.stop(this->GetSafeHwnd());

		GetDocument()->cpath = (LPCTSTR)caFullPathName;
		GetDocument()->cft.disassemble();
		GetDocument()->cft.compages((char*)caFullPathName, GetDocument()->cpi.get_cur_plugin_info());

		if (fp=fopen((const char *)GetDocument()->ini_path, "w"))
		{
			fputs((const char *)(LPCTSTR)GetDocument()->cpath, fp);
			fclose(fp);
		}
	}
}

void CTestView::OnFilePrev() 
{
	bool order = GetDocument()->play_order;

	GetDocument()->play_order = 1;

	if (GetDocument()->ctr.get_playmark() == PLAYSTAT_PLAY)
		GetDocument()->ctr.pause();

	OnTimer((IDT_TIMEOUT+1));

	GetDocument()->play_order = order;
}

void CTestView::OnFileNext() 
{
	bool order = GetDocument()->play_order;
	
	GetDocument()->play_order = 0;
	
	if (GetDocument()->ctr.get_playmark() == PLAYSTAT_PLAY)
		GetDocument()->ctr.pause();
	
	OnTimer((IDT_TIMEOUT+1));
	
	GetDocument()->play_order = order;
}
