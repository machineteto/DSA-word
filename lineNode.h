#pragma once
//#1 luu noi dung tung dong
#include <string>

using namespace std;

class lineNode {
public:
	string data;
	lineNode* pPrev;
	lineNode* pNext;

	lineNode(string text = "") {
		data = text;
		pPrev = nullptr;
		pNext = nullptr;
	}
};