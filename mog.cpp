
#include "opencv\cv.h"
#include "opencv\highgui.h"
#include "opencv\cxcore.h"
#include "opencv\cvaux.h"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
//C
#include <stdio.h>
//C++
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <thread>

#define DEGREE 10

using namespace cv;
using namespace std;

int sub_Bground(char *videoFile);
void changeGray(Mat &Img);
void cvDiff(Mat &image, Mat &image2, Mat &diff);
void copy(const Mat &Img, Mat &result, Mat &mask);
void cal_Degree(vector <Mat> &frame, int start, int end); 
int return_Max(vector <int> &agrDegree); 
void gray(char *videoFile);

Mat des;

int main(int argc, char *argv[])
{
	//cvNamedWindow("Image");
	//cvNamedWindow("Image2");
	//cvNamedWindow("tmpImage");
	//cvNamedWindow("diffImage");
	//cvNamedWindow("resultImage");
	//if (sub_Bground("3.mp4") == 1)
	//	return 1;
	//if (cvWaitKey() == 27) 
	//	; // break���� �ٲ� Ű�Է¹��������� �������̵�

	gray("3.mp4");
	cvDestroyWindow("tt");
	
	return 0;
}
void gray(char *videoFile)
{
	VideoCapture capture(videoFile);  // ���� ���� �б�
	if (!capture.isOpened()) {
		//error in opening the video input
		cerr << "Unable to open video file: " << "tt" << endl;
		exit(EXIT_FAILURE);
	}
	Mat Img = imread("result.bmp", IMREAD_GRAYSCALE);
	Mat diff = Mat(Img.rows, Img.cols, CV_8UC1, Scalar(0));
	Mat readImg = Mat(Img.rows, Img.cols, CV_8UC1, Scalar(0));
	Mat tmp = Mat(Img.rows, Img.cols, CV_8UC1, Scalar(0));

	while (capture.read(readImg)) {
		cvtColor(readImg, readImg, CV_BGR2GRAY);
		absdiff(Img, readImg, diff); // ù �����Ӱ� �� �����Ӹ����� �������� diff�� ����
		threshold(diff, tmp, 30, 255, CV_THRESH_BINARY); // �������� ����ȭ ���Ѽ� tmp�� ����
		erode(tmp, tmp, Mat(), Point(-1, -1), 3);
		imshow("tmp", tmp);
		imshow("Img", Img);
		imshow("readImg", readImg);
		if (cvWaitKey() == 27) continue; // break���� �ٲ� Ű�Է¹��������� �������̵�

	}

}
int sub_Bground(char *videoFile)
{
	VideoCapture capture(videoFile);  // ���� ���� �б�
	VideoWriter vw;
	vw = VideoWriter("AA.mp4", CV_FOURCC('D', 'I', 'V', 'X'), capture.get(CV_CAP_PROP_FPS), Size((int)capture.get(CV_CAP_PROP_FRAME_WIDTH), (int)capture.get(CV_CAP_PROP_FRAME_HEIGHT)), true);
	vector<Mat> src;

	if (!vw.isOpened())
	{
		cout << "�������� �����ϱ� ���� �ʱ�ȭ �۾� �� ���� �߻�" << endl;
		return 1;
	}
	if (!capture.isOpened()) {
		//error in opening the video input
		cerr << "Unable to open video file: " << "tt" << endl;
		exit(EXIT_FAILURE);
	}


	Mat Img;
	capture.read(Img); // ó�������� Img�� ����

	//Mat Img = imread("result.bmp"); // Img�� ����� ���� �̹��� ����
	Mat readImg = Mat(Img.rows, Img.cols, CV_8UC3, Scalar(0, 0, 0)); // 
	Mat diff = Mat(Img.rows, Img.cols, CV_8UC3, Scalar(0, 0, 0));
	Mat tmp = Mat(Img.rows, Img.cols, CV_8UC3, Scalar(0, 0, 0));
	des = Mat(Img.rows, Img.cols, CV_8UC3, Scalar(0, 0, 0)); // ����� ������ Mat

	Ptr<BackgroundSubtractor> pMOG2; //MOG2 Background subtractor
	pMOG2 = createBackgroundSubtractorMOG2();
	int check = 0;
	vector <Mat> frame;

	while (capture.read(readImg))
	{
		check++;
		if (check % 5 != 0)
			continue;
		Mat result = Mat(Img.rows, Img.cols, CV_8UC3, Scalar(0, 0, 0));

		absdiff(Img, readImg, diff); // ù �����Ӱ� �� �����Ӹ����� �������� diff�� ����
		threshold(diff, tmp, 20, 255, CV_THRESH_BINARY); // �������� ����ȭ ���Ѽ� tmp�� ����

														 //cvtColor(tmp, tmp, CV_BGR2GRAY);
		changeGray(tmp); // ����ȭ ��Ų tmp�� �Ϻ��ϰ� ����ȭ

		pMOG2->apply(tmp, diff); // ����ȭ ��Ų ���� ������ MOG2
		erode(diff, diff, Mat(), Point(-1, -1), 3); 
		//blur(diff, diff, Size(5, 5)); // noise ����
		diff = ~diff;
		readImg.copyTo(result, diff); // ��ü ������ ������ �ٿ��ִ´�.


		frame.push_back(result);

		/*	if (frame.size() == 10) {
		checkDegree(frame);
		src.push_back(Img3);
		frame.clear();
		}*/
		imshow("Image", Img);
		imshow("Image2", readImg);
		imshow("diffImage", diff);
		imshow("tmpImage", tmp);
		imshow("resultImage", result);

		if (cvWaitKey() == 27) continue; // break���� �ٲ� Ű�Է¹��������� �������̵�

	}

	int size = frame[0].rows / 6; // thread ����
	thread t1(&cal_Degree, frame, 0, size);
	thread t2(&cal_Degree, frame, size, size * 2);
	thread t3(&cal_Degree, frame, size * 2, size * 3);
	thread t4(&cal_Degree, frame, size * 3, size * 4);
	thread t5(&cal_Degree, frame, size * 4, size * 5);
	thread t6(&cal_Degree, frame, size * 5, size * 6);

	t1.join();
	t2.join();
	t3.join();
	t4.join();
	t5.join();
	t6.join();

	imwrite("result.bmp", des);
	capture.release();
	vw.release();

	return 0;
}
void cal_Degree(vector <Mat> &frame, int start, int end) {  // ���� ������ �����ӵ� , des�� ������ ��ǥ��
	printf("%d %d %d\n",frame.size(), start, end);
	Vec3b* desMat = (Vec3b*)des.data;
	for (int i = start; i < end; i++) {
		for (int j = 0; j < frame[0].cols; j++) {
			vector <int> agrDegree(frame.size(), 0);
			for (int k = 0; k < frame.size(); k++) { 
				 for (int t = k+1; t < frame.size(); t++) {
						Vec3b* data = (Vec3b*) frame[k].data;
						Vec3b* data2 = (Vec3b*) frame[t].data;
						if (abs(data[i * frame[0].cols + j][0] - data2[i * frame[0].cols + j][0]) < 10 && abs(data[i * frame[0].cols + j][1] - data2[i * frame[0].cols + j][1]) < 10 && abs(data[i * frame[0].cols + j][2] - data2[i * frame[0].cols + j][2]) < 10) { // DEGREE���� ������ ����
							agrDegree[k]++;
							agrDegree[t]++;
						}
				 }
			}
			int ret = return_Max(agrDegree);
			Vec3b* tmp = (Vec3b*)frame[ret].data;
			desMat[i * frame[0].cols + j] = tmp[i * frame[0].cols + j];
		}
	}

	imwrite("Img.bmp", frame[0]);
	imwrite("Img2.bmp", frame[1]);
	imwrite("result.bmp", des);

}
int return_Max(vector <int> &agrDegree) // ��ġ�� �ִ밪�� ���� ������ ��ȣ ��ȯ
{
	int ret = *max_element(agrDegree.begin(), agrDegree.end()); // ��ġ�� �ִ� ���ϱ�

	for (int i = 0; i < agrDegree.size(); i++) { // �����˻��ϸ� �ð��� ���ϼ� ����
		if (agrDegree[i] == ret)
			return i;
	}
	return 0;
}

void cvDiff(Mat &image, Mat &image2,  Mat &diff) { // ������
	int x, y;
	for (y = 0; y < image.rows; y++)
	{
		for (x = 0; x < image.cols; x++)
		{
			if (image.channels() == 1) // Gray ������ ���
			{
				diff.at<uchar>(y, x) = abs(image.at<uchar>(y, x) - image2.at<uchar>(y, x));
			}
			else if (image.channels() == 3) // RGB ������ ���
			{
				diff.at<Vec3b>(y, x)[0] = abs(image.at<Vec3b>(y, x)[0] - image2.at<Vec3b>(y, x)[0]);
				diff.at<Vec3b>(y, x)[1] = abs(image.at<Vec3b>(y, x)[1] - image2.at<Vec3b>(y, x)[1]);
				diff.at<Vec3b>(y, x)[2] = abs(image.at<Vec3b>(y, x)[2] - image2.at<Vec3b>(y, x)[2]);
			}
		}
	}
}


void changeGray(Mat &Img) // ä�� 3�� ����ȭ �ϱ�
{
	Vec3b* data = (Vec3b*)Img.data;
	for (int i = 0; i < Img.rows; i++) {
		for (int j = 0; j < Img.cols; j++) {
			if (data[i * Img.cols + j][0] == 255 || data[i * Img.cols + j][1] == 255 || data[i * Img.cols + j][2] == 255)  // ������ ������
			{
				data[i * Img.cols + j] = Vec3b(255, 255, 255); // ������� �ٲ�
			}
		}
	}
}
void copy(const Mat &Img, Mat &result, Mat &mask) // copyTo ����
{
	Vec3b* maskData = (Vec3b*)mask.data;
	Vec3b* ImgData = (Vec3b*)Img.data;
	Vec3b* resData = (Vec3b*)result.data;
	for (int i = 0; i < mask.rows; i++) {
		for (int j = 0; j < mask.cols; j++) {
			if (maskData[i * mask.cols + j] == Vec3b(255, 255, 255)) { // ����̶��! ���̱�
				resData[i * mask.cols + j] = ImgData[i * mask.cols + j];
			}
		}
	}
}
/*
void Mat::copyTo(OutputArray _dst, InputArray _mask) const
{
	Mat mask = _mask.getMat();
	if (!mask.data)
	{
		copyTo(_dst);
		return;
	}

	int cn = channels(), mcn = mask.channels();
	CV_Assert(mask.depth() == CV_8U && (mcn == 1 || mcn == cn));
	bool colorMask = mcn > 1;

	size_t esz = colorMask ? elemSize1() : elemSize();
	BinaryFunc copymask = getCopyMaskFunc(esz);

	uchar* data0 = _dst.getMat().data;
	_dst.create(dims, size, type());
	Mat dst = _dst.getMat();

	if (dst.data != data0) // do not leave dst uninitialized
		dst = Scalar(0);

	if (dims <= 2)
	{
		CV_Assert(size() == mask.size());
		Size sz = getContinuousSize(*this, dst, mask, mcn);
		copymask(data, step, mask.data, mask.step, dst.data, dst.step, sz, &esz);
		return;
	}

	const Mat* arrays[] = { this, &dst, &mask, 0 };
	uchar* ptrs[3];
	NAryMatIterator it(arrays, ptrs);
	Size sz((int)(it.size*mcn), 1);

	for (size_t i = 0; i < it.nplanes; i++, ++it)
		copymask(ptrs[0], 0, ptrs[2], 0, ptrs[1], 0, sz, &esz);
}
*/
