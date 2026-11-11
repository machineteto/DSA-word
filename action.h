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
    INSERT_TEXT,
    DELETE_TEXT
};

class action {
public:
    actionType type;
    int line;
    int column;
    string content;
    //constructor
    action(actionType t = INSERT_CHAR, int l = 0, int c = 0, string text = "") {
        type = t;
        line = l;
        column = c;
        content = text;
    }
};
