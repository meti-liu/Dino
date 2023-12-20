#include<graphics.h>
#include<iostream>
#include<conio.h>
using namespace std;
const int width = 1792;//��ͼ����
const int height = 1024;
IMAGE img_dino1, img_dino2, img_dino3;//������ͬ��ɫ�Ŀ���
IMAGE img_background;

#pragma comment(lib,"MSIMG32.LIB")//windows�Դ���һ������
inline void putimage1(int x, int y, IMAGE* img)//ֱ��ʹ��putimage�����Ļ�
{//ͼƬ�ز���Χ���к�ɫ�����������Լ�д��һ��putimage1�������н���͸���ȴ���
	int w = img->getwidth();
	int h = img->getheight();
	AlphaBlend(GetImageHDC(NULL), x, y, w, h,
		GetImageHDC(img), 0, 0, w, h, { AC_SRC_OVER,0,255,AC_SRC_ALPHA });
}
enum ObjType{Cactus,Bird,Cloud};//���Ǹ��˶������彨һ���࣬��Ծ�ֹ��dino��û��Ҫ��
class Moving
{
public:
	ObjType type;
	float x, y;
	float vx;//����˶����ϰ�����������vxӦ���Ǹ���
	Moving(float x,float y,float vx,ObjType type):x(x),y(y),vx(vx),type(type){}

};
class Dino
{
public:
	float x, y;
	float vy, g;
	Dino(float x, float y, float vy, float g) :x(x),y(y),vy(vy),g(g){}
	void draw()
	{
		putimage1(x, y, &img_dino1);
	}
	void update()//�����˶��ĸ���
	{
		vy += g;
		y += vy;
		if (y > height) vy = -vy;
	}
};
void startup()
{
	loadimage(&img_background, _T("img/background.png"));//����ͼƬ����
	loadimage(&img_dino1, _T("img/dino1.png"));//
	loadimage(&img_dino2, _T("img/dino2.png"));
	loadimage(&img_dino3, _T("img/dino3.png"));
}
int main()
{
	startup();
	initgraph(width, height);//�ܿᴰ��
	BeginBatchDraw();//˫����
	Dino dino1(200, height-400, -20, 5);
	while (true)//������Ϊ��End
	{
		cleardevice();
		putimage(0, 0, &img_background);//����ͼƬ��Ⱦ
		dino1.draw();
		dino1.update();
		FlushBatchDraw();
		Sleep(20);
	}
	EndBatchDraw();
	closegraph();
	return 0;
}