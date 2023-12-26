#include<graphics.h>
#include<iostream>
#include<conio.h>
#include<vector>//采用动态数组来管理持续增加的cactus，cloud或bird
#include<ctime>//用于随机数生成
#include <cmath>
using namespace std;
IMAGE img_dino1, img_dino2;//左右脚的恐龙，两张图片
IMAGE img_dinojump, img_ducking1, img_ducking2;//跳跃恐龙，空中脚不动
//蹲下恐龙，脚会动
IMAGE img_background;
IMAGE img_cac1, img_cac2, img_cac3, img_cac4, img_cac5, img_cac6;//六张不同的仙人掌图片
IMAGE img_bird1, img_bird2;


#pragma comment(lib,"MSIMG32.LIB")//windows自带的一个处理
inline void putimage1(int x, int y, IMAGE* img)//直接使用putimage函数的话
{//图片素材周围会有黑色框，于是我们自己写了一个putimage1函数，有进行透明度处理
	int w = img->getwidth();
	int h = img->getheight();
	AlphaBlend(GetImageHDC(NULL), x, y, w, h,
		GetImageHDC(img), 0, 0, w, h, { AC_SRC_OVER,0,255,AC_SRC_ALPHA });
}



bool isCollidingWithCactus(int x_dino, int y_dino, int x_cac, int y_cac) {
	// cac1的半圆碰撞区域的圆心相对于cac1的偏移
	const int offset_x = -12; // 半圆圆心x坐标相对于cac1的偏移
	const int offset_y = 2;   // 半圆圆心y坐标相对于cac1的偏移
	const int radius = 70;    // 半圆半径

	// 计算半圆圆心的实际坐标
	int cx_cac = x_cac + offset_x;
	int cy_cac = y_cac + offset_y;

	// 计算恐龙与半圆圆心的x和y方向上的距离
	int dx = abs(x_dino - cx_cac);
	int dy = abs(y_dino - cy_cac);

	// 计算恐龙与半圆圆心的欧几里得距离的平方
	int distanceSquared = dx * dx + dy * dy;

	// 如果距离的平方小于等于半径的平方，并且恐龙在半圆圆心上方，则认为发生了碰撞
	//不完全是半圆，左右微调

		return (distanceSquared <= radius * radius) && (y_dino < cy_cac);


}






int main()
{
	initgraph(896, 512);//跑酷窗口
	BeginBatchDraw();//双缓冲

	loadimage(&img_background, _T("img/background.png"));//背景图片加载
	loadimage(&img_dino1, _T("img/DinoRun1.png"));//
	loadimage(&img_dino2, _T("img/DinoRun2.png"));
	loadimage(&img_dinojump, _T("img/DinoJump.png"));
	loadimage(&img_ducking1, _T("img/DinoDuck1.png"));
	loadimage(&img_ducking2, _T("img/DinoDuck2.png"));
	loadimage(&img_cac1, _T("img/LargeCactus1.png"));
	loadimage(&img_cac2, _T("img/LargeCactus2.png"));
	loadimage(&img_cac3, _T("img/LargeCactus3.png"));
	loadimage(&img_cac4, _T("img/SmallCactus1.png"));
	loadimage(&img_cac5, _T("img/SmallCactus2.png"));
	loadimage(&img_cac6, _T("img/SmallCactus3.png"));
	loadimage(&img_bird1, _T("img/Bird1.png"));
	loadimage(&img_bird2, _T("img/Bird2.png"));




	int x;
	int y=272;
	ExMessage msg;
	while (true)
	{

		if (peekmessage(&msg))
		{
			x = msg.x;
			y = msg.y;
			cout << x << " " << y << endl;
			cout << isCollidingWithCactus(x, y, 400, 292)<<endl;
			FlushBatchDraw();//双缓冲

		}	
		cleardevice();
		putimage(0, 0, &img_background);
		putimage(x, y, &img_dino1);
		putimage1(400, 292, &img_cac4);
		Sleep(10);
	}
	EndBatchDraw();

	return 0;
}
