#pragma once
//#6 
#include "textDoc.h"
#include "cursor.h"
#include "actionStack.h"
#include "textFile.h"

class textEdit {
private:
	textDoc doc;
	cursor cur;
	actionStack undoStack;
	actionStack redoStack;

	string clipBoard;
	bool hasClipBoard;

	void insertLinesAt(int position, string text, bool reuseFirst);
	void deleteLinesAt(int position, int count);
	int countLinesInText(string text);

	void undoOne(action& a);   // MOI: hoan tac dung 1 action
	void redoOne(action& a);   // MOI: lam lai dung 1 action

	int countOccurrences(string text, string keyword);

	textFile file;
	bool modified; //văn bản có thay đổi chưa lưu

	

	
public:
	//constructor
	textEdit();

	//them xoa ki tu
	void insertChar(char ch);
	void deleteChar();

	//them xoa hang
	void insertLine(string text);
	void deleteLine();

	//coppy, cat va dan dong
	void copyLines(int start, int end);
	void cutLines(int start, int end);
	void pasteLines();

	//undo redo
	void undo();
	void redo();
	
	void search(string keyword);
	void replace(string oldText, string newText);

	void showStatistics();
	void run();

	//move con tro
	void moveUp();
	void moveDown();
	void moveLeft();
	void moveRight();

	//hien thi con tro
	void showCursorPosition();

	//hien thi
	void display();

	//Textfile
	bool openFile(string path, bool force = false);
	bool saveFile();
	bool saveFileAs(string path);

	
};

void textEdit::insertLinesAt(int position, string text, bool reuseFirst) {
	int start = 0;
	int lineCount = countLinesInText(text);

	for (int i = 0; i < lineCount; i++) {
		size_t pos = text.find('\n', start);
		string lineText = (pos == string::npos) ? text.substr(start)
			: text.substr(start, pos - start);

		if (i == 0 && reuseFirst)
			doc.getLine(position)->data = lineText;
		else
			doc.insertLine(position, lineText);

		position++;
		if (pos != string::npos) start = pos + 1;
	}
}
void textEdit::deleteLinesAt(int position, int count) {
	for (int i = 0; i < count; i++) {
		if (position >= doc.getLineCount())
			break;

		doc.deleteLine(position);
	}
}

int textEdit::countLinesInText(string text)
{
	int count = 1;

	for (int i = 0; i < text.length(); i++) {
		if (text[i] == '\n')
			count++;
	}

	return count;
}

textEdit::textEdit() {
	if (doc.getLineCount() == 0) {
		doc.insertLine(0, "");
	}

	clipBoard = "";
	hasClipBoard = false;

	modified = false;

}

void textEdit::insertChar(char ch) {
	lineNode* p = doc.getLine(cur.getLine());
	
	if (p == nullptr)
		return;

	int l = cur.getLine();
	int c = cur.getColumn();

	if (c < 0 || c > p->data.length())
		return;

	p->data.insert(c, 1, ch);

	undoStack.push(action(INSERT_CHAR, l, c, string(1, ch)));

	redoStack.clear();
	modified = true;
	cur.moveRight(doc);
}

void textEdit::deleteChar() {
	lineNode* p = doc.getLine(cur.getLine());

	if (p == nullptr || cur.getColumn() >= p->data.length())
		return;

	int l = cur.getLine();
	int c = cur.getColumn();

	string deleted = p->data.substr(c, 1);

	p->data.erase(c, 1);
	undoStack.push(action(DELETE_CHAR, l, c, deleted));
	redoStack.clear();
	modified = true;
}

void textEdit::undoOne(action& a) {
	lineNode* p = nullptr;

	switch (a.type) {
	case INSERT_CHAR:
		p = doc.getLine(a.line);
		if (p != nullptr && a.column < (int)p->data.length())
			p->data.erase(a.column, a.content.length());
		cur.setPosition(a.line, a.column, doc);      // MOI: dua con tro ve cho cu
		break;

	case DELETE_CHAR:
		p = doc.getLine(a.line);
		if (p != nullptr)
			p->data.insert(a.column, a.content);
		cur.setPosition(a.line, a.column, doc);
		break;

	case INSERT_LINE:
		if (a.reuse) doc.getLine(0)->data = "";      // MOI: dung co reuse
		else         doc.deleteLine(a.line);
		cur.setPosition(a.line - 1, 0, doc);
		break;

	case DELETE_LINE:
		if (a.reuse) doc.getLine(0)->data = a.content;
		else         doc.insertLine(a.line, a.content);
		cur.setPosition(a.line, 0, doc);
		break;

	case COPPY_LINES:
		clipBoard = a.oldClipboard;
		hasClipBoard = a.oldHasClipboard;
		break;

	case CUT_LINES:
		insertLinesAt(a.line, a.content, a.reuse);   // MOI: them a.reuse
		clipBoard = a.oldClipboard;
		hasClipBoard = a.oldHasClipboard;
		cur.setPosition(a.line, 0, doc);
		break;

	case PASTE_LINES:
		deleteLinesAt(a.line, countLinesInText(a.content));
		cur.setPosition(a.line, 0, doc);
		break;

	case REPLACE_TEXT:                               // MOI
		p = doc.getLine(a.line);
		if (p != nullptr)
			p->data.replace(a.column, a.newContent.length(), a.content);
		cur.setPosition(a.line, a.column, doc);
		break;

	default:
		break;
	}
}

void textEdit::redoOne(action& a) {
	lineNode* p = nullptr;

	switch (a.type) {
	case INSERT_CHAR:
		p = doc.getLine(a.line);
		if (p != nullptr && a.column <= (int)p->data.length())
			p->data.insert(a.column, a.content);
		cur.setPosition(a.line, a.column + (int)a.content.length(), doc);
		break;

	case DELETE_CHAR:
		p = doc.getLine(a.line);
		if (p != nullptr && a.column < (int)p->data.length())
			p->data.erase(a.column, a.content.length());
		cur.setPosition(a.line, a.column, doc);
		break;

	case INSERT_LINE:
		if (a.reuse) doc.getLine(0)->data = a.content;
		else         doc.insertLine(a.line, a.content);
		cur.setPosition(a.line, 0, doc);
		break;

	case DELETE_LINE:
		if (a.reuse) doc.getLine(0)->data = "";
		else         doc.deleteLine(a.line);
		cur.setPosition(a.line, 0, doc);
		break;

	case COPPY_LINES:
		clipBoard = a.newClipboard;
		hasClipBoard = a.newHasClipboard;
		break;

	case CUT_LINES:
		deleteLinesAt(a.line, countLinesInText(a.content));
		clipBoard = a.newClipboard;
		hasClipBoard = a.newHasClipboard;
		cur.setPosition(a.line, 0, doc);
		break;

	case PASTE_LINES:
		insertLinesAt(a.line, a.content, a.reuse);
		cur.setPosition(a.line + countLinesInText(a.content) - 1, 0, doc);
		break;

	case REPLACE_TEXT:
		p = doc.getLine(a.line);
		if (p != nullptr)
			p->data.replace(a.column, a.content.length(), a.newContent);
		cur.setPosition(a.line, a.column, doc);
		break;

	default:
		break;
	}
}

void textEdit::undo() {
	if (undoStack.isEmpty()) {
		cout << "Khong con thao tac nao de undo\n";
		return;
	}
	modified = true;
	action first = undoStack.pop();
	int n = first.groupSize;               // thuong = 1

	undoOne(first);
	redoStack.push(first);

	for (int i = 1; i < n && !undoStack.isEmpty(); i++) {
		action a = undoStack.pop();
		undoOne(a);
		redoStack.push(a);
	}
}

void textEdit::redo() {
	if (redoStack.isEmpty()) {
		cout << "Khong con thao tac nao de redo\n";
		return;
	}

	modified = true;
	action first = redoStack.pop();
	int n = first.groupSize;

	redoOne(first);
	undoStack.push(first);

	for (int i = 1; i < n && !redoStack.isEmpty(); i++) {
		action a = redoStack.pop();
		redoOne(a);
		undoStack.push(a);
	}
}



void textEdit::insertLine(string text) {
	int l = cur.getLine();

	// van ban dang chi co 1 dong rong -> ghi de len dong do (reuse = true)
	if (doc.getLineCount() == 1 && doc.getLine(0)->data == "") {
		doc.getLine(0)->data = text;

		action a(INSERT_LINE, 0, 0, text);
		a.reuse = true;                    // MOI
		undoStack.push(a);

		redoStack.clear();
		modified = true;
		cur.setPosition(0, 0, doc);
		return;
	}

	int newLine = l + 1;
	doc.insertLine(newLine, text);
	undoStack.push(action(INSERT_LINE, newLine, 0, text));
	redoStack.clear();
	modified = true;                       // THEM
	cur.setPosition(newLine, 0, doc);
}

void textEdit::deleteLine() {
	int l = cur.getLine();
	lineNode* p = doc.getLine(l);

	if (p == nullptr) {
		return;
	}

	string deleted = p->data;
	bool onlyLine = (doc.getLineCount() == 1);   // MOI: ghi truoc khi xoa

	doc.deleteLine(l);

	action a(DELETE_LINE, l, 0, deleted);
	a.reuse = onlyLine;                          // MOI
	undoStack.push(a);
	redoStack.clear();
	modified = true;

	int newline = l;

	if (newline >= doc.getLineCount())
		newline = doc.getLineCount() - 1;

	cur.setPosition(newline, 0, doc);
}

void textEdit::copyLines(int start, int end) {
	start--; //
	end--;
	if (start < 0 || end < 0 || start >= doc.getLineCount() || end >= doc.getLineCount())
		return;

	if (start > end) {
		int temp = start;
		start = end;
		end = temp;
	}

	string oldClip = clipBoard;
	bool oldHas = hasClipBoard;
	clipBoard = "";
	hasClipBoard = false;
	
	for (int i = start; i <= end; i++) {

		lineNode* p = doc.getLine(i);

		if (p == nullptr)
			return;

		clipBoard += p->data;

		if (i < end)
			clipBoard += '\n';
	}

	hasClipBoard = true;

	undoStack.push(action(COPPY_LINES, start, 0, "", oldClip, clipBoard, oldHas, hasClipBoard));
	redoStack.clear();
}

void textEdit::cutLines(int start, int end) {
	start--;
	end--;
	if (start < 0 || end < 0 || start >= doc.getLineCount() || end >= doc.getLineCount())
		return;

	if (start > end) {
		int temp = start;
		start = end;
		end = temp;
	}

	string oldClip = clipBoard;
	bool oldHas = hasClipBoard;

	// lay noi dung cac dong bi cat
	string deletedText = "";

	for (int i = start; i <= end; i++) {
		lineNode* p = doc.getLine(i);

		if (p == nullptr)
			return;

		deletedText += p->data;

		if (i < end)
			deletedText += '\n';
	}

	// dua noi dung vao Clipboard
	clipBoard = deletedText;
	hasClipBoard = true;

	int count = end - start + 1;
	bool cutAll = (count == doc.getLineCount());   // MOI: tinh TRUOC khi xoa
	deleteLinesAt(start, count);

	// Luu thao tac CUT
	action a(CUT_LINES, start, 0, deletedText, oldClip, clipBoard, oldHas, hasClipBoard);
	a.reuse = cutAll;                              // MOI
	undoStack.push(a);

	redoStack.clear();
	modified = true;
	int newLine = start;

	if (newLine >= doc.getLineCount())
		newLine = doc.getLineCount() - 1;

	cur.setPosition(newLine, 0, doc);
}

void textEdit::pasteLines() {
	if (!hasClipBoard)
		return;

	int insertPos = cur.getLine() + 1;

	// van ban chi co 1 dong rong -> dong dau cua doan dan ghi de len dong do
	bool reuse = false;
	if (doc.getLineCount() == 1 && doc.getLine(0)->data == "") {
		insertPos = 0;
		reuse = true;
	}

	string pastedText = clipBoard;

	insertLinesAt(insertPos, pastedText, reuse);   // MOI: them tham so reuse

	int count = countLinesInText(pastedText);

	action a(PASTE_LINES, insertPos, 0, pastedText);
	a.reuse = reuse;                               // MOI
	undoStack.push(a);
	redoStack.clear();
	modified = true;
	cur.setPosition(insertPos + count - 1, 0, doc);
}

void textEdit::moveUp() {
	cur.moveUp(doc);
}
void textEdit::moveDown() {
	cur.moveDown(doc);
}
void textEdit::moveLeft() {
	cur.moveLeft(doc);
}
void textEdit::moveRight() {
	cur.moveRight(doc);
}

void textEdit::showCursorPosition() {
	cout << "Line: " << cur.getLine() + 1 << " | Column: " << cur.getColumn() + 1 << endl;
}

void textEdit::display() {
	doc.display();
}


//  Search/Replace

int textEdit::countOccurrences(string text, string keyword) {
	if (keyword.empty())
		return 0;

	int count = 0;
	size_t pos = text.find(keyword, 0);

	while (pos != string::npos) {
		count++;
		pos = text.find(keyword, pos + keyword.length());
	}
	return count;
}

void textEdit::search(string keyword) {
	if (keyword.empty()) {
		cout << "Tu khoa rong\n";
		return;
	}

	int total = 0;
	int lineIndex = 0;
	lineNode* p = doc.getLine(0);

	while (p != nullptr) {
		size_t pos = p->data.find(keyword, 0);

		while (pos != string::npos) {
			cout << "Tim thay tai Line: " << lineIndex + 1
				<< " | Column: " << pos + 1 << endl;
			total++;
			pos = p->data.find(keyword, pos + keyword.length());
		}

		p = p->pNext;
		lineIndex++;
	}

	if (total == 0)
		cout << "Khong tim thay \"" << keyword << "\"\n";
	else
		cout << "Tong cong: " << total << " ket qua\n";
}

void textEdit::replace(string oldText, string newText) {
	if (oldText.empty())
		return;

	// dem truoc so lan thay the de gan groupSize
	int total = 0;
	for (lineNode* p = doc.getLine(0); p != nullptr; p = p->pNext)
		total += countOccurrences(p->data, oldText);

	if (total == 0) {
		cout << "Khong tim thay \"" << oldText << "\" de thay the\n";
		return;
	}

	int k = 0;
	int lineIndex = 0;
	int lastLine = 0, lastCol = 0;

	for (lineNode* p = doc.getLine(0); p != nullptr; p = p->pNext, lineIndex++) {
		size_t pos = p->data.find(oldText, 0);

		while (pos != string::npos) {
			p->data.replace(pos, oldText.length(), newText);
			k++;

			action a(REPLACE_TEXT, lineIndex, (int)pos, oldText);
			a.newContent = newText;
			if (k == 1 || k == total)        // undo doc tu cuoi, redo doc tu dau
				a.groupSize = total;
			undoStack.push(a);

			lastLine = lineIndex;
			lastCol = (int)pos;

			// tim tiep SAU doan vua thay (tranh lap vo han neu newText chua oldText)
			pos = p->data.find(oldText, pos + newText.length());
		}
	}

	redoStack.clear();
	modified = true;
	cur.setPosition(lastLine, lastCol, doc);
	cout << "Da thay the " << total << " vi tri\n";
}

//Statistics

void textEdit::showStatistics() {
	int lines = doc.getLineCount();
	int words = 0;
	int chars = 0;          // tat ca ky tu (khong tinh ky tu xuong dong)
	int charsNoSpace = 0;   // khong tinh khoang trang

	for (lineNode* p = doc.getLine(0); p != nullptr; p = p->pNext) {
		bool inWord = false;

		for (int i = 0; i < (int)p->data.length(); i++) {
			char ch = p->data[i];
			chars++;

			if (ch == ' ' || ch == '\t') {
				inWord = false;
			}
			else {
				charsNoSpace++;
				if (!inWord) {
					words++;
					inWord = true;
				}
			}
		}
	}

	cout << "So dong: " << lines << endl;
	cout << "So tu: " << words << endl;
	cout << "So ky tu: " << chars << endl;
	cout << "So ky tu (khong tinh khoang trang): " << charsNoSpace << endl;
}

//Textfile
bool textEdit::openFile(string path, bool force) {
	if (modified && !force) {
		cout << "Van ban co thay doi chua luu! Hay Save truoc hoac mo voi force = true\n";
		return false;
	}

	if (!file.open(path, doc))
		return false;

	undoStack.clear();      // lich su cu khong con dung voi noi dung moi
	redoStack.clear();
	cur.setPosition(0, 0, doc);
	modified = false;
	return true;
}

bool textEdit::saveFile() {
	if (!file.save(doc))
		return false;
	modified = false;
	return true;
}

bool textEdit::saveFileAs(string path) {
	if (!file.saveAs(path, doc))
		return false;
	modified = false;
	return true;
}