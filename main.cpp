#include <iostream>
#include "textEdit.h"
using namespace std;

int main() {
    textEdit editor;

    cout << "\n===== TEST INSERT LINE =====\n";

    editor.insertLine("Hello");
    editor.insertLine("World");
    editor.insertLine("C++");

    editor.display();

    cout << "\n===== TEST DELETE LINE =====\n";

    editor.deleteLine();
    editor.display();

    cout << "\n===== TEST UNDO DELETE =====\n";

    editor.undo();
    editor.display();

    cout << "\n===== TEST UNDO INSERT =====\n";

    editor.undo();
    editor.display();

    cout << "\n===== TEST REDO =====\n";

    editor.redo();
    editor.display();

    editor.redo();
    editor.display();

    return 0;
}