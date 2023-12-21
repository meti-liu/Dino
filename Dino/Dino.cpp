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
IMAGE img_background;
IMAGE img_cac1, img_cac2, img_cac3, img_cac4, img_cac5, img_cac6;//���Ų�ͬ��������ͼƬ
int dino_index;//�����ſ���ͼƬ��ѭ�����ţ������¼�±�




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
	bool Jumping;
	Dino(float x, float y, float vy, float g) :x(x),y(y),vy(vy),g(g){}//���캯�������ݳ�ʼ��
	void draw()
	{
		if(dino_index==0) putimage1(x, y, &img_dino1);//�������
		if (dino_index == 1) putimage1(x, y, &img_dino2);//�ҽ�����

	}
	void StartJump()
	{
		if (Jumping == false)
		{
			vy = -45;//y������ٶ�����
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
};

//��ʼ��������ͼƬ
void startup()
{
	loadimage(&img_background, _T("img/background.png"));//����ͼƬ����
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
void AddCac()//���ˮ���ĺ����������ʱ��ʹ��
{
	unsigned long currenttime = GetTickCount();//������������lasttimeʹ��
	const unsigned long Cac_interval = 5000;//5000���룬5s
	if (currenttime - lasttime > Cac_interval)
	{
		int Cactype= rand() % 6 + 1;//rand()%3�������0��12345������������ѡһ�������һ��������
		cactus.push_back(Cactus(width,height-240,7,Cactype));//��vector�����һ�����������
		//���Ҹ��ϳ�ʼֵ�������7����ˮƽ�����ٶ�
		lasttime = currenttime;
	}

}
int main()
{
	srand(static_cast<unsigned int>(time(0)));//����randʹ�ã���ÿ�������һ��
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