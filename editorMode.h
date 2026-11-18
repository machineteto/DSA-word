#pragma once
//#8 quan ly che do lam viec (Normal, Insert, Visual, Command) + phim tat kieu Vim
#include <iostream>
#include <string>
#include <sstream>
#include "textEdit.h"
using namespace std;

enum modeType {
	NORMAL_MODE,
	INSERT_MODE,
	VISUAL_MODE,
	COMMAND_MODE
};

// ma phim (editorUI::readKey tra ve cac gia tri nay)
enum keyCode {
	KEY_ESC = 27,
	KEY_ENTER = 13,
	KEY_BACKSPACE = 8,
	KEY_CTRL_R = 18,
	KEY_UP = 1001,
	KEY_DOWN = 1002,
	KEY_LEFT = 1003,
	KEY_RIGHT = 1004
};

class editorMode {
private:
	modeType current;
	string commandBuffer;   // noi dung dang go sau dau ':'
	char pendingKey;        // phim cho ghep doi: d(d), y(y), g(g)
	int visualAnchor;       // dong bat dau vung chon (Visual)
	string message;         // thong bao hien o dong cuoi
	bool needPause;         // lenh co in ra man hinh -> UI phai cho nhan phim

	void enterMode(modeType m);
	void normalizeCursor(textEdit& ed);   // Normal: con tro khong dung sau ky tu cuoi

	void handleNormal(int key, textEdit& ed);
	void handleInsert(int key, textEdit& ed);
	void handleVisual(int key, textEdit& ed);
	bool handleCommand(int key, textEdit& ed);        // false = thoat chuong trinh
	bool executeCommand(string cmd, textEdit& ed);    // false = thoat chuong trinh

public:
	editorMode();

	// xu ly 1 phim, tra ve false khi nguoi dung thoat
	bool handleKey(int key, textEdit& ed);

	modeType getMode();
	string getModeName();
	string getCommandBuffer();
	char getPendingKey();
	int getVisualAnchor();
	string getMessage();
	bool consumePause();
};

editorMode::editorMode() {
	current = NORMAL_MODE;
	commandBuffer = "";
	pendingKey = 0;
	visualAnchor = 0;
	message = "";
	needPause = false;
}

modeType editorMode::getMode() { return current; }
string editorMode::getCommandBuffer() { return commandBuffer; }
char editorMode::getPendingKey() { return pendingKey; }
int editorMode::getVisualAnchor() { return visualAnchor; }
string editorMode::getMessage() { return message; }

string editorMode::getModeName() {
	switch (current) {
	case NORMAL_MODE:  return "NORMAL";
	case INSERT_MODE:  return "INSERT";
	case VISUAL_MODE:  return "VISUAL";
	case COMMAND_MODE: return "COMMAND";
	}
	return "";
}

bool editorMode::consumePause() {
	bool r = needPause;
	needPause = false;
	return r;
}

void editorMode::enterMode(modeType m) {
	current = m;
	pendingKey = 0;
	if (m == COMMAND_MODE)
		commandBuffer = "";
}

void editorMode::normalizeCursor(textEdit& ed) {
	int l = ed.getCursorLine();
	int c = ed.getCursorColumn();
	int len = (int)ed.getLineText(l).length();

	if (len > 0 && c >= len)
		ed.setCursor(l, len - 1);
}

bool editorMode::handleKey(int key, textEdit& ed) {
	message = "";

	switch (current) {
	case NORMAL_MODE:  handleNormal(key, ed);  break;
	case INSERT_MODE:  handleInsert(key, ed);  break;
	case VISUAL_MODE:  handleVisual(key, ed);  break;
	case COMMAND_MODE: return handleCommand(key, ed);
	}
	return true;
}

// ===================== NORMAL =====================
void editorMode::handleNormal(int key, textEdit& ed) {
	int l = ed.getCursorLine();
	int c = ed.getCursorColumn();
	int len = (int)ed.getLineText(l).length();

	// phim ghep doi: dd, yy, gg
	if (pendingKey != 0) {
		char p = pendingKey;
		pendingKey = 0;

		if (p == 'd' && key == 'd') {
			ed.deleteLine();
			message = "Da xoa 1 dong";
			normalizeCursor(ed);
		}
		else if (p == 'y' && key == 'y') {
			ed.copyLines(l + 1, l + 1);
			message = "Da copy 1 dong";
		}
		else if (p == 'g' && key == 'g') {
			ed.setCursor(0, 0);
		}
		return;    // phim khac -> huy to hop
	}

	switch (key) {
	// ---- di chuyen: h j k l (va phim mui ten) ----
	case 'h': case KEY_LEFT:
		if (c > 0) ed.moveLeft();
		break;
	case 'l': case KEY_RIGHT:
		if (c + 1 < len) ed.moveRight();
		break;
	case 'j': case KEY_DOWN:
		ed.moveDown();
		normalizeCursor(ed);
		break;
	case 'k': case KEY_UP:
		ed.moveUp();
		normalizeCursor(ed);
		break;

	case '0': ed.setCursor(l, 0); break;
	case '$': ed.setCursor(l, len > 0 ? len - 1 : 0); break;
	case 'G': ed.setCursor(ed.getLineCount() - 1, 0); break;

	// ---- doi che do ----
	case 'i':
		enterMode(INSERT_MODE);
		break;
	case 'v':
		visualAnchor = l;
		enterMode(VISUAL_MODE);
		break;
	case ':':
		enterMode(COMMAND_MODE);
		break;

	// ---- chinh sua ----
	case 'd': case 'y': case 'g':
		pendingKey = (char)key;
		break;
	case 'x':
		ed.deleteChar();
		normalizeCursor(ed);
		break;
	case 'P': case 'p':
		if (!ed.hasClip()) {
			message = "Clipboard rong";
		}
		else {
			ed.pasteLines();
			normalizeCursor(ed);
		}
		break;

	// ---- undo / redo ----
	case 'u':
		if (ed.canUndo()) { ed.undo(); normalizeCursor(ed); }
		else message = "Khong con thao tac nao de undo";
		break;
	case KEY_CTRL_R:
		if (ed.canRedo()) { ed.redo(); normalizeCursor(ed); }
		else message = "Khong con thao tac nao de redo";
		break;

	default:
		break;
	}
}

// ===================== INSERT =====================
void editorMode::handleInsert(int key, textEdit& ed) {
	int c = ed.getCursorColumn();

	switch (key) {
	case KEY_ESC:
		enterMode(NORMAL_MODE);
		if (c > 0) ed.moveLeft();      // giong Vim: Esc lui con tro 1 o
		break;

	case KEY_ENTER:
		ed.insertLine("");             // tao dong moi ngay duoi dong hien tai
		break;

	case KEY_BACKSPACE:
		if (c > 0) {
			ed.moveLeft();
			ed.deleteChar();
		}
		break;

	case KEY_LEFT:  ed.moveLeft();  break;
	case KEY_RIGHT: ed.moveRight(); break;
	case KEY_UP:    ed.moveUp();    break;
	case KEY_DOWN:  ed.moveDown();  break;

	default:
		if (key >= 32 && key < 127)
			ed.insertChar((char)key);
		break;
	}
}

// ===================== VISUAL (chon theo dong) =====================
void editorMode::handleVisual(int key, textEdit& ed) {
	int l = ed.getCursorLine();
	int start = (visualAnchor < l) ? visualAnchor : l;
	int end = (visualAnchor < l) ? l : visualAnchor;

	switch (key) {
	case KEY_ESC: case 'v':
		enterMode(NORMAL_MODE);
		break;

	case 'h': case KEY_LEFT:
		if (ed.getCursorColumn() > 0) ed.moveLeft();
		break;
	case 'l': case KEY_RIGHT:
		if (ed.getCursorColumn() + 1 < (int)ed.getLineText(l).length()) ed.moveRight();
		break;
	case 'j': case KEY_DOWN:
		ed.moveDown();
		break;
	case 'k': case KEY_UP:
		ed.moveUp();
		break;

	case 'y':
		ed.copyLines(start + 1, end + 1);
		ed.setCursor(start, 0);
		message = "Da copy " + to_string(end - start + 1) + " dong";
		enterMode(NORMAL_MODE);
		break;

	case 'd': case 'x':
		ed.cutLines(start + 1, end + 1);
		message = "Da cat " + to_string(end - start + 1) + " dong";
		enterMode(NORMAL_MODE);
		normalizeCursor(ed);
		break;

	default:
		break;
	}
}

// ===================== COMMAND (:w :q :wq :e ...) =====================
bool editorMode::handleCommand(int key, textEdit& ed) {
	if (key == KEY_ESC) {
		enterMode(NORMAL_MODE);
	}
	else if (key == KEY_BACKSPACE) {
		if (commandBuffer.empty()) enterMode(NORMAL_MODE);
		else commandBuffer.pop_back();
	}
	else if (key == KEY_ENTER) {
		string cmd = commandBuffer;
		enterMode(NORMAL_MODE);
		return executeCommand(cmd, ed);
	}
	else if (key >= 32 && key < 127) {
		commandBuffer += (char)key;
	}
	return true;
}

bool editorMode::executeCommand(string cmd, textEdit& ed) {
	istringstream iss(cmd);
	string name, rest;
	iss >> name;
	getline(iss, rest);
	size_t first = rest.find_first_not_of(' ');
	rest = (first == string::npos) ? "" : rest.substr(first);

	if (name.empty())
		return true;

	if (name == "q") {
		if (ed.isModified()) {
			message = "Chua luu! Dung :wq de luu va thoat, hoac :q! de bo qua";
			return true;
		}
		return false;
	}
	if (name == "q!")
		return false;

	// cac lenh con lai co the in ra man hinh -> xoa man hinh va cho nhan phim
	cout << "\x1b[2J\x1b[H";
	needPause = true;

	if (name == "w") {
		bool ok = rest.empty() ? ed.saveFile() : ed.saveFileAs(rest);
		message = ok ? "Da luu" : "Luu that bai";
	}
	else if (name == "wq") {
		bool ok = rest.empty() ? ed.saveFile() : ed.saveFileAs(rest);
		if (ok) { needPause = false; return false; }
	}
	else if (name == "e" || name == "e!") {
		if (rest.empty()) cout << "Cu phap: :e <ten file>\n";
		else if (ed.openFile(rest, name == "e!")) cout << "Da mo file: " << rest << endl;
	}
	else if (name == "search" || name == "s") {
		ed.search(rest);
	}
	else if (name == "replace" || name == "r") {
		istringstream is(rest);
		string a, b;
		is >> a >> b;
		if (a.empty()) cout << "Cu phap: :replace <cu> <moi>\n";
		else ed.replace(a, b);
	}
	else if (name == "stats") {
		ed.showStatistics();
	}
	else if (name == "redo") {
		ed.redo();
	}
	else {
		cout << "Lenh khong hop le: " << name << "\n\n";
		cout << "Phim tat:  h j k l  i  Esc  dd  yy  P  u  Ctrl+R  v  :\n";
		cout << "Lenh:      :w [file]  :q  :q!  :wq  :e[!] file\n";
		cout << "           :search tu  :replace cu moi  :stats\n";
	}
	return true;
}