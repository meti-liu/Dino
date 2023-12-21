#include<graphics.h>
#include<iostream>
#include<conio.h>
using namespace std;
const int width = 896;//地图长宽
const int height = 512;
IMAGE img_dino1, img_dino2, img_dino3;//三个不同颜色的恐龙
IMAGE img_background;
int dino_index;//有两张恐龙图片，循环播放，这里记录下标

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
	bool Jumping;
	Dino(float x, float y, float vy, float g) :x(x),y(y),vy(vy),g(g){}
	void draw()
	{
		if(dino_index==0) putimage1(x, y, &img_dino1);
		if (dino_index == 1) putimage1(x, y, &img_dino2);

	}
	void StartJump()
	{
		if (Jumping == false)
		{
			vy = -40;
			Jumping = true;
		}
	}

	void jump()//物理运动的更新
	{
		if (Jumping)
		{
			vy += g;
			y += vy;
			if (y > height - 240)
			{
				y = height - 240;//保持在地面上
				vy = 0;//停止下落
				Jumping = false;
			}
		}

	}
};
void startup()
{
	loadimage(&img_background, _T("img/background.png"));//背景图片加载
	loadimage(&img_dino1, _T("img/DinoRun1.png"));//
	loadimage(&img_dino2, _T("img/DinoRun2.png"));
	loadimage(&img_dino3, _T("img/dino3.png"));
}
int main()
{
	ExMessage msg;//
	startup();
	initgraph(width, height);//跑酷窗口
	BeginBatchDraw();//双缓冲
	Dino dino1(200, height-240, 0, 5);
	while (true)//后续改为！End
	{
		while (peekmessage(&msg))//信息输出，上箭头（或space）和下箭头
		{
			if (msg.message == WM_KEYDOWN)
			{
				if (msg.vkcode == VK_SPACE || msg.vkcode == VK_UP)
				{
					dino1.StartJump();
				}
			}
		}
		static int counter;//使用一个计数器，用于图片循环播放
		if (++counter % 5 == 0)//每5帧，切换一个恐龙（假设一次循环是一帧）
		{
			dino_index++;
		}
		dino_index = dino_index % 2;//一共就两张图，这里取模循环


		cleardevice();
		putimage(0, 0, &img_background);//背景图片渲染
		dino1.draw();
		dino1.jump();
		FlushBatchDraw();
		Sleep(30);
	}
	EndBatchDraw();
	closegraph();
	return 0;
}