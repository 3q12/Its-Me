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
	
		/* ��ĸ�� ����ؼ� ������Ʈ�� �����Ҷ� ���� �ڵ� 
		foreach(QCameraInfo info, QCameraInfo::availableCameras())
		{
			cam = new QCamera(info);//��ĸ�� ������ �����´�
		}
		cam->setViewfinder(vf);
		vf->resize(dw->geometry().width(), dw->geometry().height());
		ui.verticalLayoutWidget->resize(dw->geometry().width(), dw->geometry().height());//������ ũ�⸦ ī�޶� ���� ũ��� �������ش�.
		cam->setCaptureMode(QCamera::CaptureVideo);
		cam->start();
		*/

}

MyClass::~MyClass()
{

}

void MyClass::newDig() {
	//ù��° ��ư�� �������� �� �κ��� ����ȴ�.

	if (ui.strBtn->text() == "start")//��ư�� ���°� start �� ��� 
	{
		ui.strBtn->setText("save");//��ư�� ���¸� save��  �������ش�

		QFileDialog fileDialog(this, tr("Open File"), tr("C:"), tr("movie (*.mp4 *.avi )"));//������ ���� ���� ���̾�α� ���� 
		QStringList fileNames;
		if (fileDialog.exec()) {
			fileNames = fileDialog.selectedFiles();
		}

		QString selectedFile;
		for (int nIndex = 0; nIndex < fileNames.size(); nIndex++) {
			selectedFile.append(fileNames.at(nIndex).toLocal8Bit().constData());
		}
		/*��ķ ���� �ڵ�
			cam->stop();
		capture >> frame;
		*/
		VideoCapture capture(selectedFile.toStdString());//���õ� �������� �������� �����´�
		VideoWriter outputVideo;
		Mat frame;
	
		Size *s = new Size((int)frame.cols, (int)frame.rows);

		outputVideo.open("video.avi", 0, 30, *s, true);

	/*	if (!outputVideo.isOpened())
		{
			cout << "�������� �����ϱ� ���� �ʱ�ȭ �۾� �� ���� �߻�" << endl;
			return;
		}
*/

		if (!capture.isOpened())
		{
			cout << "��ķ�� ���� �����ϴ�." << endl;
			return;
		}

		//ĸó ������ 640x480���� ����  
		capture.set(CAP_PROP_FRAME_WIDTH, 640);
		capture.set(CAP_PROP_FRAME_HEIGHT, 480);

		namedWindow("Cam", 1);
		int i = 0;

		int fps = 30;

		while (1)
		{
			//��ĸ���κ��� �� �������� �о��  


			i++;
			if (!capture.read(frame))//�о�� �������� ���ٸ� 
				break;//����

			outputVideo << frame;

			//ȭ�鿡 ������ ������
			imshow("Cam", frame);

			//������ 5�� ������ ������ ����.
			if ((fps * 8) == i)
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
