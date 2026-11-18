#pragma once
//#9 hien thi giao dien console: so dong, con tro, status bar + doc phim
#include <iostream>
#include <string>
#include <algorithm>   // std::min, std::max
#include "editorMode.h"
using namespace std;

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#include <sys/select.h>
#endif

class editorUI {
private:
	int topLine;        // dong dau tien dang hien (cuon man hinh)
	int viewHeight;     // so dong van ban hien thi
	int screenWidth;

#ifndef _WIN32
	termios oldTerm;
	bool rawOn;
#endif

	string padNumber(int n, int width);
	string modeColor(modeType m);

public:
	editorUI(int height = 20, int width = 80);
	~editorUI();

	void render(textEdit& ed, editorMode& mode);
	int readKey();          // tra ve -1 neu het du lieu nhap
	void pause();
	void clearScreen();
};

editorUI::editorUI(int height, int width) {
	topLine = 0;
	viewHeight = height;
	screenWidth = width;

#ifdef _WIN32
	// bat ANSI escape (mau, in nguoc) tren console Windows
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD mode = 0;
	if (GetConsoleMode(h, &mode))
		SetConsoleMode(h, mode | 0x0004);   // ENABLE_VIRTUAL_TERMINAL_PROCESSING
#else
	rawOn = false;
	if (tcgetattr(STDIN_FILENO, &oldTerm) == 0) {
		termios raw = oldTerm;
		raw.c_lflag &= ~(ICANON | ECHO);
		raw.c_cc[VMIN] = 1;
		raw.c_cc[VTIME] = 0;
		tcsetattr(STDIN_FILENO, TCSANOW, &raw);
		rawOn = true;
	}
#endif
	clearScreen();
}

editorUI::~editorUI() {
	cout << "\x1b[0m\x1b[?25h" << flush;
#ifndef _WIN32
	if (rawOn) tcsetattr(STDIN_FILENO, TCSANOW, &oldTerm);
#endif
}

void editorUI::clearScreen() {
	cout << "\x1b[2J\x1b[H" << flush;
}

string editorUI::padNumber(int n, int width) {
	string s = to_string(n);
	while ((int)s.length() < width) s = " " + s;
	return s;
}

string editorUI::modeColor(modeType m) {
	switch (m) {
	case NORMAL_MODE:  return "\x1b[30;42m";   // chu den, nen xanh la
	case INSERT_MODE:  return "\x1b[30;44m";   // nen xanh duong
	case VISUAL_MODE:  return "\x1b[30;45m";   // nen tim
	case COMMAND_MODE: return "\x1b[30;43m";   // nen vang
	}
	return "";
}

void editorUI::render(textEdit& ed, editorMode& mode) {
	int total = ed.getLineCount();
	int curL = ed.getCursorLine();
	int curC = ed.getCursorColumn();

	// cuon man hinh theo con tro
	if (curL < topLine) topLine = curL;
	if (curL >= topLine + viewHeight) topLine = curL - viewHeight + 1;

	int numWidth = (int)to_string(total).length();
	if (numWidth < 3) numWidth = 3;

	bool visual = (mode.getMode() == VISUAL_MODE);
	int vs = visual ? min(mode.getVisualAnchor(), curL) : -1;
	int ve = visual ? max(mode.getVisualAnchor(), curL) : -1;

	string cursorStyle = (mode.getMode() == INSERT_MODE) ? "\x1b[4m" : "\x1b[7m";

	string out = "\x1b[?25l\x1b[H";

	// ---- vung van ban ----
	for (int row = 0; row < viewHeight; row++) {
		int idx = topLine + row;

		if (idx >= total) {
			out += "\x1b[34m~\x1b[0m\x1b[K\r\n";
			continue;
		}

		string text = ed.getLineText(idx);
		bool selected = visual && idx >= vs && idx <= ve;
		string bg = selected ? "\x1b[44m" : "";

		out += (idx == curL) ? "\x1b[1;33m" : "\x1b[90m";
		out += padNumber(idx + 1, numWidth);
		out += "\x1b[0m | " + bg;

		if (idx == curL) {
			string before = text.substr(0, curC);
			string ch = (curC < (int)text.length()) ? text.substr(curC, 1) : " ";
			string after = (curC < (int)text.length()) ? text.substr(curC + 1) : "";

			out += before;
			out += "\x1b[0m" + cursorStyle + ch + "\x1b[0m" + bg;
			out += after;
		}
		else {
			out += text;
		}
		out += "\x1b[0m\x1b[K\r\n";
	}

	// ---- status bar ----
	string label = " " + mode.getModeName() + " ";
	string file = ed.hasFileName() ? ed.getFileName() : "[No Name]";
	if (ed.isModified()) file += " [+]";

	string left = " " + file;
	string right = "Ln " + to_string(curL + 1) + "/" + to_string(total) +
		", Col " + to_string(curC + 1) + " ";

	int used = (int)(label.length() + left.length() + right.length());
	int gap = screenWidth - used;
	if (gap < 1) gap = 1;

	out += modeColor(mode.getMode()) + label + "\x1b[0m";
	out += "\x1b[7m" + left + string(gap, ' ') + right + "\x1b[0m\x1b[K\r\n";

	// ---- dong lenh / thong bao ----
	if (mode.getMode() == COMMAND_MODE) {
		out += ":" + mode.getCommandBuffer() + "_";
	}
	else if (mode.getPendingKey() != 0) {
		out += string(1, mode.getPendingKey());
	}
	else {
		out += mode.getMessage();
	}
	out += "\x1b[K\x1b[J";

	cout << out << flush;
}

void editorUI::pause() {
	cout << "\n\x1b[?25h-- Nhan phim bat ky de tiep tuc --" << flush;
	readKey();
	clearScreen();
}

int editorUI::readKey() {
#ifdef _WIN32
	int c = _getch();
	if (c == 0 || c == 224) {              // phim dac biet (mui ten...)
		int c2 = _getch();
		switch (c2) {
		case 72: return KEY_UP;
		case 80: return KEY_DOWN;
		case 75: return KEY_LEFT;
		case 77: return KEY_RIGHT;
		}
		return 0;
	}
	if (c == 10) return KEY_ENTER;
	return c;
#else
	unsigned char c;
	if (read(STDIN_FILENO, &c, 1) <= 0) return -1;

	if (c == 27) {                          // Esc hoac chuoi mui ten ESC [ A..D
		fd_set fds;
		FD_ZERO(&fds);
		FD_SET(STDIN_FILENO, &fds);
		timeval tv = { 0, 50000 };
		if (select(STDIN_FILENO + 1, &fds, nullptr, nullptr, &tv) > 0) {
			unsigned char s[2];
			if (read(STDIN_FILENO, &s[0], 1) > 0 && s[0] == '[' &&
				read(STDIN_FILENO, &s[1], 1) > 0) {
				switch (s[1]) {
				case 'A': return KEY_UP;
				case 'B': return KEY_DOWN;
				case 'C': return KEY_RIGHT;
				case 'D': return KEY_LEFT;
				}
				return 0;
			}
		}
		return KEY_ESC;
	}
	if (c == 10) return KEY_ENTER;
	if (c == 127) return KEY_BACKSPACE;
	return c;
#endif
}