#define RS_STRETCH	21000		//拉伸图像
#define RS_CENTER		21001    //居中
#define RS_TILE		21002    //平铺
#define RS_KEEP		21003    //保持图像

//以下数值请与resource.h中IDC_RS_KEEP_UL至IDC_RS_KEEP_DL的值保持一致
#define RS_UL			4327    //左上角
#define RS_UM			4328    //上中
#define RS_UR			4329    //右上角
#define RS_ML			4332    //左中
#define RS_MR			4330    //右中
#define RS_DL			4335    //左下角
#define RS_DM			4333    //下中
#define RS_DR			4334    //右下角
#define RS_MM			4331    //居中

int Resize(int w,int h,int rs_flag,int rs_pos,COLORREF rs_bgcolor);
int Output_ResizeTile(int width,int height,LPBYTE lpDestData);
int Output_ResizePos(int w,int h,LPBYTE lpDestData,int rs_pos,COLORREF* rs_bgcolor);

