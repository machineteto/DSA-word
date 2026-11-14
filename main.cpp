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

    // SEARCH
    cout << "\n===== SEARCH \"o\" =====\n";
    editor.search("o");

    cout << "\n===== SEARCH \"Python\" (khong ton tai) =====\n";
    editor.search("Python");

    // REPLACE
    cout << "\n===== REPLACE \"o\" -> \"0\" =====\n";
    editor.replace("o", "0");
    editor.display();

    // UNDO REPLACE
    cout << "\n===== UNDO REPLACE =====\n";
    editor.undo();
    editor.display();

    // REDO REPLACE
    cout << "\n===== REDO REPLACE =====\n";
    editor.redo();
    editor.display();

    // REPLACE ma chuoi moi chua chuoi cu
    cout << "\n===== REPLACE \"a\" -> \"aa\" =====\n";
    editor.replace("a", "aa");
    editor.display();

    // UNDO REPLACE
    cout << "\n===== UNDO REPLACE =====\n";
    editor.undo();
    editor.display();

    // STATISTICS
    cout << "\n===== STATISTICS =====\n";
    editor.showStatistics();

    cout << "\n===== SAVE AS =====\n";
    editor.saveFileAs("test.txt");

    editor.insertLine("Dong moi sau khi luu");

    cout << "\n===== OPEN khi chua luu (bi chan) =====\n";
    editor.openFile("test.txt");

    cout << "\n===== SAVE =====\n";
    editor.saveFile();

    cout << "\n===== OPEN =====\n";
    editor.openFile("test.txt");
    editor.display();

    cout << "\n===== UNDO sau OPEN (stack da bi xoa) =====\n";
    editor.undo();

    cout << "\n===== OPEN file khong ton tai =====\n";
    editor.openFile("khongco.txt");

    return 0;
}