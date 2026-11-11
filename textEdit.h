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

public:
	//constructor
	textEdit();

	//them xoa ki tu
	void insertChar(char ch);
	void deleteChar();

	void insertLine(string text);
	void deleteLine();

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

	//hien thi
	void display();
};

textEdit::textEdit() {
	if (doc.getLineCount() == 0) {
		doc.insertLine(0, "");
	}

	clipBoard = "";
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
	lineNode* p = doc.getLine(a.line);

	if (p == nullptr) {
		undoStack.push(a);
		return;
	}

	if (a.type == INSERT_CHAR) {
		if (a.column < 0 || a.column + a.content.length() > p->data.length()) {
			undoStack.push(a);
			return;
		}

		p->data.erase(a.column, a.content.length());
		cur.setPosition(a.line, a.column, doc);
	}
	else if (a.type == DELETE_CHAR) {
		if (a.column < 0 || a.column > p->data.length()) {
			undoStack.push(a);
			return;
		}

		p->data.insert(a.column, a.content);
		cur.setPosition(a.line, a.column + a.content.length(), doc);
	}

	redoStack.push(a);
}
void textEdit::redo() {
	if (redoStack.isEmpty())
		return;

	action a = redoStack.pop();
	lineNode* p = doc.getLine(a.line);

	if (p == nullptr) {
		redoStack.push(a);
		return;
	}

	if (a.type == INSERT_CHAR) {
		if (a.column < 0 || a.column > p->data.length()) {
			redoStack.push(a);
			return;
		}

		p->data.insert(a.column, a.content);
		cur.setPosition(a.line, a.column + a.content.length(), doc);
	}
	else if (a.type == DELETE_CHAR) {
		if (a.column < 0 || a.column + a.content.length() > p->data.length()) {
			redoStack.push(a);
			return;
		}

		p->data.erase(a.column, a.content.length());
		cur.setPosition(a.line, a.column, doc);
	}

	undoStack.push(a);
}

void textEdit::display() {
	doc.display();
}