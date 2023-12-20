#include<graphics.h>
#include<iostream>
#include<conio.h>
using namespace std;
const int width = 1792;//地图长宽
const int height = 1024;
IMAGE img_dino1, img_dino2, img_dino3;//三个不同颜色的恐龙
IMAGE img_background;

#pragma comment(lib,"MSIMG32.LIB")//windows自带的一个处理
inline void putimage1(int x, int y, IMAGE* img)//直接使用putimage函数的话
{//图片素材周围会有黑色框，于是我们自己写了一个putimage1函数，有进行透明度处理
	int w = img->getwidth();
	int h = img->getheight();
	AlphaBlend(GetImageHDC(NULL), x, y, w, h,
		GetImageHDC(img), 0, 0, w, h, { AC_SRC_OVER,0,255,AC_SRC_ALPHA });
}
enum ObjType{Cactus,Bird,Cloud};//我们给运动的物体建一个类，相对静止的dino就没必要了
class Moving
{
public:
	ObjType type;
	float x, y;
	float vx;//相对运动，障碍物向左，这里vx应该是负的
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
	void update()//物理运动的更新
	{
		vy += g;
		y += vy;
		if (y > height) vy = -vy;
	}
};
void startup()
{
	loadimage(&img_background, _T("img/background.png"));//背景图片加载
	loadimage(&img_dino1, _T("img/dino1.png"));//
	loadimage(&img_dino2, _T("img/dino2.png"));
	loadimage(&img_dino3, _T("img/dino3.png"));
}
int main()
{
	startup();
	initgraph(width, height);//跑酷窗口
	BeginBatchDraw();//双缓冲
	Dino dino1(200, height-400, -20, 5);
	while (true)//后续改为！End
	{
		cleardevice();
		putimage(0, 0, &img_background);//背景图片渲染
		dino1.draw();
		dino1.update();
		FlushBatchDraw();
		Sleep(20);
	}
	EndBatchDraw();
	closegraph();
	return 0;
}