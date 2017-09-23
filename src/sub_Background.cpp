#include "sub_Background.h"

Detector detector("./data/yolo.cfg", "./data/yolo.weights");
float cutRate = 0.95; // �߶� �׵θ� ����


 /* ����� ���ϱ� ���ؼ� ������ �о�� yolo ���� �� ��� ���� ���� */
sub_Background::sub_Background(char *videoFile)
{
	cv::Mat des = cal_Background(videoFile);
	capture_ROI(des, videoFile, "AA.mp4");
	add_ObjectToRes(des, videoFile);
}
// res Mat�� ������� ����� ä���. �� ����ŷ�ϱ�
void sub_Background::make_Mask(cv::Mat &res, std::vector<bbox_t> const result_vec)
{
	cv::Vec3b* resMat = (cv::Vec3b*)res.data;
	for (auto &i : result_vec) {
		cv::Point pt(i.x, i.y);
		cv::Rect rect(pt, cv::Size(i.w, i.h));
		cv::rectangle(res, rect, cv::Scalar(255, 255, 255), cv::FILLED);
	}
	//	imshow("Mask", res);
}
void sub_Background::make_Mask(cv::Mat &res, bbox_t const result_vec)
{
	cv::Point pt(result_vec.x, result_vec.y);
	cv::Rect rect(pt, cv::Size(result_vec.w, result_vec.h));
	cv::rectangle(res, rect, cv::Scalar(255, 255, 255), cv::FILLED);
}
void sub_Background::save_ROI(cv::Mat &Img, const std::vector <bbox_t> vec)
{
	for (auto &i : vec) {
		cv::Point pt(i.x, i.y);
		cv::rectangle(Img, pt, cv::Point(i.x + i.w, i.y + i.h), cv::Scalar(0, 0, 255), 3);
	}
}
// ��濡 ��ü�� center��ǥ�� �߽����� �ٿ��ִ´�.
cv::Mat sub_Background::add_object(cv::Mat &sub_Background, cv::Mat &object, cv::Point center) {
	// center is object's center location
	cv::Mat src_mask = 255 * cv::Mat::ones(object.rows, object.cols, object.depth());
	cv::Mat result;
	seamlessClone(object, sub_Background, src_mask, center, result, cv::NORMAL_CLONE);
	return result;
}
// ��汸�ϱ�
cv::Mat sub_Background::cal_Background(char *videoFile)
{
	cv::VideoCapture bgrCapture(videoFile);  // ���� ���� �б�
#ifdef SAVE_ROI_VIDEO
	cv::VideoWriter vw;
	vw = cv::VideoWriter("ROIvideo.mp4", CV_FOURCC('D', 'I', 'V', 'X'), 5, cv::Size((int)bgrCapture.get(CV_CAP_PROP_FRAME_WIDTH), (int)bgrCapture.get(CV_CAP_PROP_FRAME_HEIGHT)), true);
#endif

	std::vector<cv::Mat> src;
	cv::Mat des;


	if (!bgrCapture.isOpened()) {
		//error in opening the video input
		cerr << "Unable to open video file: " << "tt" << endl;
		exit(EXIT_FAILURE);
	}


	cv::Mat Img;
	bgrCapture.read(Img); // ù�������� Img�� ����
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
#ifdef SAVE_ROI_VIDEO
		cv::Mat tmp = readImg.clone();
#endif
		for (auto vec : result_vec) {
			vec.x = max((int)vec.x - 20, 0);
			vec.y = max((int)vec.y - 50, 0);
			vec.h = vec.h + 100;
			vec.w = vec.w + 50;
		}
#ifdef SAVE_ROI_VIDEO
		save_ROI(tmp, result_vec);
		vw.write(tmp);
#endif

		make_Mask(diff, result_vec); // diff�� ��ü�� ������� ä���ִ´�.
		copy_MaskToImg(readImg, result, diff); // ��ü ������ ������ �ٿ��ִ´�.
		
		frame.push_back(result); // frame vector�� result�� ������.
								 //if (cvWaitKey() == 27) continue; // break���� �ٲ� Ű�Է¹��������� �������̵�

	}
#ifdef SAVE_ROI_VIDEO
	vw.release();
#endif

	int size = frame[0].rows / 4; // thread ���� ����ð��� ���̱����� �л����� ����
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
	imwrite("bground.bmp", des);
	bg = des;
	big_backImg = des(cv::Rect(des.cols * (1 - cutRate) / 2, des.rows * (1 - cutRate) / 2, des.cols * cutRate, des.rows * cutRate));
	return des;
}
// ��濡 ����� �׷��ִ´�. des�� ����� ����, 
void sub_Background::add_ObjectToRes(cv::Mat &des, char *filename) {
	cv::VideoCapture Capture("AA.mp4");  // ���� ���� �б�
	cv::Mat first_Img, back_Img;

	if (!Capture.isOpened()) {
		//error in opening the video input
		cerr << "Unable to open video file: " << filename << endl;
		exit(EXIT_FAILURE);
	}
	cv::VideoCapture bgrCapture(filename);  // ���� ���� �б�
	if (!bgrCapture.isOpened()) {
		//error in opening the video input
		cerr << "Unable to open video file: " << filename << endl;
		exit(EXIT_FAILURE);
	}
	Capture.read(back_Img); // �׵θ��� ������� ä���� ���󿡼� �о ����
	bgrCapture.read(first_Img); // ���� ���󿡼� ù�������� Img�� ����S

	std::vector<bbox_t> first_vec = detector.detect(first_Img); // ù�������� ��� ��ġ ����
#ifdef DETECT_EMOTION
	std::vector <cv::Mat> emotion_ImgSet; // ���������� ���� �̹��� ����

	emotion_ImgSet.push_back(first_Img);
	for (int i = 0; i < RECURSIVE_COUNT; i++) { //  RECURSIVE_COUNT  = 30
		cv::Mat tmpImg;
		bgrCapture.read(tmpImg);
		emotion_ImgSet.push_back(tmpImg);
	}
	person_set = PersonSet(back_Img, emotion_ImgSet, first_vec); //  emotion detect person
#else
	person_set = PersonSet(back_Img, first_vec); // NOT detect_emotion 
#endif

	des = des(cv::Rect(des.cols * (1 - cutRate) / 2, des.rows * (1 - cutRate) / 2, des.cols * cutRate, des.rows * cutRate));
	imwrite("sub_Background.bmp", des); // �ڸ� �� ��� �̹��� ����

	Capture.release();
	bgrCapture.release();

}
// ��ġ�� ������ �����Ѵ�. des�� ����� ����, frame���� ����� ���� �̹��� vector, start�� end�� ��ġ����  ����� �̹����� ���� ���⼭�� 0���� �ִ븦 ���ϱ������� ��´�. 
void sub_Background::cal_Degree(cv::Mat &des, std::vector <cv::Mat> &frame, int start, int end)
{  // ���� ������ �����ӵ� , des�� ������ ��ǥ��


	printf("%d %d %d\n", frame.size(), start, end);
	std::vector <cv::Mat> tmp; // ������ ���� �ӽ� Mat vector
	tmp.resize(frame.size());

	for (int i = 0; i < frame.size(); i++)
		frame[i].convertTo(tmp[i], CV_8UC3);

	cv::Vec3b* desMat = (cv::Vec3b*)des.data; // 3ä�� byteũ���� VecŬ���� type���� �̹����� data ����� ����Ŵ
	for (int i = start; i < end; i++) {
		for (int j = 0; j < frame[0].cols; j++) {
			std::vector <int> agrDegree(frame.size(), 0);
			for (int k = 0; k < frame.size(); k++) {
				for (int t = k + 1; t < frame.size(); t++) {
					cv::Vec3i* data = (cv::Vec3i*)frame[k].data; // 255�̻���(= byte����) ���� ���ϱ� ���� integer type���� Vec����
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
void sub_Background::check_Mat(cv::Mat &mat)
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
	printf("300 data counts : %d\n", k);
}
// ��ġ�� �ִ밪�� ���� ������ ��ȣ ��ȯ
int sub_Background::return_Max(std::vector <int> &agrDegree)
{
	int ret = *max_element(agrDegree.begin(), agrDegree.end()); // ��ġ�� �ִ� ���ϱ�

	for (int i = 0; i < agrDegree.size(); i++) { // �����˻��ϸ� �ð��� ���ϼ� ����
		if (agrDegree[i] == ret)
			return i;
	}
	return 0;
}

// ä�� 3���� �̹����� ����ȭ ��Ų��.
void sub_Background::change_ColorToGray(cv::Mat &Img)
{
	cv::Vec3b* data = (cv::Vec3b*)Img.data;
	for (int i = 0; i < Img.rows; i++) {
		for (int j = 0; j < Img.cols; j++) {
			if (data[i * Img.cols + j][0] == 255 || data[i * Img.cols + j][1] == 255 || data[i * Img.cols + j][2] == 255)  // ������ ������
			{
				data[i * Img.cols + j] = cv::Vec3b(255, 255, 255); // ������� �ٲ��ֱ�
			}
		}
	}
}
//copyTo ���� mask��ġ�� Img���� ������ result�� ���δ�. �׸��� mask�� ���� �κ��� 300���� ä���.
void sub_Background::copy_MaskToImg(cv::Mat &Img, cv::Mat &result, cv::Mat &mask) // copyTo ����
{
	Img.convertTo(Img, CV_32SC3);
	cv::Vec3b* maskData = (cv::Vec3b*)mask.data; 
	cv::Vec3i* ImgData = (cv::Vec3i*)Img.data; // mask �κ��� 255�̻��� ���� ä���ֵ��� �ϱ����� integer type���� �����Ѵ�.
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
void sub_Background::capture_ROI(cv::Mat &des, char *videoFile, char *saveFile)
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