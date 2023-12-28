#include<graphics.h>
#include<iostream>
#include<conio.h>
#include<vector>//采用动态数组来管理持续增加的cactus，cloud或bird
#include<ctime>//用于随机数生成
#include<map>//用于碰撞检测中的批量初始化
#include<cmath>

#pragma comment(lib,"winmm.lib")

using namespace std;
int bird_speed = 20;
int cac_speed = 15;//加速相关的几个速度参数
int speed_increase = 4;
int lastSpeedIncreaseScore = 0; // 新增一个变量，记录上次增加速度时的分数
const int width = 896;//地图长宽
const int height = 512;
const int SlowStep = 4;
const int FastStep = 2;
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
IMAGE img_bird1, img_bird2,img_shield,img_gameover;
//bool End = false;//撞到仙人掌或鸟，game over
int Life = 3;//撞到仙人掌或鸟，血量-1
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


	//这是碰撞检测的参数，基本上是最难的部分
	//我们注意到恐龙的跳跃或者说它与仙人掌碰撞的临界坐标近似可以模拟成一个抛物线
	//我们用半圆的思路去处理碰撞，同时加上几个边界使其尽可能接近抛物线

	int offset_x, offset_y;//半圆圆心相对于cactus坐标的偏移量，这是图片左右不对称导致的
	int radius;//半圆半径
	int leftBoundOffset;  // 左侧限制边界的偏移
	int rightBoundOffset; // 右侧限制边界的偏移
	int upperBoundOffset; // 上方限制边界的偏移
	Cactus() : x(0), y(0), Cactype(1), offset_x(0), offset_y(0), radius(0), leftBoundOffset(0), rightBoundOffset(0), upperBoundOffset(0) {}
	//先重载构造一个无参数的Cactus，否则vector的定义会报错
	Cactus(float x, float y, int Cactype, int ox, int oy, int r, int lbo, int rbo, int ubo)
		: x(x), y(y), Cactype(Cactype), offset_x(ox), offset_y(oy), radius(r), leftBoundOffset(lbo), rightBoundOffset(rbo), upperBoundOffset(ubo) {}
	//构造函数+初始化仙人掌相关数据
	//包括其xy坐标，是哪种类型的仙人掌
	//每个类型仙人掌有不同的边界框（半圆，抛物线）

	void draw()
	{
		switch (Cactype)
		{
		case 1:putimage1(x, y, &img_cac1); break;
		case 2:putimage1(x, y, &img_cac2); break;
		case 3:putimage1(x, y, &img_cac3); break;
		case 4:putimage1(x, y , &img_cac4); break;
		case 5:putimage1(x, y , &img_cac5); break;
		case 6:putimage1(x, y , &img_cac6); break;
		}
	}
	void move()
	{
		x -= cac_speed;//相对运动
	}

	bool crack(int x_dino, int y_dino) const
		//定义碰撞函数，判断恐龙坐标是否在碰撞框内
	{
		int ox = x + offset_x;//ox,oy是半圆圆心坐标
		int oy = y + offset_y;//offset是偏移量，这是图片左右不对称导致的
		
		int dx = abs(x_dino - ox);
		int dy = abs(y_dino - oy);

		int distanceSquared = dx * dx + dy * dy;

		int leftBound = ox - leftBoundOffset;
		int rightBound = ox + rightBoundOffset;
		int upperBound = oy - upperBoundOffset;

		return (distanceSquared <= radius * radius) &&
			(x_dino > leftBound) &&
			(x_dino < rightBound) &&
			(y_dino > upperBound) && (y_dino < oy);
		//需要同时满足2个条件才能被判定为在碰撞区域内
		//1.在圆内  2.未超过上下左右边界
		//这样的碰撞区域就可模拟抛物线
	}
};

class Bird//逻辑与仙人掌类似
{
public:
	float x, y;
	float vx;
	int Birdtype;//高鸟和低鸟

	int offset_x, offset_y;//半圆圆心相对于cactus坐标的偏移量，这是图片左右不对称导致的
	int radius;//半圆半径
	int leftBoundOffset;  // 左侧限制边界的偏移
	int rightBoundOffset; // 右侧限制边界的偏移
	int upperBoundOffset; // 上方限制边界的偏移
	int downBoundOffset; // 下方限制边界的偏移

	Bird() :x(0), Birdtype(1), offset_x(0), offset_y(0), radius(0), leftBoundOffset(0), rightBoundOffset(0), upperBoundOffset(0),downBoundOffset(0) {}
	Bird(float x, int Birdtype, int ox, int oy,int r, int lbo, int rbo, int ubo,int dbo) :x(x), Birdtype(Birdtype), offset_x(ox), offset_y(oy), radius(r), leftBoundOffset(lbo), rightBoundOffset(rbo), upperBoundOffset(ubo),downBoundOffset(dbo) {}
	void draw()
	{
		if (Birdtype == 1) y = height - 280;
		else if (Birdtype == 2) y = height - 330;
		if (bird_index == 0) putimage1(x, y, &img_bird1);//翅膀在下
		if (bird_index == 1) putimage1(x, y, &img_bird2);//翅膀在上
	}
	void move()
	{
		x -= bird_speed;//相对运动，让障碍物向左运动
		//看上去就像恐龙在向右运动
	}
	bool crack(int x_dino, int y_dino) const
		//定义碰撞函数，判断恐龙坐标是否在碰撞框内
	{
		int ox = x + offset_x;//ox,oy是半圆圆心坐标
		int oy = y + offset_y;//offset是偏移量，这是图片左右不对称导致的

		int dx = abs(x_dino - ox);
		int dy = abs(y_dino - oy);

		int distanceSquared = dx * dx + dy * dy;

		int leftBound = ox - leftBoundOffset;
		int rightBound = ox + rightBoundOffset;
		int upperBound = oy - upperBoundOffset;
		int downBound = oy + downBoundOffset;


		return (distanceSquared <= radius * radius) &&
			(x_dino > leftBound) &&
			(x_dino < rightBound) &&
			(y_dino > upperBound)&&(y_dino < downBound);
		//需要同时满足2个条件才能被判定为在碰撞区域内
		//1.在圆内  2.未超过上下左右边界
		//这样的碰撞区域就可模拟抛物线
	}
};



//恐龙类，竖直方向移动，水平方向不变
class Dino
{
public:
	float x, y;
	float vy, g;
	bool Jumping = false;//检测是否在跳跃
	bool Ducking = false;//检测是否在俯下身子或空中俯冲
	bool Invincible = false;//掉血一段时间处于无敌状态，避免重复判定掉血
	float ivtimer;//用于计时
	Dino(float x, float y, float vy, float g) :x(x), y(y), vy(vy), g(g) {}//构造函数，数据初始化
	void draw()
	{
		if (Invincible)
		{
			putimage1(x+5, y-60, &img_shield);
		}
		if (Jumping) putimage1(x, y, &img_dinojump);
		else if (Ducking)
		{
			if (dino_index == 0) putimage1(x, y + 30, &img_ducking1);//与小仙人掌相同的问题
			if (dino_index == 1) putimage1(x, y + 30, &img_ducking2);//ducking的图片高度不够
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
			vy += g;//基本物理逻辑
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
		if (Jumping) vy += 35;//如果正在跳跃的过程中按下键，则加速下坠
		//这时并不会蹲下
		else Ducking = true;
	}

	void StopDuck()
	{
		Ducking = false;
	}
	void updateInvincible()//无敌帧逻辑的实现
	{
		if (Invincible == true)
		{
			ivtimer -= 0.03;
			if (ivtimer <= 0) Invincible = false;
		}
	}
};

//初始化：加载图片
void startup()
{
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
	loadimage(&img_shield, _T("img/shield.png"));
	loadimage(&img_gameover, _T("img/gameover.png"));




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

		switch (Cactype) //这些参数是在另外一个测试代码中一点点调试出来的，调试了三个小时
		{
		case 1:
			cactus.push_back(Cactus(width, height - 240, Cactype, -12, 2, 70, 60, 65, 70));
			break;
		case 2:
			cactus.push_back(Cactus(width, height - 240, Cactype,10,2,90,90,90,85));
			break;
		case 3:
			cactus.push_back(Cactus(width, height - 240, Cactype, 10, 2, 88,88, 88, 80));
			break;
			//因为加载图片是从x，y开始向右下角加载的,所以 + 20是避免小仙人掌加载时悬在空中
		case 4:
			cactus.push_back(Cactus(width, height - 220, Cactype, -5, 2, 70, 70, 50, 70));
			break;
		case 5:
			cactus.push_back(Cactus(width, height - 220, Cactype, -2, 2, 78, 67, 67, 78));
			break;
		case 6:
			cactus.push_back(Cactus(width, height - 220, Cactype, 15, 2, 85, 85, 85, 80));
			break;
			
		}





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

		switch (Birdtype) //这些参数是在另外一个测试代码中一点点调试出来的，调试了三个小时
		{
		case 1:
			bird.push_back(Bird(width, Birdtype, 8, 11, 93, 93, 80, 93,63));
			break;
		case 2:
			bird.push_back(Bird(width,  Birdtype, 8, 11, 93, 93, 80, 93,63));
			break;
		}


		lastbirdtime = currenttime;
	}
}

void DrawEnd() 
{

	putimage1(width / 2-180, height / 2-50, &img_gameover);


	FlushBatchDraw();
	Sleep(3000); // 显示结束画面几秒钟

}



int main()
{
	srand(static_cast<unsigned int>(time(0)));//搭配rand使用，让每次随机不一样
	//我们用static静态声明，这样不会每次调用函数都间隔都是5s，从而实现随机数应有的效果
	ExMessage msg;//
	startup();
	initgraph(width, height);//跑酷窗口
	mciSendString(L"play music/Summer.mp3 repeat", 0, 0, 0);//播放音乐
	//mciSendString(_T("play bgm repeat from 0"), NULL, 0, NULL);//循环播放

	BeginBatchDraw();//双缓冲
	Dino dino1(200, height - 240, 0, 3.5);

	while (Life>=0)//后续改为！End
	{
		if (Life == 0)
		{
			DrawEnd();
			break;
		}


		AddCac();
		AddBird();
		static float Score = 0;
		Score += Score_Speed;//配合强制类型转换使用
		while (peekmessage(&msg))//信息输出，上箭头（或space）和下箭头
		{
			if (msg.message == WM_KEYDOWN)
			{
				if (msg.vkcode == VK_SPACE || msg.vkcode == VK_UP)//检测键盘的上键和空格键
				{
					dino1.StartJump();
				}
				else if (msg.vkcode == VK_DOWN)//检测键盘的下键
				{
					dino1.StartDuck();
				}
			}
			else if (msg.message == WM_KEYUP)//如果松开下键，停止蹲的动作
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

		settextcolor(GREEN);
		settextstyle(25, 0, _T("宋体"));
		TCHAR lifeStr[20];
		_stprintf_s(lifeStr, _T("血量: %d"), static_cast<int>(Life));
		outtextxy(width-100, 10, lifeStr);

		if (static_cast<int>(Score) > 50 + lastSpeedIncreaseScore)
		{
			//游戏难度随着分数增长而逐渐增加
			//具体体现在速度加快，我们直接把速度改为全局变量，防止作用域出问题
			cac_speed += speed_increase;
			bird_speed += speed_increase;
			lastSpeedIncreaseScore = static_cast<int>(Score);
		}
		dino1.draw();
		dino1.jump();
		dino1.updateInvincible();
		for (auto& c : cactus)//一种新的for循环，用于遍历vector这类的动态数组
		{
			c.draw();
			c.move();
			if (c.crack(dino1.x, dino1.y)&&dino1.Invincible==false)//碰撞与无敌帧的判定
			{
				Life--;//仙人掌碰撞，下面的鸟同理
				dino1.Invincible = true;
				dino1.ivtimer = 2.0;
				break;
			}
		}
		for (auto& b : bird)
		{
			b.draw();
			b.move();
			if (b.crack(dino1.x, dino1.y) && dino1.Invincible == false&&dino1.Ducking==false)//碰撞与无敌帧的判定
				//如果蹲下则绝对不会和鸟发生碰撞
			{
				Life--;//仙人掌碰撞，下面的鸟同理
				dino1.Invincible = true;
				dino1.ivtimer = 2.0;
				break;
			}
		}
		FlushBatchDraw();
		Sleep(30);
	}
	EndBatchDraw();
	closegraph();
	return 0;
}