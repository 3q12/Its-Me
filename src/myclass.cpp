#include "myclass.h"
#include "editdig.hpp"
#include<qmovie.h>
#include <thread>
#include<stdio.h>

using namespace cv;
using namespace std;


MyClass::MyClass(QWidget *parent)
	: QMainWindow(parent)
{

	QDesktopWidget * dw = new QDesktopWidget();
	ui.setupUi(this);
	vf = new QCameraViewfinder(ui.verticalLayoutWidget);
	ui.verticalLayout->addWidget(vf);
	
		/*
		foreach(QCameraInfo info, QCameraInfo::availableCameras())
		{
			cam = new QCamera(info);
		}
		cam->setViewfinder(vf);
		vf->resize(dw->geometry().width(), dw->geometry().height());
		ui.verticalLayoutWidget->resize(dw->geometry().width(), dw->geometry().height());
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

		QFileDialog fileDialog(this, tr("Open File"), tr("C:"), tr("movie (*.mp4 *.avi )"));
		QStringList fileNames;
		if (fileDialog.exec()) {
			fileNames = fileDialog.selectedFiles();
		}

		QString selectedFile;
		for (int nIndex = 0; nIndex < fileNames.size(); nIndex++) {
			selectedFile.append(fileNames.at(nIndex).toLocal8Bit().constData());
		}

		//	cam->stop();
		//capture >> frame;

		VideoCapture capture(selectedFile.toStdString());

		VideoWriter outputVideo;
		Mat frame;
	
		Size *s = new Size((int)frame.cols, (int)frame.rows);

		outputVideo.open("video.avi", 0, 30, *s, true);

		if (!outputVideo.isOpened())
		{
			cout << "동영상을 저장하기 위한 초기화 작업 중 에러 발생" << endl;
			return;
		}


		if (!capture.isOpened())
		{
			cout << "웹캠을 열수 없습니다." << endl;
			return;
		}

		//캡처 영상을 640x480으로 지정  
		capture.set(CAP_PROP_FRAME_WIDTH, 640);
		capture.set(CAP_PROP_FRAME_HEIGHT, 480);

		namedWindow("Cam", 1);
		int i = 0;

		int fps = 30;
		//fps = capture.get(CV_CAP_PROP_FPS);


		while (1)
		{
			//웹캡으로부터 한 프레임을 읽어옴  


			i++;
			if (!capture.read(frame))
				break;

			outputVideo << frame;

			//화면에 영상을 보여줌
			imshow("Cam", frame);

			//동영상 5초 저장이 끝나면 종료.
			if ((fps * 8) == i)
			{
				cvDestroyWindow("Cam");
				break;
			}
			//ESC키 누르면 종료
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
		edit->init(back, per, bg.AImg);
		ui.editBtn->setEnabled(true);

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
void MyClass::reStart() {

	ui.strBtn->setText("start");

}
