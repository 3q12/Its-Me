#include"backGround.h"

Detector detector("yolo.cfg", "yolo.weights");
float cutRate = 0.95; // �߶� �׵θ� ����

//int main(int argc, char *argv[])
/* ����� ���ϱ� ���ؼ� ������ �о�� yolo ���� �� ��� ���� ����
*/
backGround::backGround(char *videoFile)
{

	//msgBox.show();
	//detector= detector("yolo.cfg", "yolo.weights");
	//char videoFile[50] = "1.mp4";

	Mat des = sub_Bground(videoFile);
	capture_ROI(des, videoFile, "AA.mp4");
	add_ObjectToRes(des, videoFile);
	//	//	; // break���� �ٲ� Ű�Է¹��������� �������̵�

	//	/*gray("3.mp4");*/
	//	cvDestroyWindow("tt");

	//return 0;
}
// res Mat�� ������� ����� ä���. �� ����ŷ�ϱ�
void backGround::make_Mask(cv::Mat &res, std::vector<bbox_t> const result_vec)
{
	cv::Vec3b* resMat = (cv::Vec3b*)res.data;
	for (auto &i : result_vec) {
		cv::Point pt(i.x, i.y);
		cv::Rect rect(pt, cv::Size(i.w, i.h));
		cv::rectangle(res, rect, cv::Scalar(255, 255, 255), cv::FILLED);
	}
	//	imshow("Mask", res);
}
void backGround::make_Mask(cv::Mat &res, bbox_t const result_vec)
{
	cv::Point pt(result_vec.x, result_vec.y);
	cv::Rect rect(pt, cv::Size(result_vec.w, result_vec.h));
	cv::rectangle(res, rect, cv::Scalar(255, 255, 255), cv::FILLED);

	//imshow("Mask", res);
}
// ��濡 ��ü�� center��ǥ�� �߽����� �ٿ��ִ´�.
cv::Mat backGround::add_object(cv::Mat &background, cv::Mat &object, cv::Point center) {
	// center is object's center location
	cv::Mat src_mask = 255 * cv::Mat::ones(object.rows, object.cols, object.depth());
	cv::Mat result;
	seamlessClone(object, background, src_mask, center, result, cv::NORMAL_CLONE);
	return result;
}
// ��汸�ϱ�
cv::Mat backGround::sub_Bground(char *videoFile)
{
	cv::VideoCapture bgrCapture(videoFile);  // ���� ���� �б�

	std::vector<cv::Mat> src;
	cv::Mat des;


	if (!bgrCapture.isOpened()) {
		//error in opening the video input
		cerr << "Unable to open video file: " << "tt" << endl;
		exit(EXIT_FAILURE);
	}


	cv::Mat Img;
	bgrCapture.read(Img); // ó�������� Img�� ����
	cv::Mat readImg = cv::Mat(Img.rows, Img.cols, CV_32SC3, cv::Scalar(0, 0, 0)); //  ���� �̹����� ������ Mat
	des = cv::Mat(Img.rows, Img.cols, CV_8UC3, cv::Scalar(0, 0, 0)); // ����� ������ Mat

	int check = 0;
	std::vector <cv::Mat> frame;

	while (bgrCapture.read(readImg))
	{
		check++;
		if (check % 4 != 0)
			continue;
		std::vector<bbox_t> result_vec = detector.detect(readImg);
		cv::Mat diff = cv::Mat(Img.rows, Img.cols, CV_8UC3, cv::Scalar(0, 0, 0)); // ��ü�� ������� ä���� ������ Mat
		cv::Mat result = cv::Mat(Img.rows, Img.cols, CV_32SC3, cv::Scalar(0, 0, 0)); // ��ü�� ������ ������ ������ ����

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
	bg = des;
	return des;
}
// ��濡 ����� �׷��ִ´�. des�� ����� ����, 
void backGround::add_ObjectToRes(cv::Mat &des, char *filename) {
	cv::VideoCapture Capture("AA.mp4");  // ���� ���� �б�
	cv::Mat Img, AImg;

	if (!Capture.isOpened()) {
		//error in opening the video input
		cerr << "Unable to open video file: " << "tt" << endl;
		exit(EXIT_FAILURE);
	}
	cv::VideoCapture bgrCapture(filename);  // ���� ���� �б�
	if (!Capture.isOpened()) {
		//error in opening the video input
		cerr << "Unable to open video file: " << "tt" << endl;
		exit(EXIT_FAILURE);
	}
	Capture.read(AImg); // �׵θ��� ������� ä���� ���󿡼� �о ����
	bgrCapture.read(Img); // ���� ���󿡼� ù�������� Img�� ����

	std::vector<bbox_t> first_vec = detector.detect(Img); // ù�������� ��� ��ġ ����
	std::vector <cv::Mat> AAA;

	AAA.push_back(Img);
	for (int i = 0; i < RECURSIVE_COUNT; i++) { //  RECURSIVE_COUNT  = 30
		cv::Mat tmpImg;
		bgrCapture.read(tmpImg);
		AAA.push_back(tmpImg);
	}
	person = Person(AImg, AAA, first_vec); // ������ �� person

	imwrite("result.bmp", des); // �ڸ��� �� ����̹��� ����
	des = des(Rect(des.cols * (1 - cutRate) / 2, des.rows * (1 - cutRate) / 2, des.cols * cutRate, des.rows * cutRate));
	imwrite("resultV2.bmp", des); // �ڸ� �� ��� �̹��� ����

	Capture.release();
	bgrCapture.release();

}
// ��ġ�� ������ �����Ѵ�. des�� ����� ����, frame���� ����� ���� �̹��� vector, start�� end�� ��ġ����  ����� �̹����� ���� ���⼭�� 0���� �ִ븦 ���ϱ������� ��´�. 
void backGround::cal_Degree(cv::Mat &des, std::vector <cv::Mat> &frame, int start, int end)) {  // ���� ������ �����ӵ� , des�� ������ ��ǥ��
	printf("%d %d %d\n", frame.size(), start, end);
	std::vector <cv::Mat> tmp;
	tmp.resize(frame.size());

	for (int i = 0; i < frame.size(); i++)
		frame[i].convertTo(tmp[i], CV_8UC3);

	cv::Vec3b* desMat = (cv::Vec3b*)des.data;
	for (int i = start; i < end; i++) {
		for (int j = 0; j < frame[0].cols; j++) {
			std::vector <int> agrDegree(frame.size(), 0);
			for (int k = 0; k < frame.size(); k++) {
				for (int t = k + 1; t < frame.size(); t++) {
					cv::Vec3i* data = (cv::Vec3i*)frame[k].data;
					cv::Vec3i* data2 = (cv::Vec3i*)frame[t].data;
					if (data[i * frame[0].cols + j] == cv::Vec3i(300, 300, 300)) {
						continue;
					}
					else if (abs(data[i * frame[0].cols + j][0] - data2[i * frame[0].cols + j][0]) < 10 && abs(data[i * frame[0].cols + j][1] - data2[i * frame[0].cols + j][1]) < 10 && abs(data[i * frame[0].cols + j][2] - data2[i * frame[0].cols + j][2]) < 10) { // DEGREE���� ������ ����
						agrDegree[k]++;
						agrDegree[t]++;
					}
				}
			}
			int ret = return_Max(agrDegree);
			cv::Vec3b* tmpData = (cv::Vec3b*)tmp[ret].data;
			desMat[i * frame[0].cols + j] = tmpData[i * frame[0].cols + j];
		}
	}
}
// 300���� ä���� ���� �̹����� �󸶳� �ִ��� ���
void backGround::check_Mat(cv::Mat &mat)
{
	cv::Vec3b* matData = (cv::Vec3b*)mat.data;
	int k = 0;
	for (int i = 0; i < mat.rows; i++) {
		for (int j = 0; j < mat.cols; j++) {
			if (matData[i * mat.cols + j] == cv::Vec3b(300, 300, 300)) {
				k++;
			}

		}
	}
	printf("k : %d\n", k);
}
// ��ġ�� �ִ밪�� ���� ������ ��ȣ ��ȯ
int backGround::return_Max(std::vector <int> &agrDegree)
{
	int ret = *max_element(agrDegree.begin(), agrDegree.end()); // ��ġ�� �ִ� ���ϱ�

	for (int i = 0; i < agrDegree.size(); i++) { // �����˻��ϸ� �ð��� ���ϼ� ����
		if (agrDegree[i] == ret)
			return i;
	}
	return 0;
}

// ä�� 3���� �̹����� ����ȭ ��Ų��.
void backGround::changeGray(cv::Mat &Img)
{
	cv::Vec3b* data = (cv::Vec3b*)Img.data;
	for (int i = 0; i < Img.rows; i++) {
		for (int j = 0; j < Img.cols; j++) {
			if (data[i * Img.cols + j][0] == 255 || data[i * Img.cols + j][1] == 255 || data[i * Img.cols + j][2] == 255)  // ������ ������
			{
				data[i * Img.cols + j] = cv::Vec3b(255, 255, 255); // ������� �ٲ�
			}
		}
	}
}
//copyTo ���� mask��ġ�� Img���� ������ result�� ���δ�. �׸��� mask�� ���� �κ��� 300���� ä���.
void backGround::copyMask(cv::Mat &Img, cv::Mat &result, cv::Mat &mask) // copyTo ����
{
	Img.convertTo(Img, CV_32SC3);
	cv::Vec3b* maskData = (cv::Vec3b*)mask.data;
	cv::Vec3i* ImgData = (cv::Vec3i*)Img.data;
	cv::Vec3i* resData = (cv::Vec3i*)result.data;

	for (int i = 0; i < mask.rows; i++) {
		for (int j = 0; j < mask.cols; j++) {
			if (maskData[i * mask.cols + j] != cv::Vec3b(255, 255, 255)) { // ����̶��! ���̱�
				resData[i * mask.cols + j] = ImgData[i * mask.cols + j];
			}
			else { // ������ �κ��� 300���� ä���
				resData[i * mask.cols + j] = cv::Vec3i(300, 300, 300);
			}
		}
	}
}
/* ������ �׵θ��� �߶� ������� ä�� �� savefile�� ����
des�� ����̹���, videofile�� �������� savefile�� �׵θ��� �ڸ��� ����� ���� �̸� */
void backGround::capture_ROI(cv::Mat &des, char *videoFile, char *saveFile)
{
	cv::VideoCapture capture(videoFile);  // ���� ���� �б�
	cv::VideoWriter vw;

	vw = cv::VideoWriter(saveFile, CV_FOURCC('D', 'I', 'V', 'X'), capture.get(CV_CAP_PROP_FPS), cv::Size((int)capture.get(CV_CAP_PROP_FRAME_WIDTH), (int)capture.get(CV_CAP_PROP_FRAME_HEIGHT)), true);

	cv::Mat Img = cv::Mat(capture.get(CV_CAP_PROP_FRAME_HEIGHT), capture.get(CV_CAP_PROP_FRAME_WIDTH), CV_8UC3, cv::Scalar(0, 0, 0));
	cv::Mat desMask = cv::Mat(Img.rows, Img.cols, CV_8UC3, cv::Scalar(0, 0, 0));
	cv::Mat resMask = cv::Mat(Img.rows, Img.cols, CV_8UC3, cv::Scalar(0, 0, 0));
	cv::Mat res = cv::Mat(Img.rows, Img.cols, CV_8UC3, cv::Scalar(0, 0, 0));


	cv::Point pt(Img.cols * (1 - cutRate) / 2, Img.rows * (1 - cutRate) / 2);
	cv::Rect rect(pt, cv::Size(Img.cols * cutRate, Img.rows * cutRate));
	cv::rectangle(desMask, rect, cv::Scalar(255, 255, 255), cv::FILLED);
	cv::rectangle(resMask, rect, cv::Scalar(255, 255, 255), cv::FILLED);
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