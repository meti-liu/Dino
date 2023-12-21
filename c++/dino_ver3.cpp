#include<graphics.h>
#include<iostream>
#include<conio.h>
#include<vector>//采用动态数组来管理持续增加的cactus，cloud或bird
#include<ctime>//用于随机数生成
using namespace std;
const int width = 896;//地图长宽
const int height = 512;
unsigned long lasttime = 0;//我们尝试通过一个计时器来逐渐增加新的仙人掌
IMAGE img_dino1, img_dino2;//左右脚的恐龙，两张图片
IMAGE img_background;
IMAGE img_cac1, img_cac2, img_cac3, img_cac4, img_cac5, img_cac6;//六张不同的仙人掌图片
int dino_index;//有两张恐龙图片，循环播放，这里记录下标




#pragma comment(lib,"MSIMG32.LIB")//windows自带的一个处理
inline void putimage1(int x, int y, IMAGE* img)//直接使用putimage函数的话
{//图片素材周围会有黑色框，于是我们自己写了一个putimage1函数，有进行透明度处理
	int w = img->getwidth();
	int h = img->getheight();
	AlphaBlend(GetImageHDC(NULL), x, y, w, h,
		GetImageHDC(img), 0, 0, w, h, { AC_SRC_OVER,0,255,AC_SRC_ALPHA });
}

//仙人掌类
class Cactus
{
public:
	float x, y;
	float vx;//相对运动，障碍物向左，这里vx应该是负的
	int Cactype;
	Cactus() : x(0), y(0), vx(0), Cactype(1) {}//先重载构造一个无参数的Cactus，否则vector的定义会报错
	Cactus(float x,float y,float vx,int Cactype):x(x),y(y),vx(vx),Cactype(Cactype){}//构造函数+初始化仙人掌相关数据
	//包括其xy坐标，移动速度vx，是哪种类型的仙人掌
	void draw()
	{
		switch (Cactype)
		{
		case 1:putimage1(x, y, &img_cac1); break;
		case 2:putimage1(x, y, &img_cac2); break;
		case 3:putimage1(x, y, &img_cac3); break;
		case 4:putimage1(x, y+20, &img_cac4); break;
		case 5:putimage1(x, y+20, &img_cac5); break;
		case 6:putimage1(x, y+20, &img_cac6); break;
		}
	}
	void move()
	{
		x -= vx;
	}
};

//恐龙类，竖直方向移动，水平方向不变
class Dino
{
public:
	float x, y;
	float vy, g;
	bool Jumping;
	Dino(float x, float y, float vy, float g) :x(x),y(y),vy(vy),g(g){}//构造函数，数据初始化
	void draw()
	{
		if(dino_index==0) putimage1(x, y, &img_dino1);//左脚在下
		if (dino_index == 1) putimage1(x, y, &img_dino2);//右脚在下

	}
	void StartJump()
	{
		if (Jumping == false)
		{
			vy = -45;//y方向初速度向上
			Jumping = true;
		}
	}

	void jump()//跳跃运动的逻辑
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

//初始化：加载图片
void startup()
{
	loadimage(&img_background, _T("img/background.png"));//背景图片加载
	loadimage(&img_dino1, _T("img/DinoRun1.png"));//
	loadimage(&img_dino2, _T("img/DinoRun2.png"));
	loadimage(&img_cac1, _T("img/LargeCactus1.png"));
	loadimage(&img_cac2, _T("img/LargeCactus2.png"));
	loadimage(&img_cac3, _T("img/LargeCactus3.png"));
	loadimage(&img_cac4, _T("img/SmallCactus1.png"));
	loadimage(&img_cac5, _T("img/SmallCactus2.png"));
	loadimage(&img_cac6, _T("img/SmallCactus3.png"));

}
vector<Cactus> cactus;
void AddCac()//添加水果的函数，搭配计时器使用
{
	unsigned long currenttime = GetTickCount();//搭配最初定义的lasttime使用
	const unsigned long Cac_interval = 5000;//5000毫秒，5s
	if (currenttime - lasttime > Cac_interval)
	{
		int Cactype= rand() % 6 + 1;//rand()%3就是随机0或12345，这里用于六选一随机生成一个仙人掌
		cactus.push_back(Cactus(width,height-240,7,Cactype));//在vector中添加一个随机仙人掌
		//并且附上初始值，这里的7就是水平方向速度
		lasttime = currenttime;
	}

}
int main()
{
	srand(static_cast<unsigned int>(time(0)));//搭配rand使用，让每次随机不一样
	ExMessage msg;//
	startup();
	initgraph(width, height);//跑酷窗口
	BeginBatchDraw();//双缓冲
	Dino dino1(200, height-240, 0, 3.5);

	while (true)//后续改为！End
	{
		AddCac();

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


		static int counter1;//使用一个计数器，用于图片循环播放
		if (++counter1 % 5 == 0)//每5帧，切换一个恐龙（假设一次循环是一帧）
		{
			dino_index++;
		}
		dino_index = dino_index % 2;//一共就两张图，这里取模循环


		cleardevice();
		putimage(0, 0, &img_background);//背景图片渲染
		dino1.draw();
		dino1.jump();
		for (auto& c : cactus)
		{
			c.draw();
			c.move();
		}
		FlushBatchDraw();
		Sleep(30);
	}
	EndBatchDraw();
	closegraph();
	return 0;
}