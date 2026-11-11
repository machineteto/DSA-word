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

	if (a.type == INSERT_CHAR) {
		lineNode* p = doc.getLine(a.line);

		if (p == nullptr || a.column < 0 || a.column + a.content.length() > p->data.length()) {
			undoStack.push(a);
			return;
		}

		p->data.erase(a.column, a.content.length());
		cur.setPosition(a.line, a.column, doc);
	}
	else if (a.type == DELETE_CHAR) {
		lineNode* p = doc.getLine(a.line);

		if (p == nullptr ||a.column < 0 || a.column > p->data.length()) {
			undoStack.push(a);
			return;
		}

		p->data.insert(a.column, a.content);
		cur.setPosition(a.line, a.column + a.content.length(), doc);
	}
	else if (a.type == INSERT_LINE) {
		if (a.line < 0 || a.line >= doc.getLineCount()) {
			undoStack.push(a);
			return;
		}

		if (doc.getLineCount() == 1) {
			lineNode* p = doc.getLine(0);
			p->data = "";
		}
		else {
			doc.deleteLine(a.line);
		}

		int newLine = a.line;

		if (newLine >= doc.getLineCount())
			newLine = doc.getLineCount() - 1;

		cur.setPosition(newLine, 0, doc);
	}
	else if (a.type == DELETE_LINE) {
		if (a.line < 0 || a.line > doc.getLineCount()) {
			undoStack.push(a);
			return;
		}

		if (doc.getLineCount() == 1) {
			lineNode* p = doc.getLine(0);
			p->data = a.content;
		}
		else {
			doc.insertLine(a.line, a.content);
		}

		cur.setPosition(a.line, 0, doc);
	}

	redoStack.push(a);
}
void textEdit::redo() {
	if (redoStack.isEmpty())
		return;

	action a = redoStack.pop();

	if (a.type == INSERT_CHAR) {
		lineNode* p = doc.getLine(a.line);

		if (p == nullptr || a.column < 0 || a.column > p->data.length()) {
			redoStack.push(a);
			return;
		}

		p->data.insert(a.column, a.content);
		cur.setPosition(a.line, a.column + a.content.length(), doc);
	}
	else if (a.type == DELETE_CHAR) {
		lineNode* p = doc.getLine(a.line);

		if (p == nullptr || a.column < 0 || a.column + a.content.length() > p->data.length()) {
			redoStack.push(a);
			return;
		}

		p->data.erase(a.column, a.content.length());
		cur.setPosition(a.line, a.column, doc);
	}
	else if (a.type == INSERT_LINE) {
		if (a.line < 0 || a.line > doc.getLineCount()) {
			redoStack.push(a);
			return;
		}

		if (doc.getLineCount() == 1 && doc.getLine(0)->data == "") {
			doc.getLine(0)->data = a.content;
		}
		else {
			doc.insertLine(a.line, a.content);
		}

		cur.setPosition(a.line, 0, doc);
	}
	else if (a.type == DELETE_LINE) {
		if (a.line < 0 || a.line >= doc.getLineCount()) {
			redoStack.push(a);
			return;
		}

		if (doc.getLineCount() == 1) {
			lineNode* p = doc.getLine(0);
			p->data = "";
		}
		else {
			doc.deleteLine(a.line);
		}

		int newLine = a.line;

		if (newLine >= doc.getLineCount())
			newLine = doc.getLineCount() - 1;

		cur.setPosition(newLine, 0, doc);
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

void textEdit::display() {
	doc.display();
}