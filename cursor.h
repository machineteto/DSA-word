#pragma once
//#3 quan ly con tro va di chuyen con tro trong textdoc

#include "textDoc.h"

using namespace std;

class cursor {
private:
	int line = 0;
	int column = 0;

public:
	//constructor
	//cursor();

	//di chuyen con tro
	void moveUp(textDoc& doc);
	void moveDown(textDoc& doc);
	void moveLeft(textDoc& doc);
	void moveRight(textDoc& doc);

	void setPosition(int l, int c, textDoc& doc);

	//truy xuat vi tri
	int getLine();
	int getColumn();
};

//cursor::cursor() {
//	line = 0;
//	column = 0;
//}

void cursor::moveUp(textDoc& doc) {
	if (line <= 0 || doc.getLineCount() == 0)
		return;
	line--;
	lineNode* p = doc.getLine(line);

	if (column > p->data.length()) {
		column = p->data.length();
	}
}

void cursor::moveDown(textDoc& doc) {
	if (line + 1 >= doc.getLineCount())
		return;
	line++;
	lineNode* p = doc.getLine(line);

	if (column > p->data.length()) {
		column = p->data.length();
	}
}

void cursor::moveLeft(textDoc& doc) {
	if (doc.getLineCount() == 0)
		return;

	if (column > 0) {
		column--;
	}
	else if (line > 0) {
		line--;
		lineNode* p = doc.getLine(line);
		column = p->data.length();
	}
}

void cursor::moveRight(textDoc& doc) {
	if (doc.getLineCount() == 0)
		return;

	lineNode* p = doc.getLine(line);

	if (column < p->data.length())
		column++;
	else if (line + 1 < doc.getLineCount()) {
		line++;
		column = 0;
	}
}

void cursor::setPosition(int l, int c, textDoc& doc) {
	if (doc.getLineCount() == 0) {
		line = 0;
		column = 0;
		return;
	}

	if (l < 0)
		l = 0;

	if (l >= doc.getLineCount())
		l = doc.getLineCount() - 1;

	lineNode* p = doc.getLine(l);

	line = l;
	column = c;

	if (column < 0)
		column = c;

	if (column > p->data.length())
		column = p->data.length();
}

int cursor::getLine() {
	return line;
}

int cursor::getColumn() {
	return column;
}