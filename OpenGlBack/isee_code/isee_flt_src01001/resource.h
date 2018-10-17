/********************************************************************

	resource.h - ISee图像浏览器—图像处理模块图像旋转处理资源头文件

    版权所有(C) VCHelp-coPathway-ISee workgroup 2000 all member's

    这一程序是自由软件，你可以遵照自由软件基金会出版的GNU 通用许可证
	条款来修改和重新发布这一程序。或者用许可证的第二版，或者（根据你
	的选择）用任何更新的版本。

    发布这一程序的目的是希望它有用，但没有任何担保。甚至没有适合特定
	目地的隐含的担保。更详细的情况请参阅GNU通用许可证。

    你应该已经和程序一起收到一份GNU通用许可证(GPL)的副本。如果还没有，
	写信给：
    The Free Software Foundation, Inc.,  675  Mass Ave,  Cambridge,
    MA02139,  USA

	如果你在使用本软件时有什么问题或建议，用以下地址可以与我们取得联
	系：
		http://isee.126.com
		http://www.vchelp.net
	或：
		iseesoft@china.com

	作者：临风
   e-mail:ringphone@sina.com

   功能实现：图像旋转设置对话框元素标识

	文件版本：
		Build 00617
		Date  2000-6-17

********************************************************************/

/////////位图标识////////////////
#define IDB_BKG				4201

/////////图标标识////////////////
#define IDI_ISEE				4100
#define IDI_VFLIP				4104
#define IDI_HFLIP				4103
#define IDI_TURNRIGHT		4102
#define IDI_TURNLEFT			4101
#define IDI_EGG				4105
#define IDI_POS				4106

//////////对话框标识////////////
#define IDD_ROTATE			4001
#define IDD_RESIZE			4002

//////////对话框元素标识/////////

/////旋转/////////
#define IDC_PREVIEW			4301
#define IDC_TURNLEFT			4302
#define IDC_TURNRIGHT		4303
#define IDC_HFLIP				4305
#define IDC_VFLIP				4306
#define IDC_IMAGENAME		4307

////重定义尺寸////
#define IDC_RS_ORGSIZE		4311
#define IDC_RS_MAXSIZE		4312
#define IDC_RS_WIDTH			4313
#define IDC_RS_HEIGHT		4314
#define IDC_RS_AUTO			4317
#define IDC_RS_PER			4318
#define IDC_RS_PERUD			4319
#define IDC_RS_TILED			4320
#define IDC_RS_CENTER		4321
#define IDC_RS_KEEPED		4322
#define IDC_RS_STRETCH		4323
#define IDC_RS_BKGCOLOR		4324
#define IDC_RS_SELBKGCOLOR	4325
#define IDC_RS_PREVBKGCOLOR 4326

//更改以下数值请注意保持数字连号
#define IDC_RS_KEEP_UL		4327   //必须保证该项为最小值
#define IDC_RS_KEEP_UM		4328
#define IDC_RS_KEEP_UR		4329
#define IDC_RS_KEEP_MR		4330
#define IDC_RS_KEEP_MM		4331
#define IDC_RS_KEEP_ML		4332
#define IDC_RS_KEEP_DM		4333
#define IDC_RS_KEEP_DR		4334
#define IDC_RS_KEEP_DL		4335   //必须保证该项为最大值
//连号条件结束

#define IDC_RS_GROUP_RESIZE 4336
#define IDC_RS_GROUP_PROP	 4337

