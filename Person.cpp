#include "Person.h"



Person::Person()
{

}
Person::Person(Mat &Img, vector<bbox_t> vec)
{
	for (auto &i : vec) {
		add_Person(Img, i);
	}
}

Person::~Person()
{
}
int Person::size() {
	return object.size();
}
void Person::add_Person(Mat &Img, bbox_t vec)
{
	obj_t tmp;
	Mat tmpImg;
	Img.copyTo(tmpImg);
	vec.y = max((int)vec.y - 50, 0);
	vec.h = vec.h + 80;
	if (vec.x + vec.w > Img.cols)
		vec.w = Img.cols - vec.x;
	if (vec.y + vec.h > Img.rows)
		vec.h = Img.rows - vec.y;
	tmp.frame = tmpImg(Rect(vec.x, vec.y, vec.w-1, vec.h-1));
	tmp.vec = vec;
	object.push_back(tmp);

}

obj_t Person::get_Person(int cnt)
{
	return object.at(cnt);
}

// ���࿡ ���������� �װ͵� �ϳ��� ��ü�� ����ϵ����ϴ� �׷��� ��ġ���� �������� �������� vector�����