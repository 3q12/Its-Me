#include"backGround.h"

Detector detector("yolo.cfg", "yolo.weights");
float cutRate = 0.95; // �߶� �׵θ� ����

//int main(int argc, char *argv[])
/* ����� ���ϱ� ���ؼ� ������ �о�� yolo ���� �� ��� ���� ����
*/
backGround::backGround()
{
	
	//msgBox.show();
	//detector= detector("yolo.cfg", "yolo.weights");
	char videoFile[50] = "1.mp4";
	
	Mat des = sub_Bground(videoFile);
	capture_ROI(des, videoFile, "AA.mp4");
	add_ObjectToRes(des, videoFile);
	//	//	; // break���� �ٲ� Ű�Է¹��������� �������̵�

	//	/*gray("3.mp4");*/
	//	cvDestroyWindow("tt");

	//return 0;
}
// res Mat�� ������� ����� ä���. �� ����ŷ�ϱ�
void backGround::make_Mask(Mat &res, vector<bbox_t> const result_vec)
{
	Vec3b* resMat = (Vec3b*)res.data;
	for (auto &i : result_vec) {
		Point pt(i.x, i.y);
		Rect rect(pt, Size(i.w, i.h));
		rectangle(res, rect, Scalar(255, 255, 255), FILLED);
	}
	//	imshow("Mask", res);
}
void backGround::make_Mask(Mat &res, bbox_t const result_vec)
{
	Point pt(result_vec.x, result_vec.y);
	Rect rect(pt, Size(result_vec.w, result_vec.h));
	rectangle(res, rect, Scalar(255, 255, 255), FILLED);

	//imshow("Mask", res);
}
// ��濡 ��ü�� center��ǥ�� �߽����� �ٿ��ִ´�.
Mat backGround::add_object(Mat &background, Mat &object, Point center) {
	// center is object's center location
	Mat src_mask = 255 * Mat::ones(object.rows, object.cols, object.depth());
	Mat result;
	seamlessClone(object, background, src_mask, center, result, NORMAL_CLONE);
	return result;
}
// ��汸�ϱ�
Mat backGround::sub_Bground(char *videoFile)
{
	
	VideoCapture bgrCapture(videoFile);  // ���� ���� �б�

	vector<Mat> src;
	Mat des;


	if (!bgrCapture.isOpened()) {
		//error in opening the video input
		cerr << "Unable to open video file: " << "tt" << endl;
		exit(EXIT_FAILURE);
	}


	Mat Img; // ù������ ������ Mat
	bgrCapture.read(Img); // ù�������� Img�� ����
	Mat readImg = Mat(Img.rows, Img.cols, CV_32SC3, Scalar(0, 0, 0)); //  ���� �̹����� ������ Mat
	des = Mat(Img.rows, Img.cols, CV_8UC3, Scalar(0, 0, 0)); // ����� ������ Mat

	int check = 0;
	vector <Mat> frame;

	while (bgrCapture.read(readImg))
	{
		check++;
		if (check % 4 != 0)
			continue;
		vector<bbox_t> result_vec = detector.detect(readImg);
		Mat diff = Mat(Img.rows, Img.cols, CV_8UC3, Scalar(0, 0, 0)); // ��ü�� ������� ä���� ������ Mat
		Mat result = Mat(Img.rows, Img.cols, CV_32SC3, Scalar(0, 0, 0)); // ��ü�� ������ ������ ������ ����

		//cvtColor(tmp, tmp, CV_BGR2GRAY);

		//absdiff(Img, readImg, diff); // ù �����Ӱ� �� �����Ӹ����� �������� diff�� ����
		//threshold(diff, tmp, 20, 255, CV_THRESH_BINARY); // �������� ����ȭ ���Ѽ� tmp�� ����
		//changeGray(tmp); // ����ȭ ��Ų tmp�� �Ϻ��ϰ� ����ȭ
		//pMOG2->apply(tmp, diff); // ����ȭ ��Ų ���� ������ MOG2
		//erode(diff, diff, Mat(), Point(-1, -1), 3);
		//blur(diff, diff, Size(5, 5)); // noise ����
		//diff = ~diff;
		//readImg.copyTo(result, diff); // ��ü ������ ������ �ٿ��ִ´�.

		make_Mask(diff, result_vec); // diff�� ��ü�� ������� ä���ִ´�.
		copyMask(readImg, result, diff); // ��ü ������ ������ �ٿ��ִ´�.

		frame.push_back(result); // frame vector�� result�� ������.
		//if (cvWaitKey() == 27) continue; // break���� �ٲ� Ű�Է¹��������� �������̵�

	}
	int size = frame[0].rows / 4; // thread ����
	thread t1(&cal_Degree, des, frame, 0, size);
	thread t2(&cal_Degree, des, frame, size, size * 2);
	thread t3(&cal_Degree, des, frame, size * 2, size * 3);
	thread t4(&cal_Degree, des, frame, size * 3, size * 4);
	t1.join();
	t2.join();
	t3.join();
	t4.join();

	check_Mat(des); // ��濡 300���� ä���� ���� ��� �� ����� �� ���Ѻκ�

	bgrCapture.release();
	//imshow("des", des);
	imwrite("bground.bmp", des);
	bg=des;
	return des;
}
// ��濡 ����� �׷��ִ´�. des�� ����� ����, 
void backGround::add_ObjectToRes(Mat &des, char *filename) { 
	VideoCapture Capture("AA.mp4");  // ���� ���� �б�
	Mat Img, AImg;
	if (!Capture.isOpened()) {
		//error in opening the video input
		cerr << "Unable to open video file: " << "tt" << endl;
		exit(EXIT_FAILURE);
	}
	VideoCapture bgrCapture(filename);  // ���� ���� �б�
	if (!Capture.isOpened()) {
		//error in opening the video input
		cerr << "Unable to open video file: " << "tt" << endl;
		exit(EXIT_FAILURE);
	}
	Capture.read(AImg); // �׵θ��� ������� ä���� ���󿡼� �о ����
	bgrCapture.read(Img); // ���� ���󿡼� ù�������� Img�� ����

	vector<bbox_t> first_vec = detector.detect(Img); // ù�������� ��� ��ġ ����

	person=Person(AImg, first_vec); // ù�������� ��� ��������
	//imshow("des", des);
	//person.get_Person(0).frame;

	for (int k = 0; k < person.size(); k++) {
		int c;
		vector<obj_t> group = person.get_Group(k);

		// �׷��� ��Ƴ��� ��������� �̹���
		Mat groupImg = Mat(AImg.rows, AImg.cols, CV_8UC3, Scalar(0, 0, 0));
		Mat tmp = Mat(AImg.rows, AImg.cols, CV_8UC3, Scalar(0, 0, 0)); // ����� �ִ� �簢���� ������� ����ŷ �̹����� ����ȴ�.
		Rect A = Rect(group[0].vec.x, group[0].vec.y, group[0].vec.w - 1, group[0].vec.h - 1); // ù��° ����� �簢�̹����� �����´�.
		Rect B;
		for (int j = 0; j < group.size(); j++) {
			B = Rect(group[j].vec.x, group[j].vec.y, group[j].vec.w - 1, group[j].vec.h - 1);
			A = A | B; // �簢���� or�����Ѵ�.
			rectangle(tmp, B, Scalar(255, 255, 255), FILLED); 
			AImg.copyTo(groupImg, tmp); // �簢�� ������ �̹����� �����ͼ� groupImg�� ����
		}
		groupImg = groupImg(Rect(A));
		//imshow("group", groupImg);
		/*while (c = cvWaitKey()) {
			if (c == 27 || c == 'q')
				break;
			else {
				for (int i = 0; i < group.size(); i++) // �׷��� ��� �̹��� ���� �ֱ�
					des = add_object(des, group[i].frame, Point(group[i].vec.x + group[i].vec.w / 2, group[i].vec.y + group[i].vec.h / 2));

				imshow("des", des);
				break;
			}
		}*/


	}
	//if (cvWaitKey() == 27);

	imwrite("result.bmp", des); // �ڸ��� �� ����̹��� ����
	des = des(Rect(des.cols * (1 - cutRate) / 2, des.rows * (1 - cutRate) / 2, des.cols * cutRate, des.rows * cutRate));
	imwrite("resultV2.bmp", des); // �ڸ� �� ��� �̹��� ����

	Capture.release();
	bgrCapture.release();

}
// ��ġ�� ������ �����Ѵ�. des�� ����� ����, frame���� ����� ���� �̹��� vector, start�� end�� ��ġ����  ����� �̹����� ���� ���⼭�� 0���� �ִ븦 ���ϱ������� ��´�. 
void backGround::cal_Degree(Mat &des, vector <Mat> &frame, int start, int end) {  // ���� ������ �����ӵ� , des�� ������ ��ǥ��
	printf("%d %d %d\n", frame.size(), start, end);
	vector <Mat> tmp;
	tmp.resize(frame.size());

	for (int i = 0; i < frame.size(); i++)
		frame[i].convertTo(tmp[i], CV_8UC3);

	Vec3b* desMat = (Vec3b*)des.data;
	for (int i = start; i < end; i++) {
		for (int j = 0; j < frame[0].cols; j++) {
			vector <int> agrDegree(frame.size(), 0);
			for (int k = 0; k < frame.size(); k++) {
				for (int t = k + 1; t < frame.size(); t++) {
					Vec3i* data = (Vec3i*)frame[k].data;
					Vec3i* data2 = (Vec3i*)frame[t].data;
					if (data[i * frame[0].cols + j] == Vec3i(300, 300, 300)) {
						continue;
					}
					else if (abs(data[i * frame[0].cols + j][0] - data2[i * frame[0].cols + j][0]) < 10 && abs(data[i * frame[0].cols + j][1] - data2[i * frame[0].cols + j][1]) < 10 && abs(data[i * frame[0].cols + j][2] - data2[i * frame[0].cols + j][2]) < 10) { // DEGREE���� ������ ����
						agrDegree[k]++;
						agrDegree[t]++;
					}
				}
			}
			int ret = return_Max(agrDegree);
			Vec3b* tmpData = (Vec3b*)tmp[ret].data;
			desMat[i * frame[0].cols + j] = tmpData[i * frame[0].cols + j];
		}
	}
	//imwrite("result.bmp", des);
}
// 300���� ä���� ���� �̹����� �󸶳� �ִ��� ���
void backGround::check_Mat(Mat &mat)
{
	Vec3b* matData = (Vec3b*)mat.data;
	int k = 0;
	for (int i = 0; i < mat.rows; i++) {
		for (int j = 0; j < mat.cols; j++) {
			if (matData[i * mat.cols + j] == Vec3b(300, 300, 300)) {
				k++;
			}

		}
	}
	printf("k : %d\n", k);
}
// ��ġ�� �ִ밪�� ���� ������ ��ȣ ��ȯ
int backGround::return_Max(vector <int> &agrDegree) 
{
	int ret = *max_element(agrDegree.begin(), agrDegree.end()); // ��ġ�� �ִ� ���ϱ�

	for (int i = 0; i < agrDegree.size(); i++) { // �����˻��ϸ� �ð��� ���ϼ� ����
		if (agrDegree[i] == ret)
			return i;
	}
	return 0;
}

// ä�� 3���� �̹����� ����ȭ ��Ų��.
void backGround::changeGray(Mat &Img) 
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
//copyTo ���� mask��ġ�� Img���� ������ result�� ���δ�. �׸��� mask�� ���� �κ��� 300���� ä���.
void backGround::copyMask(Mat &Img, Mat &result, Mat &mask) // copyTo ����
{
	Img.convertTo(Img, CV_32SC3);

	//uchar* maskData = (uchar*)mask.data;
	Vec3b* maskData = (Vec3b*)mask.data;
	Vec3i* ImgData = (Vec3i*)Img.data;
	Vec3i* resData = (Vec3i*)result.data;

	//for (int i = 0; i < mask.rows; i++) {
	//	for (int j = 0; j < mask.cols; j++) {
	//		if (maskData[i * mask.cols + j] != uchar(128)) { // ����̶��! ���̱�
	//			resData[i * mask.cols + j] = ImgData[i * mask.cols + j];
	//		}
	//		else { // ������ �κ��� 300���� ä���
	//			resData[i * mask.cols + j] = Vec3i(300, 300, 300);
	//		}
	//	}
	//}
	for (int i = 0; i < mask.rows; i++) {
		for (int j = 0; j < mask.cols; j++) {
			if (maskData[i * mask.cols + j] != Vec3b(255, 255, 255)) { // ����̶��! ���̱�
				resData[i * mask.cols + j] = ImgData[i * mask.cols + j];
			}
			else { // ������ �κ��� 300���� ä���
				resData[i * mask.cols + j] = Vec3i(300, 300, 300);
			}
		}
	}
}
/* ������ �׵θ��� �߶� ������� ä�� �� savefile�� ����
des�� ����̹���, videofile�� �������� savefile�� �׵θ��� �ڸ��� ����� ���� �̸� */
void backGround::capture_ROI(Mat &des, char *videoFile, char *saveFile)
{
	VideoCapture capture(videoFile);  // ���� ���� �б�
	VideoWriter vw;

	vw = VideoWriter(saveFile, CV_FOURCC('D', 'I', 'V', 'X'), capture.get(CV_CAP_PROP_FPS), Size((int)capture.get(CV_CAP_PROP_FRAME_WIDTH), (int)capture.get(CV_CAP_PROP_FRAME_HEIGHT)), true);

	Mat Img = Mat(capture.get(CV_CAP_PROP_FRAME_HEIGHT), capture.get(CV_CAP_PROP_FRAME_WIDTH), CV_8UC3, Scalar(0, 0, 0));
	Mat desMask = Mat(Img.rows, Img.cols, CV_8UC3, Scalar(0, 0, 0));
	Mat resMask = Mat(Img.rows, Img.cols, CV_8UC3, Scalar(0, 0, 0));
	Mat res = Mat(Img.rows, Img.cols, CV_8UC3, Scalar(0, 0, 0));


	Point pt(Img.cols * (1 - cutRate) / 2, Img.rows * (1 - cutRate) / 2);
	Rect rect(pt, Size(Img.cols * cutRate, Img.rows * cutRate));
	rectangle(desMask, rect, Scalar(255, 255, 255), FILLED);
	rectangle(resMask, rect, Scalar(255, 255, 255), FILLED);
	desMask = ~desMask;

	if (!capture.isOpened()) {
		//error in opening the video input
		cerr << "Unable to open video file: " << "tt" << endl;
		exit(EXIT_FAILURE);
	}
	if (!vw.isOpened())
	{
		cout << "�������� �����ϱ� ���� �ʱ�ȭ �۾� �� ���� �߻�" << endl;
		exit(1);
	}
	while (capture.read(Img)) {
		des.copyTo(res, desMask);
		Img.copyTo(res, resMask);
		vw.write(res);
	}
	vw.release();
}