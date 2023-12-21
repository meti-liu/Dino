#include<graphics.h>
#include<iostream>
#include<conio.h>
using namespace std;
const int width = 896;//��ͼ����
const int height = 512;
IMAGE img_dino1, img_dino2, img_dino3;//������ͬ��ɫ�Ŀ���
IMAGE img_background;
int dino_index;//�����ſ���ͼƬ��ѭ�����ţ������¼�±�

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

	void jump()//�����˶��ĸ���
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
void startup()
{
	loadimage(&img_background, _T("img/background.png"));//����ͼƬ����
	loadimage(&img_dino1, _T("img/DinoRun1.png"));//
	loadimage(&img_dino2, _T("img/DinoRun2.png"));
	loadimage(&img_dino3, _T("img/dino3.png"));
}
int main()
{
	ExMessage msg;//
	startup();
	initgraph(width, height);//�ܿᴰ��
	BeginBatchDraw();//˫����
	Dino dino1(200, height-240, 0, 5);
	while (true)//������Ϊ��End
	{
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
		static int counter;//ʹ��һ��������������ͼƬѭ������
		if (++counter % 5 == 0)//ÿ5֡���л�һ������������һ��ѭ����һ֡��
		{
			dino_index++;
		}
		dino_index = dino_index % 2;//һ��������ͼ������ȡģѭ��


		cleardevice();
		putimage(0, 0, &img_background);//����ͼƬ��Ⱦ
		dino1.draw();
		dino1.jump();
		FlushBatchDraw();
		Sleep(30);
	}
	EndBatchDraw();
	closegraph();
	return 0;
}