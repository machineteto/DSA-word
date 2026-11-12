#pragma once
//#6 
#include "textDoc.h"
#include "cursor.h"
#include "actionStack.h"

class textEdit {
private:
	textDoc doc;
	cursor cur;
	actionStack undoStack;
	actionStack redoStack;

	string clipBoard;
	bool hasClipBoard;

	void insertLinesAt(int position, string text);
	void deleteLinesAt(int position, int count);
	int countLinesInText(string text);

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
};

void textEdit::insertLinesAt(int position, string text) {
	int start = 0;
	int lineCount = countLinesInText(text);

	for (int i = 0; i < lineCount; i++) {
		size_t pos = text.find('\n', start);
		string lineText;

		if (pos == string::npos)
			lineText = text.substr(start);
		else
			lineText = text.substr(start, pos - start);

		// tai su dung dong rong neu tai lieu chi co mot dong rong
		if (doc.getLineCount() == 1 && doc.getLine(0)->data == "" && position == 0) {
			doc.getLine(0)->data = lineText;
		}
		else {
			doc.insertLine(position, lineText);
		}

		position++;

		if (pos != string::npos)
			start = pos + 1;
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
}

void textEdit::undo() {
	if (undoStack.isEmpty())
		return;

	action a = undoStack.pop();

	lineNode* p = nullptr;

	switch (a.type) {
	case INSERT_CHAR:
		p = doc.getLine(a.line);

		if (p != nullptr && a.column < p->data.length()) {
			p->data.erase(a.column, a.content.length());
		}
		break;

	case DELETE_CHAR:
		p = doc.getLine(a.line);

		if (p != nullptr) {
			p->data.insert(a.column, a.content);
		}
		break;

	case INSERT_LINE:
		if (doc.getLineCount() == 1) {
			doc.getLine(0)->data = "";
		}
		else {
			doc.deleteLine(a.line);
		}
		break;

	case DELETE_LINE:
		if (doc.getLineCount() == 1 && doc.getLine(0)->data == "") {
			doc.getLine(0)->data = a.content;
		}
		else {
			doc.insertLine(a.line, a.content);
		}
		break;

		// Undo COPY: khoi phuc Clipboard cu
	case COPPY_LINES:
		clipBoard = a.oldClipboard;
		hasClipBoard = a.oldHasClipboard;
		break;

		// Undo CUT: khoi phuc cac dong va Clipboard cu
	case CUT_LINES: {
		insertLinesAt(a.line, a.content);

		clipBoard = a.oldClipboard;
		hasClipBoard = a.oldHasClipboard;

		cur.setPosition(a.line, 0, doc);
		break;
	}

		// Undo PASTE: xoa cac dong vua dan
	case PASTE_LINES: {
		int count = countLinesInText(a.content);

		deleteLinesAt(a.line, count);

		cur.setPosition(a.line, 0, doc);
		break;
	}
	}

	redoStack.push(a);
}
void textEdit::redo() {
	if (redoStack.isEmpty())
		return;

	action a = redoStack.pop();

	lineNode* p = nullptr;

	switch (a.type) {
	case INSERT_CHAR:
		p = doc.getLine(a.line);

		if (p != nullptr) {
			p->data.insert(a.column, a.content);
		}
		break;

	case DELETE_CHAR:
		p = doc.getLine(a.line);

		if (p != nullptr &&
			a.column < p->data.length()) {
			p->data.erase(a.column, a.content.length());
		}
		break;

	case INSERT_LINE:
		if (doc.getLineCount() == 1 && doc.getLine(0)->data == "") {
			doc.getLine(0)->data = a.content;
		}
		else {
			doc.insertLine(a.line, a.content);
		}
		break;

	case DELETE_LINE:
		if (doc.getLineCount() == 1) {
			doc.getLine(0)->data = "";
		}
		else {
			doc.deleteLine(a.line);
		}
		break;

		// Redo COPY: khoi phuc Clipboard moi
	case COPPY_LINES:
		clipBoard = a.newClipboard;
		hasClipBoard = a.newHasClipboard;
		break;

		// Redo CUT: cat lai cac dong
	case CUT_LINES: {
		int count = countLinesInText(a.content);

		deleteLinesAt(a.line, count);

		clipBoard = a.newClipboard;
		hasClipBoard = a.newHasClipboard;

		cur.setPosition(a.line, 0, doc);
		break;
	}

		// Redo PASTE: dan lai cac dong
	case PASTE_LINES: {
		insertLinesAt(a.line, a.content);

		int count = countLinesInText(a.content);

		cur.setPosition(a.line + count - 1, 0, doc);
		break;
	}
	}

	undoStack.push(a);
}

void textEdit::insertLine(string text) {
	int l = cur.getLine();

	if (doc.getLineCount() == 1) {

		lineNode* p = doc.getLine(0);

		if (p->data == "") {
			p->data = text;
			undoStack.push(action(INSERT_LINE, 0, 0, text));
			redoStack.clear();
			cur.setPosition(0, 0, doc);
			return;
		}
	}

	int newLine = l + 1;
	doc.insertLine(newLine, text);
	undoStack.push(action(INSERT_LINE, newLine, 0, text));
	redoStack.clear();
	cur.setPosition(newLine, 0, doc);
}
void textEdit::deleteLine() {
	int l = cur.getLine();
	lineNode* p = doc.getLine(l);

	if (p == nullptr) {
		return;
	}

	string deleted = p->data;

	doc.deleteLine(l);

	undoStack.push(action(DELETE_LINE, l, 0, deleted));
	redoStack.clear();
	
	int newline = l;

	if (newline >= doc.getLineCount())
		newline = doc.getLineCount() - 1;

	cur.setPosition(newline, 0, doc);
}

void textEdit::copyLines(int start, int end) {
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
	deleteLinesAt(start, count);

	// Luu thao tac CUT
	undoStack.push(action(CUT_LINES, start, 0, deletedText, oldClip, clipBoard, oldHas, hasClipBoard));

	redoStack.clear();
	int newLine = start;

	if (newLine >= doc.getLineCount())
		newLine = doc.getLineCount() - 1;

	cur.setPosition(newLine, 0, doc);
}
void textEdit::pasteLines() {
	if (!hasClipBoard)
		return;

	int insertPos = cur.getLine() + 1;

	if (doc.getLineCount() == 1 && doc.getLine(0)->data == "" && insertPos == 1) {
		insertPos = 0;
	}


	string pastedText = clipBoard;

	insertLinesAt(insertPos, pastedText);

	int count = countLinesInText(pastedText);

	undoStack.push(action(PASTE_LINES, insertPos, 0, pastedText));
	redoStack.clear();
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