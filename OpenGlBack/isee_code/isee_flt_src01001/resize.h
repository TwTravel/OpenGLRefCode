#define RS_STRETCH	21000		//����ͼ��
#define RS_CENTER		21001    //����
#define RS_TILE		21002    //ƽ��
#define RS_KEEP		21003    //����ͼ��

//������ֵ����resource.h��IDC_RS_KEEP_UL��IDC_RS_KEEP_DL��ֵ����һ��
#define RS_UL			4327    //���Ͻ�
#define RS_UM			4328    //����
#define RS_UR			4329    //���Ͻ�
#define RS_ML			4332    //����
#define RS_MR			4330    //����
#define RS_DL			4335    //���½�
#define RS_DM			4333    //����
#define RS_DR			4334    //���½�
#define RS_MM			4331    //����

int Resize(int w,int h,int rs_flag,int rs_pos,COLORREF rs_bgcolor);
int Output_ResizeTile(int width,int height,LPBYTE lpDestData);
int Output_ResizePos(int w,int h,LPBYTE lpDestData,int rs_pos,COLORREF* rs_bgcolor);

