#pragma once
//#7 quan ly doc/ghi file van ban (Open, Save, Save as)
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

#include "textDoc.h"

class textFile {
private:
	string fileName;   // duong dan file dang lam viec
	bool hasFile;      // da co file gan voi van ban chua?

	bool writeToFile(string path, textDoc& doc);

public:
	textFile();

	bool open(string path, textDoc& doc);     // doc file -> doc
	bool save(textDoc& doc);                  // ghi de len file hien tai
	bool saveAs(string path, textDoc& doc);   // ghi ra file moi

	bool hasFileName();
	string getFileName();
};

textFile::textFile() {
	fileName = "";
	hasFile = false;
}

bool textFile::hasFileName() {
	return hasFile;
}

string textFile::getFileName() {
	return fileName;
}

// ghi toan bo cac dong cua doc ra file path, moi dong ket thuc bang '\n'
bool textFile::writeToFile(string path, textDoc& doc) {
	ofstream fout(path);

	if (!fout.is_open())
		return false;

	for (lineNode* p = doc.getLine(0); p != nullptr; p = p->pNext) {
		fout << p->data << '\n';
	}

	fout.close();
	return !fout.fail();
}

bool textFile::open(string path, textDoc& doc) {
	ifstream fin(path);

	// mo that bai -> chua dong cham den doc, van ban cu van con nguyen
	if (!fin.is_open()) {
		cout << "Khong mo duoc file: " << path << endl;
		return false;
	}

	doc.clear();

	string line;
	int position = 0;

	while (getline(fin, line)) {
		// bo ky tu '\r' thua (file tao tren Windows/Linux khac nhau)
		if (!line.empty() && line.back() == '\r')
			line.pop_back();

		// bo BOM cua UTF-8 o dau file (Notepad hay them vao)
		if (position == 0 && line.size() >= 3 &&
			line.compare(0, 3, "\xEF\xBB\xBF") == 0)
			line.erase(0, 3);

		doc.insertLine(position, line);
		position++;
	}

	fin.close();

	// file rong -> van ban luon phai co it nhat 1 dong
	if (doc.getLineCount() == 0)
		doc.insertLine(0, "");

	fileName = path;
	hasFile = true;
	return true;
}

bool textFile::save(textDoc& doc) {
	if (!hasFile) {
		cout << "Chua co ten file, hay dung Save as\n";
		return false;
	}

	if (!writeToFile(fileName, doc)) {
		cout << "Khong ghi duoc file: " << fileName << endl;
		return false;
	}

	cout << "Da luu file: " << fileName << endl;
	return true;
}

bool textFile::saveAs(string path, textDoc& doc) {
	if (path.empty()) {
		cout << "Ten file rong\n";
		return false;
	}

	if (!writeToFile(path, doc)) {
		cout << "Khong ghi duoc file: " << path << endl;
		return false;
	}

	// luu thanh cong moi doi file hien tai
	fileName = path;
	hasFile = true;
	cout << "Da luu file: " << fileName << endl;
	return true;
}
