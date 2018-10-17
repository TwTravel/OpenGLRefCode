/********************************************************************

	rgbtab.c

	----------------------------------------------------------------
    软件许可证 － GPL
	版权所有 (C) 2001 VCHelp coPathway ISee workgroup.
	----------------------------------------------------------------
	这一程序是自由软件，你可以遵照自由软件基金会出版的GNU 通用公共许
	可证条款来修改和重新发布这一程序。或者用许可证的第二版，或者（根
	据你的选择）用任何更新的版本。

    发布这一程序的目的是希望它有用，但没有任何担保。甚至没有适合特定
	目地的隐含的担保。更详细的情况请参阅GNU通用公共许可证。

    你应该已经和程序一起收到一份GNU通用公共许可证的副本（本目录
	GPL.txt文件）。如果还没有，写信给：
    The Free Software Foundation, Inc.,  675  Mass Ave,  Cambridge,
    MA02139,  USA
	----------------------------------------------------------------
	如果你在使用本软件时有什么问题或建议，请用以下地址与我们取得联系：

			http://isee.126.com
			http://www.vchelp.net
			http://www.chinafcu.com

	或发信到：

			yzfree##sina.com
	----------------------------------------------------------------
	本文件用途：	ISee图像浏览器—XPM图像读写模块彩色表辅助功能文件

	本文件编写人：	YZ			yzfree##yeah.net

	本文件版本：	20125
	最后修改于：	2002-1-25

	注：以上E-Mail地址中的##请用@替换，这样做是为了抵制恶意的E-Mail
	    地址收集软件。
  	----------------------------------------------------------------
	修正历史：

			2002-1		第一次发布


********************************************************************/


#ifndef WIN32
#if defined(_WIN32)||defined(_WINDOWS)
#define WIN32
#endif
#endif /* WIN32 */

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN				/* 缩短windows.h文件的编译时间 */
#include <windows.h>
#endif /* WIN32 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "rgbtab.h"


/* XPM 预定义颜色表（全表） */
static XPMRGBTAB rgbtab[RGBTABSIZE] = {
	{0x00ff0000, 315, "red"},			{0x00d2b48c, 323, "tan"},
	{0x00ff0000, 364, "red1"},			{0x00ee0000, 365, "red2"},
	{0x00cd0000, 366, "red3"},			{0x008b0000, 367, "red4"},
	{0x00ffa54f, 372, "tan1"},			{0x00ee9a49, 373, "tan2"},
	{0x00cd853f, 374, "tan3"},			{0x008b5a2b, 375, "tan4"},
	{0x00ffd700, 422, "gold"},			{0x000000ff, 424, "blue"},
	{0x0000ffff, 427, "cyan"},			{0x00ffc0cb, 434, "pink"},
	{0x00bebebe, 435, "gray"},			{0x00bebebe, 439, "grey"},
	{0x00cd853f, 444, "peru"},			{0x00000080, 446, "navy"},
	{0x00dda0dd, 446, "plum"},			{0x00fffafa, 455, "snow"},
	{0x00ffd700, 471, "gold1"},			{0x00eec900, 472, "gold2"},
	{0x000000ff, 473, "blue1"},			{0x00cdad00, 473, "gold3"},
	{0x000000ee, 474, "blue2"},			{0x008b7500, 474, "gold4"},
	{0x000000cd, 475, "blue3"},			{0x0000008b, 476, "blue4"},
	{0x0000ffff, 476, "cyan1"},			{0x0000eeee, 477, "cyan2"},
	{0x0000cdcd, 478, "cyan3"},			{0x00008b8b, 479, "cyan4"},
	{0x00ffb5c5, 483, "pink1"},			{0x00000000, 483, "gray0"},
	{0x00eea9b8, 484, "pink2"},			{0x00030303, 484, "gray1"},
	{0x00cd919e, 485, "pink3"},			{0x00050505, 485, "gray2"},
	{0x008b636c, 486, "pink4"},			{0x00080808, 486, "gray3"},
	{0x00000000, 487, "grey0"},			{0x000a0a0a, 487, "gray4"},
	{0x00030303, 488, "grey1"},			{0x000d0d0d, 488, "gray5"},
	{0x00050505, 489, "grey2"},			{0x000f0f0f, 489, "gray6"},
	{0x00080808, 490, "grey3"},			{0x00121212, 490, "gray7"},
	{0x000a0a0a, 491, "grey4"},			{0x00141414, 491, "gray8"},
	{0x000d0d0d, 492, "grey5"},			{0x00171717, 492, "gray9"},
	{0x000f0f0f, 493, "grey6"},			{0x00121212, 494, "grey7"},
	{0x00141414, 495, "grey8"},			{0x00ffbbff, 495, "plum1"},
	{0x00eeaeee, 496, "plum2"},			{0x00171717, 496, "grey9"},
	{0x00cd96cd, 497, "plum3"},			{0x008b668b, 498, "plum4"},
	{0x00fffafa, 504, "snow1"},			{0x00eee9e9, 505, "snow2"},
	{0x00cdc9c9, 506, "snow3"},			{0x008b8989, 507, "snow4"},
	{0x00f5f5dc, 508, "beige"},			{0x00000000, 509, "black"},
	{0x00f0e68c, 520, "khaki"},			{0x00ff7f50, 529, "coral"},
	{0x0000ff00, 529, "green"},			{0x001a1a1a, 532, "gray10"},
	{0x001c1c1c, 533, "gray11"},			{0x00333333, 533, "gray20"},
	{0x001f1f1f, 534, "gray12"},			{0x00faf0e6, 534, "linen"},
	{0x00363636, 534, "gray21"},			{0x004d4d4d, 534, "gray30"},
	{0x00383838, 535, "gray22"},			{0x00212121, 535, "gray13"},
	{0x004f4f4f, 535, "gray31"},			{0x00666666, 535, "gray40"},
	{0x00242424, 536, "gray14"},			{0x001a1a1a, 536, "grey10"},
	{0x00525252, 536, "gray32"},			{0x003b3b3b, 536, "gray23"},
	{0x00696969, 536, "gray41"},			{0x007f7f7f, 536, "gray50"},
	{0x00f5deb3, 537, "wheat"},				{0x00262626, 537, "gray15"},
	{0x00545454, 537, "gray33"},			{0x001c1c1c, 537, "grey11"},
	{0x003d3d3d, 537, "gray24"},			{0x006b6b6b, 537, "gray42"},
	{0x00333333, 537, "grey20"},			{0x00828282, 537, "gray51"},
	{0x00999999, 537, "gray60"},			{0x00292929, 538, "gray16"},
	{0x004d4d4d, 538, "grey30"},			{0x001f1f1f, 538, "grey12"},
	{0x006e6e6e, 538, "gray43"},			{0x00363636, 538, "grey21"},
	{0x00404040, 538, "gray25"},			{0x00858585, 538, "gray52"},
	{0x00575757, 538, "gray34"},			{0x009c9c9c, 538, "gray61"},
	{0x00b3b3b3, 538, "gray70"},			{0x002b2b2b, 539, "gray17"},
	{0x00707070, 539, "gray44"},			{0x00212121, 539, "grey13"},
	{0x00595959, 539, "gray35"},			{0x00383838, 539, "grey22"},
	{0x00878787, 539, "gray53"},			{0x00666666, 539, "grey40"},
	{0x004f4f4f, 539, "grey31"},			{0x009e9e9e, 539, "gray62"},
	{0x00424242, 539, "gray26"},			{0x00b5b5b5, 539, "gray71"},
	{0x00cccccc, 539, "gray80"},			{0x00525252, 540, "grey32"},
	{0x00242424, 540, "grey14"},			{0x00696969, 540, "grey41"},
	{0x008a8a8a, 540, "gray54"},			{0x003b3b3b, 540, "grey23"},
	{0x00454545, 540, "gray27"},			{0x005c5c5c, 540, "gray36"},
	{0x00a1a1a1, 540, "gray63"},			{0x00737373, 540, "gray45"},
	{0x002e2e2e, 540, "gray18"},			{0x00b8b8b8, 540, "gray72"},
	{0x007f7f7f, 540, "grey50"},			{0x00cfcfcf, 540, "gray81"},
	{0x00e5e5e5, 540, "gray90"},			{0x00545454, 541, "grey33"},
	{0x00999999, 541, "grey60"},			{0x00757575, 541, "gray46"},
	{0x00474747, 541, "gray28"},			{0x00303030, 541, "gray19"},
	{0x00a3a3a3, 541, "gray64"},			{0x00262626, 541, "grey15"},
	{0x00828282, 541, "grey51"},			{0x006b6b6b, 541, "grey42"},
	{0x00bababa, 541, "gray73"},			{0x005e5e5e, 541, "gray37"},
	{0x003d3d3d, 541, "grey24"},			{0x00d1d1d1, 541, "gray82"},
	{0x008c8c8c, 541, "gray55"},			{0x00e8e8e8, 541, "gray91"},
	{0x00616161, 542, "gray38"},			{0x00a6a6a6, 542, "gray65"},
	{0x00787878, 542, "gray47"},			{0x00b3b3b3, 542, "grey70"},
	{0x00575757, 542, "grey34"},			{0x008f8f8f, 542, "gray56"},
	{0x00404040, 542, "grey25"},			{0x00bdbdbd, 542, "gray74"},
	{0x006e6e6e, 542, "grey43"},			{0x00292929, 542, "grey16"},
	{0x009c9c9c, 542, "grey61"},			{0x00d4d4d4, 542, "gray83"},
	{0x004a4a4a, 542, "gray29"},			{0x00858585, 542, "grey52"},
	{0x00ebebeb, 542, "gray92"},			{0x00b5b5b5, 543, "grey71"},
	{0x00878787, 543, "grey53"},			{0x00707070, 543, "grey44"},
	{0x009e9e9e, 543, "grey62"},			{0x00bfbfbf, 543, "gray75"},
	{0x00424242, 543, "grey26"},			{0x00cccccc, 543, "grey80"},
	{0x00595959, 543, "grey35"},			{0x00919191, 543, "gray57"},
	{0x00a8a8a8, 543, "gray66"},			{0x00d6d6d6, 543, "gray84"},
	{0x00636363, 543, "gray39"},			{0x007a7a7a, 543, "gray48"},
	{0x002b2b2b, 543, "grey17"},			{0x00ededed, 543, "gray93"},
	{0x00c2c2c2, 544, "gray76"},			{0x00ababab, 544, "gray67"},
	{0x007d7d7d, 544, "gray49"},			{0x00737373, 544, "grey45"},
	{0x00cfcfcf, 544, "grey81"},			{0x005c5c5c, 544, "grey36"},
	{0x00949494, 544, "gray58"},			{0x00a1a1a1, 544, "grey63"},
	{0x00d9d9d9, 544, "gray85"},			{0x00b8b8b8, 544, "grey72"},
	{0x00e5e5e5, 544, "grey90"},			{0x00454545, 544, "grey27"},
	{0x002e2e2e, 544, "grey18"},			{0x008a8a8a, 544, "grey54"},
	{0x00f0f0f0, 544, "gray94"},			{0x00a3a3a3, 545, "grey64"},
	{0x008c8c8c, 545, "grey55"},			{0x00d1d1d1, 545, "grey82"},
	{0x00757575, 545, "grey46"},			{0x00bababa, 545, "grey73"},
	{0x005e5e5e, 545, "grey37"},			{0x00dbdbdb, 545, "gray86"},
	{0x00adadad, 545, "gray68"},			{0x00ffffff, 545, "white"},
	{0x00c4c4c4, 545, "gray77"},			{0x00e8e8e8, 545, "grey91"},
	{0x00969696, 545, "gray59"},			{0x00474747, 545, "grey28"},
	{0x00303030, 545, "grey19"},			{0x00f2f2f2, 545, "gray95"},
	{0x00a6a6a6, 546, "grey65"},			{0x00dedede, 546, "gray87"},
	{0x00787878, 546, "grey47"},			{0x00bdbdbd, 546, "grey74"},
	{0x008f8f8f, 546, "grey56"},			{0x004a4a4a, 546, "grey29"},
	{0x00b0b0b0, 546, "gray69"},			{0x00ebebeb, 546, "grey92"},
	{0x00d4d4d4, 546, "grey83"},			{0x00c7c7c7, 546, "gray78"},
	{0x00616161, 546, "grey38"},			{0x00f5f5f5, 546, "gray96"},
	{0x00919191, 547, "grey57"},			{0x00d6d6d6, 547, "grey84"},
	{0x00a8a8a8, 547, "grey66"},			{0x00636363, 547, "grey39"},
	{0x00c9c9c9, 547, "gray79"},			{0x00ededed, 547, "grey93"},
	{0x00e0e0e0, 547, "gray88"},			{0x007a7a7a, 547, "grey48"},
	{0x00bfbfbf, 547, "grey75"},			{0x00f7f7f7, 547, "gray97"},
	{0x00949494, 548, "grey58"},			{0x00d9d9d9, 548, "grey85"},
	{0x00ababab, 548, "grey67"},			{0x00f0f0f0, 548, "grey94"},
	{0x00c2c2c2, 548, "grey76"},			{0x007d7d7d, 548, "grey49"},
	{0x00e3e3e3, 548, "gray89"},			{0x00fafafa, 548, "gray98"},
	{0x00adadad, 549, "grey68"},			{0x00f2f2f2, 549, "grey95"},
	{0x00c4c4c4, 549, "grey77"},			{0x00dbdbdb, 549, "grey86"},
	{0x00969696, 549, "grey59"},			{0x00fcfcfc, 549, "gray99"},
	{0x00f5f5f5, 550, "grey96"},			{0x00b0b0b0, 550, "grey69"},
	{0x00c7c7c7, 550, "grey78"},			{0x00dedede, 550, "grey87"},
	{0x00c9c9c9, 551, "grey79"},			{0x00f7f7f7, 551, "grey97"},
	{0x00f0ffff, 551, "azure"},				{0x00e0e0e0, 551, "grey88"},
	{0x00a52a2a, 552, "brown"},				{0x00fafafa, 552, "grey98"},
	{0x00e3e3e3, 552, "grey89"},			{0x00fcfcfc, 553, "grey99"},
	{0x00fffff0, 569, "ivory"},				{0x00fff68f, 569, "khaki1"},
	{0x00eee685, 570, "khaki2"},			{0x00cdc673, 571, "khaki3"},
	{0x008b864e, 572, "khaki4"},			{0x0000ff00, 578, "green1"},
	{0x00ff7256, 578, "coral1"},			{0x0000ee00, 579, "green2"},
	{0x00ee6a50, 579, "coral2"},			{0x0000cd00, 580, "green3"},
	{0x00cd5b45, 580, "coral3"},			{0x00ffffff, 580, "gray100"},
	{0x008b3e2f, 581, "coral4"},			{0x00008b00, 581, "green4"},
	{0x00ffffff, 584, "grey100"},			{0x00ffe7ba, 586, "wheat1"},
	{0x00eed8ae, 587, "wheat2"},			{0x00cdba96, 588, "wheat3"},
	{0x008b7e66, 589, "wheat4"},			{0x00f0ffff, 600, "azure1"},
	{0x00e0eeee, 601, "azure2"},			{0x00ff4040, 601, "brown1"},
	{0x00c1cdcd, 602, "azure3"},			{0x00ee3b3b, 602, "brown2"},
	{0x00838b8b, 603, "azure4"},			{0x00cd3333, 603, "brown3"},
	{0x008b2323, 604, "brown4"},			{0x00fffff0, 618, "ivory1"},
	{0x00eeeee0, 619, "ivory2"},			{0x00cdcdc1, 620, "ivory3"},
	{0x008b8b83, 621, "ivory4"},			{0x00da70d6, 633, "orchid"},
	{0x00ffa500, 636, "orange"},			{0x00a0522d, 638, "sienna"},
	{0x00ffe4c4, 649, "bisque"},			{0x00fa8072, 650, "salmon"},
	{0x00b03060, 652, "maroon"},			{0x00ee82ee, 659, "violet"},
	{0x00ff6347, 660, "tomato"},			{0x00a020f0, 664, "purple"},
	{0x00ffff00, 668, "yellow"},			{0x00ff83fa, 682, "orchid1"},
	{0x00ee7ae9, 683, "orchid2"},			{0x00cd69c9, 684, "orchid3"},
	{0x00ffa500, 685, "orange1"},			{0x008b4789, 685, "orchid4"},
	{0x00ee9a00, 686, "orange2"},			{0x00ff8247, 687, "sienna1"},
	{0x00cd8500, 687, "orange3"},			{0x00ee7942, 688, "sienna2"},
	{0x008b5a00, 688, "orange4"},			{0x00cd6839, 689, "sienna3"},
	{0x008b4726, 690, "sienna4"},			{0x00ffe4c4, 698, "bisque1"},
	{0x00eed5b7, 699, "bisque2"},			{0x00ff8c69, 699, "salmon1"},
	{0x00cdb79e, 700, "bisque3"},			{0x00ee8262, 700, "salmon2"},
	{0x008b7d6b, 701, "bisque4"},			{0x00cd7054, 701, "salmon3"},
	{0x00ff34b3, 701, "maroon1"},			{0x008b4c39, 702, "salmon4"},
	{0x00ee30a7, 702, "maroon2"},			{0x00cd2990, 703, "maroon3"},
	{0x008b1c62, 704, "maroon4"},			{0x00ff6347, 709, "tomato1"},
	{0x00ee5c42, 710, "tomato2"},			{0x00cd4f39, 711, "tomato3"},
	{0x008b3626, 712, "tomato4"},			{0x009b30ff, 713, "purple1"},
	{0x00912cee, 714, "purple2"},			{0x007d26cd, 715, "purple3"},
	{0x00551a8b, 716, "purple4"},			{0x00ffff00, 717, "yellow1"},
	{0x00eeee00, 718, "yellow2"},			{0x00cdcd00, 719, "yellow3"},
	{0x008b8b00, 720, "yellow4"},			{0x00fdf5e6, 724, "oldlace"},
	{0x00ff00ff, 733, "magenta"},			{0x008b0000, 733, "darkred"},
	{0x00696969, 749, "dimgray"},			{0x00696969, 753, "dimgrey"},
	{0x00fdf5e6, 756, "old lace"},			{0x00d8bfd8, 765, "thistle"},
	{0x008b0000, 765, "dark red"},			{0x00ff69b4, 765, "hotpink"},
	{0x0087ceeb, 767, "skyblue"},			{0x00696969, 781, "dim gray"},
	{0x00ff00ff, 782, "magenta1"},			{0x00ee00ee, 783, "magenta2"},
	{0x00cd00cd, 784, "magenta3"},			{0x00696969, 785, "dim grey"},
	{0x008b008b, 785, "magenta4"},			{0x00ff69b4, 797, "hot pink"},
	{0x0087ceeb, 799, "sky blue"},			{0x00ff6eb4, 814, "hotpink1"},
	{0x00ffe1ff, 814, "thistle1"},			{0x00ee6aa7, 815, "hotpink2"},
	{0x00eed2ee, 815, "thistle2"},			{0x0087ceff, 816, "skyblue1"},
	{0x00cd6090, 816, "hotpink3"},			{0x00cdb5cd, 816, "thistle3"},
	{0x008b3a62, 817, "hotpink4"},			{0x007ec0ee, 817, "skyblue2"},
	{0x008b7b8b, 817, "thistle4"},			{0x006ca6cd, 818, "skyblue3"},
	{0x004a708b, 819, "skyblue4"},			{0x002e8b57, 842, "seagreen"},
	{0x0000008b, 842, "darkblue"},			{0x00ffe4b5, 845, "moccasin"},
	{0x00008b8b, 845, "darkcyan"},			{0x00ff1493, 848, "deeppink"},
	{0x00fff5ee, 849, "seashell"},			{0x00e6e6fa, 849, "lavender"},
	{0x00a9a9a9, 853, "darkgray"},			{0x00a9a9a9, 857, "darkgrey"},
	{0x00f0fff0, 867, "honeydew"},			{0x00fff8dc, 869, "cornsilk"},
	{0x00000080, 870, "navyblue"},			{0x002e8b57, 874, "sea green"},
	{0x0000008b, 874, "dark blue"},			{0x00008b8b, 877, "dark cyan"},
	{0x00ff1493, 880, "deep pink"},			{0x00a9a9a9, 885, "dark gray"},
	{0x00a9a9a9, 889, "dark grey"},			{0x0054ff9f, 891, "seagreen1"},
	{0x004eee94, 892, "seagreen2"},			{0x0043cd80, 893, "seagreen3"},
	{0x002e8b57, 894, "seagreen4"},			{0x00ff1493, 897, "deeppink1"},
	{0x00fff5ee, 898, "seashell1"},			{0x00ee1289, 898, "deeppink2"},
	{0x00eee5de, 899, "seashell2"},			{0x00cd1076, 899, "deeppink3"},
	{0x00cdc5bf, 900, "seashell3"},			{0x008b0a50, 900, "deeppink4"},
	{0x008b8682, 901, "seashell4"},			{0x00000080, 902, "navy blue"},
	{0x00f0fff0, 916, "honeydew1"},			{0x00e0eee0, 917, "honeydew2"},
	{0x00fff8dc, 918, "cornsilk1"},			{0x00c1cdc1, 918, "honeydew3"},
	{0x00eee8cd, 919, "cornsilk2"},			{0x00838b83, 919, "honeydew4"},
	{0x00cdc8b1, 920, "cornsilk3"},			{0x008b8878, 921, "cornsilk4"},
	{0x00f0f8ff, 934, "aliceblue"},			{0x005f9ea0, 937, "cadetblue"},
	{0x00bdb76b, 938, "darkkhaki"},			{0x00cd5c5c, 942, "indianred"},
	{0x00b22222, 945, "firebrick"},			{0x00d2691e, 946, "chocolate"},
	{0x00ffdab9, 946, "peachpuff"},			{0x0098fb98, 947, "palegreen"},
	{0x00006400, 947, "darkgreen"},			{0x00ff4500, 951, "orangered"},
	{0x006b8e23, 952, "olivedrab"},			{0x0032cd32, 952, "limegreen"},
	{0x00daa520, 958, "goldenrod"},			{0x00add8e6, 960, "lightblue"},
	{0x00f5fffa, 960, "mintcream"},			{0x006a5acd, 961, "slateblue"},
	{0x007cfc00, 963, "lawngreen"},			{0x00e0ffff, 963, "lightcyan"},
	{0x00dcdcdc, 964, "gainsboro"},			{0x004682b4, 965, "steelblue"},
	{0x00f0f8ff, 966, "alice blue"},		{0x005f9ea0, 969, "cadet blue"},
	{0x00ffb6c1, 970, "lightpink"},			{0x00bdb76b, 970, "dark khaki"},
	{0x00d3d3d3, 971, "lightgray"},			{0x00708090, 972, "slategray"},
	{0x00cd5c5c, 974, "indian red"},		{0x00d02090, 974, "violetred"},
	{0x004169e1, 975, "royalblue"},			{0x00d3d3d3, 975, "lightgrey"},
	{0x00708090, 976, "slategrey"},			{0x00ffdab9, 978, "peach puff"},
	{0x0098fb98, 979, "pale green"},			{0x00006400, 979, "dark green"},
	{0x00ff4500, 983, "orange red"},			{0x0032cd32, 984, "lime green"},
	{0x006b8e23, 984, "olive drab"},			{0x0098f5ff, 986, "cadetblue1"},
	{0x008ee5ee, 987, "cadetblue2"},			{0x007ac5cd, 988, "cadetblue3"},
	{0x0053868b, 989, "cadetblue4"},			{0x00ff6a6a, 991, "indianred1"},
	{0x00ee6363, 992, "indianred2"},			{0x00add8e6, 992, "light blue"},
	{0x00f5fffa, 992, "mint cream"},			{0x006a5acd, 993, "slate blue"},
	{0x00cd5555, 993, "indianred3"},			{0x008b3a3a, 994, "indianred4"},
	{0x00ff3030, 994, "firebrick1"},			{0x00ff7f24, 995, "chocolate1"},
	{0x00ffdab9, 995, "peachpuff1"},			{0x00ee2c2c, 995, "firebrick2"},
	{0x007cfc00, 995, "lawn green"},			{0x00e0ffff, 995, "light cyan"},
	{0x00ee7621, 996, "chocolate2"},			{0x009aff9a, 996, "palegreen1"},
	{0x00eecbad, 996, "peachpuff2"},			{0x00cd2626, 996, "firebrick3"},
	{0x0090ee90, 997, "palegreen2"},			{0x00cdaf95, 997, "peachpuff3"},
	{0x008b1a1a, 997, "firebrick4"},			{0x00cd661d, 997, "chocolate3"},
	{0x004682b4, 997, "steel blue"},			{0x007ccd7c, 998, "palegreen3"},
	{0x008b7765, 998, "peachpuff4"},			{0x008b4513, 998, "chocolate4"},
	{0x00deb887, 999, "burlywood"},				{0x00548b54, 999, "palegreen4"},
	{0x00ff4500, 1000, "orangered1"},			{0x00c0ff3e, 1001, "olivedrab1"},
	{0x00ee4000, 1001, "orangered2"},			{0x00ffb6c1, 1002, "light pink"},
	{0x00b3ee3a, 1002, "olivedrab2"},			{0x00cd3700, 1002, "orangered3"},
	{0x009acd32, 1003, "olivedrab3"},			{0x00d3d3d3, 1003, "light gray"},
	{0x008b2500, 1003, "orangered4"},			{0x00708090, 1004, "slate gray"},
	{0x00698b22, 1004, "olivedrab4"},			{0x00d02090, 1006, "violet red"},
	{0x00d3d3d3, 1007, "light grey"},			{0x004169e1, 1007, "royal blue"},
	{0x00ffe4e1, 1007, "mistyrose"},			{0x00ffc125, 1007, "goldenrod1"},
	{0x00708090, 1008, "slate grey"},			{0x00eeb422, 1008, "goldenrod2"},
	{0x0040e0d0, 1009, "turquoise"},			{0x00bfefff, 1009, "lightblue1"},
	{0x00cd9b1d, 1009, "goldenrod3"},			{0x00b2dfee, 1010, "lightblue2"},
	{0x00836fff, 1010, "slateblue1"},			{0x008b6914, 1010, "goldenrod4"},
	{0x007a67ee, 1011, "slateblue2"},			{0x009ac0cd, 1011, "lightblue3"},
	{0x0068838b, 1012, "lightblue4"},			{0x00e0ffff, 1012, "lightcyan1"},
	{0x006959cd, 1012, "slateblue3"},			{0x00bc8f8f, 1013, "rosybrown"},
	{0x00d1eeee, 1013, "lightcyan2"},			{0x00473c8b, 1013, "slateblue4"},
	{0x0063b8ff, 1014, "steelblue1"},			{0x00b4cdcd, 1014, "lightcyan3"},
	{0x005cacee, 1015, "steelblue2"},			{0x007a8b8b, 1015, "lightcyan4"},
	{0x004f94cd, 1016, "steelblue3"},			{0x0036648b, 1017, "steelblue4"},
	{0x00ffaeb9, 1019, "lightpink1"},			{0x00eea2ad, 1020, "lightpink2"},
	{0x00c6e2ff, 1021, "slategray1"},			{0x00cd8c95, 1021, "lightpink3"},
	{0x00b9d3ee, 1022, "slategray2"},			{0x008b5f65, 1022, "lightpink4"},
	{0x009fb6cd, 1023, "slategray3"},			{0x00ff3e96, 1023, "violetred1"},
	{0x004876ff, 1024, "royalblue1"},			{0x00ee3a8c, 1024, "violetred2"},
	{0x006c7b8b, 1024, "slategray4"},			{0x00436eee, 1025, "royalblue2"},
	{0x00cd3278, 1025, "violetred3"},			{0x003a5fcd, 1026, "royalblue3"},
	{0x008b2252, 1026, "violetred4"},			{0x0027408b, 1027, "royalblue4"},
	{0x00ffe4e1, 1039, "misty rose"},			{0x00bc8f8f, 1045, "rosy brown"},
	{0x00ffd39b, 1048, "burlywood1"},			{0x00eec591, 1049, "burlywood2"},
	{0x00cdaa7d, 1050, "burlywood3"},			{0x009932cc, 1051, "darkorchid"},
	{0x008b7355, 1051, "burlywood4"},			{0x001e90ff, 1053, "dodgerblue"},
	{0x00ff8c00, 1054, "darkorange"},			{0x00ffe4e1, 1056, "mistyrose1"},
	{0x00eed5d2, 1057, "mistyrose2"},			{0x00cdb7b5, 1058, "mistyrose3"},
	{0x0000f5ff, 1058, "turquoise1"},			{0x008b7d7b, 1059, "mistyrose4"},
	{0x0000e5ee, 1059, "turquoise2"},			{0x007fffd4, 1060, "aquamarine"},
	{0x0000c5cd, 1060, "turquoise3"},			{0x0000868b, 1061, "turquoise4"},
	{0x00ffc1c1, 1062, "rosybrown1"},			{0x00eeb4b4, 1063, "rosybrown2"},
	{0x00cd9b9b, 1064, "rosybrown3"},			{0x000000cd, 1065, "mediumblue"},
	{0x00f08080, 1065, "lightcoral"},			{0x008b6969, 1065, "rosybrown4"},
	{0x0090ee90, 1065, "lightgreen"},			{0x00e9967a, 1068, "darksalmon"},
	{0x00ffefd5, 1076, "papayawhip"},			{0x009400d3, 1077, "darkviolet"},
	{0x007fff00, 1078, "chartreuse"},			{0x00b0e0e6, 1081, "powderblue"},
	{0x008a2be2, 1083, "blueviolet"},			{0x009932cc, 1083, "dark orchid"},
	{0x001e90ff, 1085, "dodger blue"},			{0x00ff8c00, 1086, "dark orange"},
	{0x00f5f5f5, 1088, "whitesmoke"},			{0x00f8f8ff, 1094, "ghostwhite"},
	{0x00f4a460, 1095, "sandybrown"},			{0x000000cd, 1097, "medium blue"},
	{0x0090ee90, 1097, "light green"},			{0x00f08080, 1097, "light coral"},
	{0x00e9967a, 1100, "dark salmon"},			{0x00bf3eff, 1100, "darkorchid1"},
	{0x00b23aee, 1101, "darkorchid2"},			{0x001e90ff, 1102, "dodgerblue1"},
	{0x009a32cd, 1102, "darkorchid3"},			{0x00ff7f00, 1103, "darkorange1"},
	{0x001c86ee, 1103, "dodgerblue2"},			{0x0068228b, 1103, "darkorchid4"},
	{0x001874cd, 1104, "dodgerblue3"},			{0x00ee7600, 1104, "darkorange2"},
	{0x00104e8b, 1105, "dodgerblue4"},			{0x00cd6600, 1105, "darkorange3"},
	{0x008b4500, 1106, "darkorange4"},			{0x00ffefd5, 1108, "papaya whip"},
	{0x009400d3, 1109, "dark violet"},			{0x007fffd4, 1109, "aquamarine1"},
	{0x0076eec6, 1110, "aquamarine2"},			{0x0066cdaa, 1111, "aquamarine3"},
	{0x00458b74, 1112, "aquamarine4"},			{0x00b0e0e6, 1113, "powder blue"},
	{0x008a2be2, 1115, "blue violet"},			{0x00f5f5f5, 1120, "white smoke"},
	{0x00f8f8ff, 1126, "ghost white"},			{0x00f4a460, 1127, "sandy brown"},
	{0x007fff00, 1127, "chartreuse1"},			{0x0076ee00, 1128, "chartreuse2"},
	{0x0066cd00, 1129, "chartreuse3"},			{0x00458b00, 1130, "chartreuse4"},
	{0x008b008b, 1151, "darkmagenta"},			{0x008b4513, 1173, "saddlebrown"},
	{0x0000bfff, 1181, "deepskyblue"},			{0x008b008b, 1183, "dark magenta"},
	{0x00ffdead, 1184, "navajowhite"},			{0x00fffaf0, 1185, "floralwhite"},
	{0x00ffa07a, 1186, "lightsalmon"},			{0x0000ff7f, 1188, "springgreen"},
	{0x00228b22, 1188, "forestgreen"},			{0x009acd32, 1197, "yellowgreen"},
	{0x00adff2f, 1197, "greenyellow"},			{0x00ffffe0, 1204, "lightyellow"},
	{0x008b4513, 1205, "saddle brown"},			{0x00ffdead, 1216, "navajo white"},
	{0x00fffaf0, 1217, "floral white"},			{0x00ffa07a, 1218, "light salmon"},
	{0x0000ff7f, 1220, "spring green"},			{0x00228b22, 1220, "forest green"},
	{0x00adff2f, 1229, "green yellow"},			{0x009acd32, 1229, "yellow green"},
	{0x0000bfff, 1230, "deepskyblue1"},			{0x0000b2ee, 1231, "deepskyblue2"},
	{0x00009acd, 1232, "deepskyblue3"},			{0x00ffdead, 1233, "navajowhite1"},
	{0x0000688b, 1233, "deepskyblue4"},			{0x00eecfa1, 1234, "navajowhite2"},
	{0x00cdb38b, 1235, "navajowhite3"},			{0x00ffa07a, 1235, "lightsalmon1"},
	{0x008b795e, 1236, "navajowhite4"},			{0x00ffffe0, 1236, "light yellow"},
	{0x00ee9572, 1236, "lightsalmon2"},			{0x0000ff7f, 1237, "springgreen1"},
	{0x00cd8162, 1237, "lightsalmon3"},			{0x0000ee76, 1238, "springgreen2"},
	{0x008b5742, 1238, "lightsalmon4"},			{0x0000cd66, 1239, "springgreen3"},
	{0x00008b45, 1240, "springgreen4"},			{0x0000bfff, 1245, "deep sky blue"},
	{0x00ffffe0, 1253, "lightyellow1"},			{0x00eeeed1, 1254, "lightyellow2"},
	{0x00cdcdb4, 1255, "lightyellow3"},			{0x008b8b7a, 1256, "lightyellow4"},
	{0x008fbc8f, 1260, "darkseagreen"},			{0x00fffacd, 1272, "lemonchiffon"},
	{0x00ba55d3, 1274, "mediumorchid"},			{0x00191970, 1276, "midnightblue"},
	{0x0087cefa, 1303, "lightskyblue"},			{0x00faebd7, 1304, "antiquewhite"},
	{0x00fffacd, 1304, "lemon chiffon"},			{0x009370db, 1305, "mediumpurple"},
	{0x00ba55d3, 1306, "medium orchid"},			{0x00191970, 1308, "midnight blue"},
	{0x00c1ffc1, 1309, "darkseagreen1"},			{0x00b4eeb4, 1310, "darkseagreen2"},
	{0x009bcd9b, 1311, "darkseagreen3"},			{0x00698b69, 1312, "darkseagreen4"},
	{0x00fffacd, 1321, "lemonchiffon1"},			{0x00eee9bf, 1322, "lemonchiffon2"},
	{0x00cdc9a5, 1323, "lemonchiffon3"},			{0x00e066ff, 1323, "mediumorchid1"},
	{0x008b8970, 1324, "lemonchiffon4"},			{0x008fbc8f, 1324, "dark sea green"},
	{0x00d15fee, 1324, "mediumorchid2"},			{0x00b452cd, 1325, "mediumorchid3"},
	{0x007a378b, 1326, "mediumorchid4"},			{0x00faebd7, 1336, "antique white"},
	{0x009370db, 1337, "medium purple"},			{0x00b0e2ff, 1352, "lightskyblue1"},
	{0x00ffefdb, 1353, "antiquewhite1"},			{0x00a4d3ee, 1353, "lightskyblue2"},
	{0x008db6cd, 1354, "lightskyblue3"},			{0x00ab82ff, 1354, "mediumpurple1"},
	{0x00eedfcc, 1354, "antiquewhite2"},			{0x00cdc0b0, 1355, "antiquewhite3"},
	{0x009f79ee, 1355, "mediumpurple2"},			{0x00607b8b, 1355, "lightskyblue4"},
	{0x008b8378, 1356, "antiquewhite4"},			{0x008968cd, 1356, "mediumpurple3"},
	{0x005d478b, 1357, "mediumpurple4"},			{0x0087cefa, 1367, "light sky blue"},
	{0x00eee8aa, 1376, "palegoldenrod"},			{0x00b8860b, 1376, "darkgoldenrod"},
	{0x0020b2aa, 1378, "lightseagreen"},			{0x00483d8b, 1379, "darkslateblue"},
	{0x002f4f4f, 1390, "darkslategray"},			{0x00fff0f5, 1391, "lavenderblush"},
	{0x00db7093, 1392, "palevioletred"},			{0x002f4f4f, 1394, "darkslategrey"},
	{0x00eee8aa, 1408, "pale goldenrod"},			{0x00b8860b, 1408, "dark goldenrod"},
	{0x00fff0f5, 1423, "lavender blush"},			{0x00ffb90f, 1425, "darkgoldenrod1"},
	{0x00eead0e, 1426, "darkgoldenrod2"},			{0x00afeeee, 1427, "paleturquoise"},
	{0x00cd950c, 1427, "darkgoldenrod3"},			{0x0000ced1, 1427, "darkturquoise"},
	{0x008b6508, 1428, "darkgoldenrod4"},			{0x0097ffff, 1439, "darkslategray1"},
	{0x00fff0f5, 1440, "lavenderblush1"},			{0x008deeee, 1440, "darkslategray2"},
	{0x0079cdcd, 1441, "darkslategray3"},			{0x00ff82ab, 1441, "palevioletred1"},
	{0x00eee0e5, 1441, "lavenderblush2"},			{0x00cdc1c5, 1442, "lavenderblush3"},
	{0x0020b2aa, 1442, "light sea green"},			{0x00528b8b, 1442, "darkslategray4"},
	{0x00ee799f, 1442, "palevioletred2"},			{0x008b8386, 1443, "lavenderblush4"},
	{0x00cd6889, 1443, "palevioletred3"},			{0x00483d8b, 1443, "dark slate blue"},
	{0x008b475d, 1444, "palevioletred4"},			{0x00ffebcd, 1452, "blanchedalmond"},
	{0x002f4f4f, 1454, "dark slate gray"},			{0x00db7093, 1456, "pale violet red"},
	{0x002f4f4f, 1458, "dark slate grey"},			{0x00afeeee, 1459, "pale turquoise"},
	{0x0000ced1, 1459, "dark turquoise"},			{0x00bbffff, 1476, "paleturquoise1"},
	{0x00aeeeee, 1477, "paleturquoise2"},			{0x0096cdcd, 1478, "paleturquoise3"},
	{0x00668b8b, 1479, "paleturquoise4"},			{0x003cb371, 1483, "mediumseagreen"},
	{0x00ffebcd, 1484, "blanched almond"},			{0x00556b2f, 1490, "darkolivegreen"},
	{0x00eedd82, 1494, "lightgoldenrod"},			{0x008470ff, 1497, "lightslateblue"},
	{0x00b0c4de, 1501, "lightsteelblue"},			{0x00778899, 1508, "lightslategray"},
	{0x00778899, 1512, "lightslategrey"},			{0x006495ed, 1513, "cornflowerblue"},
	{0x00eedd82, 1526, "light goldenrod"},			{0x00caff70, 1539, "darkolivegreen1"},
	{0x00bcee68, 1540, "darkolivegreen2"},			{0x00a2cd5a, 1541, "darkolivegreen3"},
	{0x006e8b3d, 1542, "darkolivegreen4"},			{0x00ffec8b, 1543, "lightgoldenrod1"},
	{0x00eedc82, 1544, "lightgoldenrod2"},			{0x006495ed, 1545, "cornflower blue"},
	{0x00cdbe70, 1545, "lightgoldenrod3"},			{0x008b814c, 1546, "lightgoldenrod4"},
	{0x003cb371, 1547, "medium sea green"},			{0x00cae1ff, 1550, "lightsteelblue1"},
	{0x00bcd2ee, 1551, "lightsteelblue2"},			{0x00a2b5cd, 1552, "lightsteelblue3"},
	{0x006e7b8b, 1553, "lightsteelblue4"},			{0x00556b2f, 1554, "dark olive green"},
	{0x008470ff, 1561, "light slate blue"},			{0x00b0c4de, 1565, "light steel blue"},
	{0x00778899, 1572, "light slate gray"},			{0x00778899, 1576, "light slate grey"},
	{0x007b68ee, 1602, "mediumslateblue"},			{0x00c71585, 1615, "mediumvioletred"},
	{0x0048d1cc, 1650, "mediumturquoise"},			{0x007b68ee, 1666, "medium slate blue"},
	{0x00c71585, 1679, "medium violet red"},		{0x0048d1cc, 1682, "medium turquoise"},
	{0x0066cdaa, 1701, "mediumaquamarine"},			{0x0066cdaa, 1733, "medium aquamarine"},
	{0x0000fa9a, 1829, "mediumspringgreen"},		{0x0000fa9a, 1893, "medium spring green"},
	{0x00fafad2, 2162, "lightgoldenrodyellow"},		{0x00fafad2, 2226, "light goldenrod yellow"}
};

#define XPM_RGBTAB_STEP	10

/* 在颜色表中查找指定颜色的RGB值 */
int CALLAGREEMENT _search_color(char *name, unsigned long *pcol)
{
	int i, j;
	int succ = 0, value = 0;
	char *p = name;

	if (_stricmp(name, "None") == 0)	/* _stricmp()为非ANSI函数，移植时需注意 */
	{
		*pcol = 0xff000000;
		return 1;						/* 透明色 */
	}

	/* 计算名称串字符累加值 - ipv */
	while (*p)
		value += (int)(unsigned char)(*p++);

	/* 步进查找－粗略的预定位 */
	for (i=0;i<RGBTABSIZE;i+=XPM_RGBTAB_STEP)
		if (value <= rgbtab[i].iqv)
			break;

	if (i == 0)
		i = 0;
	else if (i >= RGBTABSIZE)
		i = (RGBTABSIZE/XPM_RGBTAB_STEP)*XPM_RGBTAB_STEP;
	else
		i -= XPM_RGBTAB_STEP;

	/* 搜索匹配颜色 */
	for (j=i;j<RGBTABSIZE;j++)
	{
		if (_stricmp(name, rgbtab[j].name) == 0)
		{
			succ = 1; break;
		}
	}

	if (succ == 1)
	{
		*pcol = rgbtab[j].rgba;
		return 0;						/* 找到指定颜色 */
	}
	else
	{
		*pcol = 0UL;
		return -1;						/* 没有找到 */
	}
}


/* 将给定的符号化颜色数据转换为二进制RGB数据 */
int CALLAGREEMENT _color_to_rgb(char *name, int len, unsigned long *rgb)
{
	char buf[256];

	assert(name);

	if (name[0] == '#')				/* 直接RGB给定方式 */
	{
		switch(len) {
		case 4:
			buf[0] = buf[1] = name[1];
			buf[2] = buf[3] = name[2];
			buf[4] = buf[5] = name[3];
			break;
		case 7:
			memcpy(buf, name + 1, 6);
			break;
		case 13:
			buf[0] = name[1];
			buf[1] = name[2];
			buf[2] = name[5];
			buf[3] = name[6];
			buf[4] = name[9];
			buf[5] = name[10];
			break;
		}
		buf[6] = '\0';
		*rgb = strtol(buf, 0, 16);	/* 转为二进制RGB值 */
		return 0;
	}
	else							/* 颜色名给定方式 */	
	{
		memcpy((void*)buf, (const void *)name, len);
		buf[len] = 0;
		return _search_color((char *)buf, rgb);
	}

	/* 返回值： 1－透明色
				0－普通色
				-1－失败，未能找到指定的颜色
	*/
}


/* 根据给定的象素描述符查找对应的RGB数据数组的索引值 */
int CALLAGREEMENT _search_pix(LPXPMCOLOR pcol, int ncol, char *pix, int cpp)
{
	int i;

	for (i=0;i<ncol;i++)
		if (memcmp((const void *)pcol[i].pix, (const void *)pix, cpp) == 0)
			return i;

	return -1;
}


