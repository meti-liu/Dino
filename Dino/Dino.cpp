#include<graphics.h>
#include<iostream>
#include<conio.h>
#include<vector>//���ö�̬����������������ӵ�cactus��cloud��bird
#include<ctime>//�������������
using namespace std;
const int width = 896;//��ͼ����
const int height = 512;
unsigned long lasttime = 0;//���ǳ���ͨ��һ����ʱ�����������µ�������
IMAGE img_dino1, img_dino2;//���ҽŵĿ���������ͼƬ
IMAGE img_dinojump, img_ducking1, img_ducking2;//��Ծ���������нŲ���
//���¿������Żᶯ
IMAGE img_background;
IMAGE img_cac1, img_cac2, img_cac3, img_cac4, img_cac5, img_cac6;//���Ų�ͬ��������ͼƬ
int dino_index;//�����ſ���ͼƬ��ѭ�����ţ������¼�±�

//to be done
//1.�ٶȼӿ죬�Ѷȵ���
//2.ducking��ض���
//3.�ƺ���
//4.��ײ���
//5.


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
	Cactus() : x(0), y(0), vx(0), Cactype(1) {}//�����ع���һ���޲�����Cactus������vector�Ķ���ᱨ��
	Cactus(float x,float y,float vx,int Cactype):x(x),y(y),vx(vx),Cactype(Cactype){}//���캯��+��ʼ���������������
	//������xy���꣬�ƶ��ٶ�vx�����������͵�������
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
			if (dino_index == 0) putimage1(x, y+30, &img_ducking1);//�������
			if (dino_index == 1) putimage1(x, y+30, &img_ducking2);//�ҽ�����
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
	loadimage(&img_background, _T("img/background.png"));//����ͼƬ����
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

}
vector<Cactus> cactus;

//�������½������ƵĲ���������������ɶ�
//1.�����1�����������������ѡһ��
//2.�����2��֤ÿ�������������Ƶ�ʱ������ȷ������������Ϸ�о���ÿ����������֮��ľ��벻���Ƕ�ֵ
void AddCac() {
	static unsigned long Cac_interval = 5000; // ��ʼĬ�ϼ��5000ms,5s
	unsigned long currenttime = GetTickCount();

	if (currenttime - lasttime > Cac_interval) {
		int a[3] = { 5000, 2500, 4000 };
		int r0 = rand() % 3;
		Cac_interval = a[r0]; // ������һ�����ʱ��

		cout << "Current interval: " << Cac_interval << " milliseconds" << endl;

		int Cactype = rand() % 6 + 1;
		cactus.push_back(Cactus(width, height - 240, 10, Cactype));
		lasttime = currenttime;
	}
}

int main()
{
	srand(static_cast<unsigned int>(time(0)));//����randʹ�ã���ÿ�������һ��
	//������static��̬��������������ÿ�ε��ú������������5s���Ӷ�ʵ�������Ӧ�е�Ч��
	ExMessage msg;//
	startup();
	initgraph(width, height);//�ܿᴰ��
	BeginBatchDraw();//˫����
	Dino dino1(200, height-240, 0, 3.5);

	while (true)//������Ϊ��End
	{
		AddCac();

		while (peekmessage(&msg))//��Ϣ������ϼ�ͷ����space�����¼�ͷ
		{
			if (msg.message == WM_KEYDOWN)
			{
				if (msg.vkcode == VK_SPACE || msg.vkcode == VK_UP)
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
		if (++counter1 % 5 == 0)//ÿ5֡���л�һ������������һ��ѭ����һ֡��
		{
			dino_index++;
		}
		dino_index = dino_index % 2;//һ��������ͼ������ȡģѭ��


		cleardevice();
		putimage(0, 0, &img_background);//����ͼƬ��Ⱦ
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