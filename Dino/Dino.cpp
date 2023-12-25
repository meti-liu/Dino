#include<graphics.h>
#include<iostream>
#include<conio.h>
#include<vector>//���ö�̬����������������ӵ�cactus��cloud��bird
#include<ctime>//�������������

#pragma comment(lib,"winmm.lib")

using namespace std;
int bird_speed = 20;
int cac_speed = 15;//������صļ����ٶȲ���
int speed_increase = 4;
int lastSpeedIncreaseScore = 0; // ����һ����������¼�ϴ������ٶ�ʱ�ķ���
const int width = 896;//��ͼ����
const int height = 512;
const int SlowStep =4;
const int FastStep =2;
const int SlowWing = 5;//���ǰ�һЩ����������Ϊȫ�ֱ�������������΢����ʱ��
//ֻ��Ҫ����ǰ��ľ͹���
const int FastWing = 3;
const float Score_Speed = 0.3;




unsigned long lastcactime = 0;//���ǳ���ͨ��һ����ʱ�����������µ�������
unsigned long lastbirdtime = 0;//���ǳ���ͨ��һ����ʱ�����������µ���

IMAGE img_dino1, img_dino2;//���ҽŵĿ���������ͼƬ
IMAGE img_dinojump, img_ducking1, img_ducking2;//��Ծ���������нŲ���
//���¿������Żᶯ
IMAGE img_background;
IMAGE img_cac1, img_cac2, img_cac3, img_cac4, img_cac5, img_cac6;//���Ų�ͬ��������ͼƬ
IMAGE img_bird1, img_bird2;
bool End = false;//ײ�������ƻ���game over
int dino_index;//�����ſ���ͼƬ��ѭ�����ţ������¼�±�
int bird_index;//ʵ����ĳ���ȶ���ѭ������
//to be done
//1.�ٶȼӿ죬�Ѷȵ���
//
//3.�ƺ���
//4.��ײ���
//5.����ֵ������Ѫ


#pragma comment(lib,"MSIMG32.LIB")//windows�Դ���һ������
inline void putimage1(int x, int y, IMAGE* img)//ֱ��ʹ��putimage�����Ļ�
{//ͼƬ�ز���Χ���к�ɫ�����������Լ�д��һ��putimage1�������н���͸���ȴ���
	int w = img->getwidth();
	int h = img->getheight();
	AlphaBlend(GetImageHDC(NULL), x, y, w, h,
		GetImageHDC(img), 0, 0, w, h, { AC_SRC_OVER,0,255,AC_SRC_ALPHA });
}

//��������
class Cactus
{
public:
	float x, y;
	float vx;//����˶����ϰ�����������vxӦ���Ǹ���
	int Cactype;
	Cactus() : x(0), y(0), Cactype(1) {}//�����ع���һ���޲�����Cactus������vector�Ķ���ᱨ��
	Cactus(float x,float y,int Cactype):x(x),y(y),Cactype(Cactype){}//���캯��+��ʼ���������������
	//������xy���꣬�ƶ��ٶ�vx�����������͵�������
	void draw()
	{
		switch (Cactype)
		{
		case 1:putimage1(x, y, &img_cac1); break;
		case 2:putimage1(x, y, &img_cac2); break;
		case 3:putimage1(x, y, &img_cac3); break;
		case 4:putimage1(x, y+20, &img_cac4); break;//��Ϊ����ͼƬ�Ǵ�x��y��ʼ�����½Ǽ��ص�
		case 5:putimage1(x, y+20, &img_cac5); break;//����+20�Ǳ���С�����Ƽ���ʱ���ڿ���
		case 6:putimage1(x, y+20, &img_cac6); break;
		}
	}
	void move()
	{
		x -= cac_speed;
	}
};

class Bird//�߼�������������
{
public:
	float x, y;
	float vx;
	int Birdtype;//����͵���
	Bird() :x(0),Birdtype(1){}
	Bird(float x,int Birdtype) :x(x),Birdtype(Birdtype){}
	void draw()
	{
		if (Birdtype == 1) y = height - 280;
		else if (Birdtype == 2) y = height - 330;
		if (bird_index == 0) putimage1(x, y, &img_bird1);//�������
		if (bird_index == 1) putimage1(x, y, &img_bird2);//�������
	}
	void move()
	{
		x -= bird_speed;
	}
};



//�����࣬��ֱ�����ƶ���ˮƽ���򲻱�
class Dino
{
public:
	float x, y;
	float vy, g;
	bool Jumping=false;//����Ƿ�����Ծ
	bool Ducking=false;//����Ƿ��ڸ������ӻ���и���
	Dino(float x, float y, float vy, float g) :x(x),y(y),vy(vy),g(g){}//���캯�������ݳ�ʼ��
	void draw()
	{
		if (Jumping) putimage1(x, y, &img_dinojump);
		else if (Ducking)
		{
			if (dino_index == 0) putimage1(x, y+30, &img_ducking1);//��С��������ͬ������
			if (dino_index == 1) putimage1(x, y+30, &img_ducking2);//ducking��ͼƬ�߶Ȳ���
			//���¿��������ڿ��У���������+30
		}
		else
		{
			if (dino_index == 0) putimage1(x, y, &img_dino1);//�������
			if (dino_index == 1) putimage1(x, y, &img_dino2);//�ҽ�����
		}

	}
	void StartJump()
	{
		if (Jumping == false)
		{
			vy = -40;//y������ٶ�����
			Jumping = true;
		}
	}

	void jump()//��Ծ�˶����߼�
	{
		if (Jumping)
		{
			vy += g;
			y += vy;
			if (y > height - 240)
			{
				y = height - 240;//�����ڵ�����
				vy = 0;//ֹͣ����
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

//��ʼ��������ͼƬ
void startup()
{
	loadimage(&img_background, _T("D:/game/lbr/Dino/img/background.png"));//����ͼƬ����
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
//�������½������ƵĲ���������������ɶ�
//1.�����1�����������������ѡһ��
//2.�����2��֤ÿ�������������Ƶ�ʱ������ȷ������������Ϸ�о���ÿ����������֮��ľ��벻���Ƕ�ֵ
void AddCac() 
{
	static unsigned long Cac_interval = 1500; // ��ʼĬ�ϼ��5000ms,5s
	unsigned long currenttime = GetTickCount();

	if (currenttime - lastcactime > Cac_interval)
	{
		int a[3] = { 1500, 2500, 4000 };
		int r0 = rand() % 3;
		Cac_interval = a[r0]; // ������һ�����ʱ��

		cout << "Current interval: " << Cac_interval << " milliseconds" << endl;

		int Cactype = rand() % 6 + 1;//�������������������ѡһ������
		cactus.push_back(Cactus(width, height - 240, Cactype));
		lastcactime = currenttime;
	}
}
void AddBird()
{
	static unsigned long Bird_interval = 5000; // ��ʼĬ�ϼ��5000ms,5s
	unsigned long currenttime = GetTickCount();

	if (currenttime - lastbirdtime > Bird_interval)
	{
		int b[3] = { 5000, 6000, 4000 };
		int r1 = rand() % 3;
		Bird_interval = b[r1]; // ������һ�����ʱ��

		cout << "Current interval: " << Bird_interval << " milliseconds" << endl;

		int Birdtype = rand() % 2 + 1;//�����������Ͱ���
		bird.push_back(Bird(width,Birdtype));
		lastbirdtime = currenttime;
	}
}

int main()
{
	srand(static_cast<unsigned int>(time(0)));//����randʹ�ã���ÿ�������һ��
	//������static��̬��������������ÿ�ε��ú������������5s���Ӷ�ʵ�������Ӧ�е�Ч��
	ExMessage msg;//
	startup();
	initgraph(width, height);//�ܿᴰ��
	mciSendString(L"play D:/game/lbr/Dino/music/Summer.mp3 repeat", 0, 0, 0);//��������
	//mciSendString(_T("play bgm repeat from 0"), NULL, 0, NULL);//ѭ������

	BeginBatchDraw();//˫����
	Dino dino1(200, height-240, 0, 3.5);

	while (true)//������Ϊ��End
	{



		AddCac();
		AddBird();
		static float Score = 0;
		Score+= Score_Speed;//���ǿ������ת��ʹ��
		while (peekmessage(&msg))//��Ϣ������ϼ�ͷ����space�����¼�ͷ
		{
			if (msg.message == WM_KEYDOWN)
			{
				if (msg.vkcode == VK_SPACE || msg.vkcode == VK_UP)//�����̵��ϼ��Ϳո��
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


		static int counter1;//ʹ��һ��������������ͼƬѭ������
		if (static_cast<int>(Score) < 200)
		{
			if (++counter1 % SlowStep == 0)//ÿ3֡���л�һ������������һ��ѭ����һ֡��
			{
				dino_index++;
			}
			dino_index = dino_index % 2;//һ��������ͼ������ȡģѭ��
		}
		else
		{
			if (++counter1 % FastStep == 0)//ÿ3֡���л�һ������������һ��ѭ����һ֡��
			{
				dino_index++;
			}
			dino_index = dino_index % 2;//һ��������ͼ������ȡģѭ��
		}







		static int counter2;//ʹ��һ����������������ͼƬѭ������
		if (static_cast<int>(Score) < 200)
		{
			if (++counter2 % SlowWing == 0)//ÿ3֡���л�һ���񣨼���һ��ѭ����һ֡��
			{
				bird_index++;
			}
			bird_index = bird_index % 2;//һ��������ͼ������ȡģѭ��
		}
		else
		{
			if (++counter2 % FastWing == 0)//ÿ3֡���л�һ���񣨼���һ��ѭ����һ֡��
			{
				bird_index++;
			}
			bird_index = bird_index % 2;//һ��������ͼ������ȡģѭ��
		}


		cleardevice();
		putimage(0, 0, &img_background);//����ͼƬ��Ⱦ
		//�Ʒְ壬ֱ�Ӱ�ˮ�����ߵļƷְ�������
		settextcolor(WHITE);
		settextstyle(25, 0, _T("����"));
		TCHAR scoreStr[20];
		_stprintf_s(scoreStr, _T("�÷�: %d"), static_cast<int>(Score));
		outtextxy(10, 10, scoreStr);
		if (static_cast<int>(Score)>50+lastSpeedIncreaseScore)
		{
			cac_speed += speed_increase;
			bird_speed += speed_increase;
			lastSpeedIncreaseScore = static_cast<int>(Score);
		}
		dino1.draw();
		dino1.jump();
		for (auto& c : cactus)//һ���µ�forѭ�������ڱ���vector����Ķ�̬����
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