#include "myclass.h"
#include "editdig.hpp"
#include<qmovie.h>
#include <thread>
#include<stdio.h>

using namespace cv;
using namespace std;



using namespace cv;
using namespace std;


MyClass::MyClass(QWidget *parent)
	: QMainWindow(parent)
{
	
	QDesktopWidget * dw = new QDesktopWidget();
	ui.setupUi(this);
	vf = new QCameraViewfinder(ui.verticalLayoutWidget);
	ui.verticalLayout->addWidget(vf);
	this->resize(dw->geometry().width(), dw->geometry().height());
	/*
	foreach(QCameraInfo info, QCameraInfo::availableCameras())
	{
		cam = new QCamera(info);
	}
	cam->setViewfinder(vf);
	vf->resize(dw->geometry().width(), dw->geometry().height());
	ui.verticalLayoutWidget->resize(dw->geometry().width(), dw->geometry().height());
<<<<<<< HEAD

=======
>>>>>>> 2ea6785f2c38dcfd06dd77455405a2a9e79e8fa4
	cam->setCaptureMode(QCamera::CaptureVideo);
	cam->start();
	*/

}

MyClass::~MyClass()
{

}

void MyClass::newDig() {


	if (ui.strBtn->text() == "start")
	{
		ui.strBtn->setText("save");

		VideoCapture capture(0);

	//	cam->stop();

		VideoWriter outputVideo;
		Mat frame;
		//capture >> frame;
		
		Size *s = new Size((int)frame.cols, (int)frame.rows);

		outputVideo.open("video.avi", -1,15, *s, true);

		/*if (!outputVideo.isOpened())
		{
			cout << "�������� �����ϱ� ���� �ʱ�ȭ �۾� �� ���� �߻�" << endl;
			return ;
		}*/

		//outputVideo.open("video.avi", -1,15, *s, true);

		if (!outputVideo.isOpened())
		{
			cout << "�������� �����ϱ� ���� �ʱ�ȭ �۾� �� ���� �߻�" << endl;
			return ;
		}


		if (!capture.isOpened())
		{
			cout << "��ķ�� ���� �����ϴ�." << endl;
			return ;
		}

		//ĸó ������ 640x480���� ����  
		capture.set(CAP_PROP_FRAME_WIDTH, 640);
		capture.set(CAP_PROP_FRAME_HEIGHT, 480);

		namedWindow("Cam", 1);
		int i = 0;

		int fps=30;
		//fps = capture.get(CV_CAP_PROP_FPS);
		//backGround bg;


	

		while(1)
		{
			//��ĸ���κ��� �� �������� �о��  
			

			i++;
			if (!capture.read(frame))
				return;		
		
			outputVideo << frame;
		
			//ȭ�鿡 ������ ������
			imshow("Cam", frame);
			
			//������ 5�� ������ ������ ����.
			if ((fps * 5) == i)
			{
				cvDestroyWindow("Cam");
				break;
			}
			//ESCŰ ������ ����
			if (waitKey(1) == 27)
			{
				cvDestroyWindow("Cam");
				break;
			}
		}

		outputVideo.release();

		QMessageBox msgbox;
		msgbox.setText("aaaaaaaaaaaaaaaaaaaaaaaaaaa");
		msgbox.setWindowTitle("making...");
		msgbox.show();
		backGround bg = backGround("video.avi");
		back = bg.bg;
		per = bg.person;
		msgbox.setWindowTitle("complete");
		msgbox.setText("complete making backGround.!!");
		msgbox.exec();
		editDig *edit = new editDig();
		edit->init(back, per,bg.AImg);
		ui.editBtn->setEnabled(true);

		


		///////////////////////////////////////////���⿡  opencv ķ 

	}
	else
	{

		QImage img;
		img.load("Blue.jpg");
		QPixmap *buf = new QPixmap();
		*buf = QPixmap::fromImage(img);

		QString name = QFileDialog::getSaveFileName(this, "save image", "untitle.png", "Images(*.png *.xpm *.jpg)");
		img.save(name);
	
	}
	 


}
void MyClass::newEdit() {

	

	
}
void MyClass::reStart() {

	ui.strBtn->setText("start");

}

void MyClass::processImage(QMediaRecorder::State *state)
{




	// sometime later, or on another press

	//shotDig mDig = new shotDig();

}


