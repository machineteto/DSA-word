#pragma once
//#4 luu thong tin thao tac duoc thuc hien
#include <iostream>
#include <string>
using namespace std;

enum actionType {
    INSERT_CHAR,
    DELETE_CHAR,
    INSERT_LINE,
    DELETE_LINE,
    COPPY_LINES,
    CUT_LINES,
    PASTE_LINES,
    INSERT_TEXT,
    DELETE_TEXT,
    REPLACE_TEXT        // MOI: thay the chuoi
};

class action {
public:
    actionType type;
    int line;
    int column;
    string content;

    string oldClipboard;
    string newClipboard;
    bool oldHasClipboard;
    bool newHasClipboard;


    string newContent = "";  // chuoi moi (REPLACE_TEXT: content = chuoi cu)
    bool reuse = false;      // thao tac "dung lai" dong rong duy nhat?
    int groupSize = 1;       // so action undo/redo cung luc


    //constructor
    action(actionType t = INSERT_CHAR, int l = 0, int c = 0, string text = "", string oldClip = "", string newClip = "", bool oldHas = false, bool newHas = false) {
        type = t;
        line = l;
        column = c;
        content = text;

        oldClipboard = oldClip;
        newClipboard = newClip;
        oldHasClipboard = oldHas;
        newHasClipboard = newHas;
    }
};





