#include<graphics.h>
#include<iostream>
#include<conio.h>
#include<vector>//采用动态数组来管理持续增加的cactus，cloud或bird
#include<ctime>//用于随机数生成

#pragma comment(lib,"winmm.lib")

using namespace std;
int bird_speed = 20;
int cac_speed = 15;//加速相关的几个速度参数
int speed_increase = 4;
int lastSpeedIncreaseScore = 0; // 新增一个变量，记录上次增加速度时的分数
const int width = 896;//地图长宽
const int height = 512;
const int SlowStep =4;
const int FastStep =2;
const int SlowWing = 5;//我们把一些参数都设置为全局变量，这样后续微调的时候
//只需要在最前面改就够了
const int FastWing = 3;
const float Score_Speed = 0.3;




unsigned long lastcactime = 0;//我们尝试通过一个计时器来逐渐增加新的仙人掌
unsigned long lastbirdtime = 0;//我们尝试通过一个计时器来逐渐增加新的鸟

IMAGE img_dino1, img_dino2;//左右脚的恐龙，两张图片
IMAGE img_dinojump, img_ducking1, img_ducking2;//跳跃恐龙，空中脚不动
//蹲下恐龙，脚会动
IMAGE img_background;
IMAGE img_cac1, img_cac2, img_cac3, img_cac4, img_cac5, img_cac6;//六张不同的仙人掌图片
IMAGE img_bird1, img_bird2;
bool End = false;//撞到仙人掌或鸟，game over
int dino_index;//有两张恐龙图片，循环播放，这里记录下标
int bird_index;//实现鸟的翅膀扇动，循环播放
//to be done
//1.速度加快，难度递增
//
//3.云和鸟
//4.碰撞检测
//5.生命值，三条血


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
	Cactus() : x(0), y(0), Cactype(1) {}//先重载构造一个无参数的Cactus，否则vector的定义会报错
	Cactus(float x,float y,int Cactype):x(x),y(y),Cactype(Cactype){}//构造函数+初始化仙人掌相关数据
	//包括其xy坐标，移动速度vx，是哪种类型的仙人掌
	void draw()
	{
		switch (Cactype)
		{
		case 1:putimage1(x, y, &img_cac1); break;
		case 2:putimage1(x, y, &img_cac2); break;
		case 3:putimage1(x, y, &img_cac3); break;
		case 4:putimage1(x, y+20, &img_cac4); break;//因为加载图片是从x，y开始向右下角加载的
		case 5:putimage1(x, y+20, &img_cac5); break;//所以+20是避免小仙人掌加载时悬在空中
		case 6:putimage1(x, y+20, &img_cac6); break;
		}
	}
	void move()
	{
		x -= cac_speed;
	}
};

class Bird//逻辑与仙人掌类似
{
public:
	float x, y;
	float vx;
	int Birdtype;//高鸟和低鸟
	Bird() :x(0),Birdtype(1){}
	Bird(float x,int Birdtype) :x(x),Birdtype(Birdtype){}
	void draw()
	{
		if (Birdtype == 1) y = height - 280;
		else if (Birdtype == 2) y = height - 330;
		if (bird_index == 0) putimage1(x, y, &img_bird1);//翅膀在下
		if (bird_index == 1) putimage1(x, y, &img_bird2);//翅膀在上
	}
	void move()
	{
		x -= bird_speed;
	}
};



//恐龙类，竖直方向移动，水平方向不变
class Dino
{
public:
	float x, y;
	float vy, g;
	bool Jumping=false;//检测是否在跳跃
	bool Ducking=false;//检测是否在俯下身子或空中俯冲
	Dino(float x, float y, float vy, float g) :x(x),y(y),vy(vy),g(g){}//构造函数，数据初始化
	void draw()
	{
		if (Jumping) putimage1(x, y, &img_dinojump);
		else if (Ducking)
		{
			if (dino_index == 0) putimage1(x, y+30, &img_ducking1);//与小仙人掌相同的问题
			if (dino_index == 1) putimage1(x, y+30, &img_ducking2);//ducking的图片高度不够
			//导致恐龙悬浮在空中，所以我们+30
		}
		else
		{
			if (dino_index == 0) putimage1(x, y, &img_dino1);//左脚在下
			if (dino_index == 1) putimage1(x, y, &img_dino2);//右脚在下
		}

	}
	void StartJump()
	{
		if (Jumping == false)
		{
			vy = -40;//y方向初速度向上
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

	void StartDuck()
	{
		if (Jumping) vy += 35;
		else Ducking = true;
	}

	void StopDuck()
	{
		Ducking = false;
	}
};

//初始化：加载图片
void startup()
{
	loadimage(&img_background, _T("D:/game/lbr/Dino/img/background.png"));//背景图片加载
	loadimage(&img_dino1, _T("D:/game/lbr/Dino/img/DinoRun1.png"));//
	loadimage(&img_dino2, _T("D:/game/lbr/Dino/img/DinoRun2.png"));
	loadimage(&img_dinojump, _T("D:/game/lbr/Dino/img/DinoJump.png"));
	loadimage(&img_ducking1, _T("D:/game/lbr/Dino/img/DinoDuck1.png"));
	loadimage(&img_ducking2, _T("D:/game/lbr/Dino/img/DinoDuck2.png"));
	loadimage(&img_cac1, _T("D:/game/lbr/Dino/img/LargeCactus1.png"));
	loadimage(&img_cac2, _T("D:/game/lbr/Dino/img/LargeCactus2.png"));
	loadimage(&img_cac3, _T("D:/game/lbr/Dino/img/LargeCactus3.png"));
	loadimage(&img_cac4, _T("D:/game/lbr/Dino/img/SmallCactus1.png"));
	loadimage(&img_cac5, _T("D:/game/lbr/Dino/img/SmallCactus2.png"));
	loadimage(&img_cac6, _T("D:/game/lbr/Dino/img/SmallCactus3.png"));
	loadimage(&img_bird1, _T("D:/game/lbr/Dino/img/Bird1.png"));
	loadimage(&img_bird2, _T("D:/game/lbr/Dino/img/Bird2.png"));


}
vector<Cactus> cactus;
vector<Bird> bird;
//我们在新建仙人掌的部分设计了两个自由度
//1.随机数1从六种仙人掌中随机选一个
//2.随机数2保证每次生成新仙人掌的时间间隔不确定，表现在游戏中就是每两个仙人掌之间的距离不总是定值
void AddCac() 
{
	static unsigned long Cac_interval = 1500; // 初始默认间隔5000ms,5s
	unsigned long currenttime = GetTickCount();

	if (currenttime - lastcactime > Cac_interval)
	{
		int a[3] = { 1500, 2500, 4000 };
		int r0 = rand() % 3;
		Cac_interval = a[r0]; // 更新下一个间隔时间

		cout << "Current interval: " << Cac_interval << " milliseconds" << endl;

		int Cactype = rand() % 6 + 1;//随机数，六种仙人掌任选一个生成
		cactus.push_back(Cactus(width, height - 240, Cactype));
		lastcactime = currenttime;
	}
}
void AddBird()
{
	static unsigned long Bird_interval = 5000; // 初始默认间隔5000ms,5s
	unsigned long currenttime = GetTickCount();

	if (currenttime - lastbirdtime > Bird_interval)
	{
		int b[3] = { 5000, 6000, 4000 };
		int r1 = rand() % 3;
		Bird_interval = b[r1]; // 更新下一个间隔时间

		cout << "Current interval: " << Bird_interval << " milliseconds" << endl;

		int Birdtype = rand() % 2 + 1;//随机数，高鸟和矮鸟
		bird.push_back(Bird(width,Birdtype));
		lastbirdtime = currenttime;
	}
}

int main()
{
	srand(static_cast<unsigned int>(time(0)));//搭配rand使用，让每次随机不一样
	//我们用static静态声明，这样不会每次调用函数都间隔都是5s，从而实现随机数应有的效果
	ExMessage msg;//
	startup();
	initgraph(width, height);//跑酷窗口
	mciSendString(L"play D:/game/lbr/Dino/music/Summer.mp3 repeat", 0, 0, 0);//播放音乐
	//mciSendString(_T("play bgm repeat from 0"), NULL, 0, NULL);//循环播放

	BeginBatchDraw();//双缓冲
	Dino dino1(200, height-240, 0, 3.5);

	while (true)//后续改为！End
	{



		AddCac();
		AddBird();
		static float Score = 0;
		Score+= Score_Speed;//配合强制类型转换使用
		while (peekmessage(&msg))//信息输出，上箭头（或space）和下箭头
		{
			if (msg.message == WM_KEYDOWN)
			{
				if (msg.vkcode == VK_SPACE || msg.vkcode == VK_UP)//检测键盘的上键和空格键
				{
					dino1.StartJump();
				}
				else if (msg.vkcode == VK_DOWN)
				{
					dino1.StartDuck();
				}
			}
			else if (msg.message == WM_KEYUP)
			{
				if (msg.vkcode == VK_DOWN) dino1.StopDuck();
			}
		}


		static int counter1;//使用一个计数器，用于图片循环播放
		if (static_cast<int>(Score) < 200)
		{
			if (++counter1 % SlowStep == 0)//每3帧，切换一个恐龙（假设一次循环是一帧）
			{
				dino_index++;
			}
			dino_index = dino_index % 2;//一共就两张图，这里取模循环
		}
		else
		{
			if (++counter1 % FastStep == 0)//每3帧，切换一个恐龙（假设一次循环是一帧）
			{
				dino_index++;
			}
			dino_index = dino_index % 2;//一共就两张图，这里取模循环
		}







		static int counter2;//使用一个计数器，用于鸟图片循环播放
		if (static_cast<int>(Score) < 200)
		{
			if (++counter2 % SlowWing == 0)//每3帧，切换一个鸟（假设一次循环是一帧）
			{
				bird_index++;
			}
			bird_index = bird_index % 2;//一共就两张图，这里取模循环
		}
		else
		{
			if (++counter2 % FastWing == 0)//每3帧，切换一个鸟（假设一次循环是一帧）
			{
				bird_index++;
			}
			bird_index = bird_index % 2;//一共就两张图，这里取模循环
		}


		cleardevice();
		putimage(0, 0, &img_background);//背景图片渲染
		//计分板，直接把水果忍者的计分板搬过来了
		settextcolor(WHITE);
		settextstyle(25, 0, _T("宋体"));
		TCHAR scoreStr[20];
		_stprintf_s(scoreStr, _T("得分: %d"), static_cast<int>(Score));
		outtextxy(10, 10, scoreStr);
		if (static_cast<int>(Score)>50+lastSpeedIncreaseScore)
		{
			cac_speed += speed_increase;
			bird_speed += speed_increase;
			lastSpeedIncreaseScore = static_cast<int>(Score);
		}
		dino1.draw();
		dino1.jump();
		for (auto& c : cactus)//一种新的for循环，用于遍历vector这类的动态数组
		{
			c.draw();
			c.move();
		}
		for (auto& b : bird)
		{
			b.draw();
			b.move();
		}
		FlushBatchDraw();
		Sleep(30);
	}
	EndBatchDraw();
	closegraph();
	return 0;
}