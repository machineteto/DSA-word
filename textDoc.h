#pragma once
//#2 quan ly danh sach lien ket kep
#include <iostream>
#include <string>
#include"lineNode.h"

using namespace std;

class textDoc {
private:
	lineNode* pHead;
	lineNode* pTail;
	int lineCount;
public:
	//textDoc(const textDoc&) = delete;
	//textDoc& operator=(const textDoc&) = delete;

	//cons va des
	textDoc();
	~textDoc();

	//quan ly ds
	bool isEmpty();
	int getLineCount();

	//truy xuat node
	lineNode* getLine(int position);

	//them dong va xoa dong
	void insertLine(int position, string text);
	void deleteLine(int position);

	//hien thi va cai dat des
	void display();
	void clear();


};

//contructor va destructor
textDoc::textDoc() {
	pHead = nullptr;
	pTail = nullptr;
	lineCount = 0;
}
textDoc::~textDoc() {
	clear();
}

//quan ly ds
bool textDoc::isEmpty() {
	return pHead == nullptr;
}
int textDoc::getLineCount() {
	return lineCount;
}

//truy xuat node
lineNode* textDoc::getLine(int position) {
	if (position < 0 || position >= lineCount)
		return nullptr;

	lineNode* p;

	if (position < lineCount / 2) {
		p = pHead;
		for (int i = 0; i < position; i++) {
			p = p->pNext;
		}
	}
	else {
		p = pTail;
		for (int i = lineCount - 1; i > position; i--) {
			p = p->pPrev;
		}
	}
	return p;
}

//thhem dong
void textDoc::insertLine(int position, string text) {
	if (position < 0 || position > lineCount) {
		return;
	}
	lineNode* pNew = new lineNode(text);
	//neu ds chi co 1 node
	//hoac dang xu ly ds rong
	if (pHead == nullptr) {
		pHead = pNew;
		pTail = pNew;
	}
	//them dau
	else if (position == 0) {
		pNew->pNext = pHead;
		pHead->pPrev = pNew;
		pHead = pNew;
	}
	//them cuoi
	else if (position == lineCount) {
		pTail->pNext = pNew;
		pNew->pPrev = pTail;
		pTail = pNew;
	}
	//them giua
	else {
		lineNode* p = getLine(position);
		pNew->pNext = p;
		pNew->pPrev = p->pPrev;
		p->pPrev->pNext = pNew;
		p->pPrev = pNew;
	}
	lineCount++;
}
//xoa dong
void textDoc::deleteLine(int position) {
	if (position < 0 || position >= lineCount) {
		return;
	}
	//neu chi co mot dong
	if (lineCount == 1) {
		pHead->data = "";
		return;
	}

	lineNode* p = getLine(position);

	//xoa dau
	if (p == pHead) {
		pHead = pHead->pNext;
		pHead->pPrev = nullptr;
	}
	//xoa cuoi
	else if (p == pTail) {
		pTail = pTail->pPrev;
		pTail->pNext = nullptr;
	}
	//xoa giua
	else {
		p->pPrev->pNext = p->pNext;
		p->pNext->pPrev = p->pPrev;
	}
	delete p;
	lineCount--;
}



//hien thi
void textDoc::display() {
	lineNode* p = pHead;
	int index = 1;
	while (p != nullptr) {
		cout << index << " | " << p->data << endl;
		p = p->pNext;
		index++;
	}
}
void textDoc::clear() {
	lineNode* p = pHead;
	while (p != nullptr) {
		lineNode* temp = p;
		p = p->pNext;
		delete temp;
	}
	pHead = nullptr;
	pTail = nullptr;
	lineCount = 0;
}