#include "textEdit.h"

int main() {
    textEdit editor;

    editor.insertLine("Hello");
    editor.insertLine("World");
    editor.insertLine("C++");
    editor.insertLine("Java");

    cout << "\n===== DOCUMENT BAN DAU =====\n";
    editor.display();

    // COPY
    cout << "\n===== COPY (1, 2) =====\n";
    editor.copyLines(1, 2);

    // PASTE
    cout << "\n===== PASTE =====\n";
    editor.pasteLines();
    editor.display();

    // UNDO PASTE
    cout << "\n===== UNDO PASTE =====\n";
    editor.undo();
    editor.display();

    // REDO PASTE
    cout << "\n===== REDO PASTE =====\n";
    editor.redo();
    editor.display();

    // CUT
    cout << "\n===== CUT (1, 2) =====\n";
    editor.cutLines(1, 2);
    editor.display();

    // UNDO CUT
    cout << "\n===== UNDO CUT =====\n";
    editor.undo();
    editor.display();

    // REDO CUT
    cout << "\n===== REDO CUT =====\n";
    editor.redo();
    editor.display();

    return 0;
}