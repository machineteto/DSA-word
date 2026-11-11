
#include <iostream>
#include "textEdit.h"

using namespace std;

int main() {

    // =================================
    // TEST 1: TEXTDOC
    // =================================
    cout << "\n========== TEST TEXTDOC ==========\n";

    textDoc doc;

    doc.insertLine(0, "Hello");
    doc.insertLine(1, "World");
    doc.insertLine(2, "C++");

    cout << "\n--- Insert Line ---\n";
    doc.display();

    cout << "\n--- Delete Line ---\n";
    doc.deleteLine(1);
    doc.display();

    cout << "\n--- Get Line ---\n";
    lineNode* p = doc.getLine(1);

    if (p != nullptr)
        cout << "Line 2: " << p->data << endl;


    // =================================
    // TEST 2: CURSOR
    // =================================
    cout << "\n========== TEST CURSOR ==========\n";

    cursor cur;

    cout << "Initial: "
        << cur.getLine() << " - "
        << cur.getColumn() << endl;

    cur.moveRight(doc);

    cout << "Move Right: "
        << cur.getLine() << " - "
        << cur.getColumn() << endl;

    cur.moveDown(doc);

    cout << "Move Down: "
        << cur.getLine() << " - "
        << cur.getColumn() << endl;

    cur.moveLeft(doc);

    cout << "Move Left: "
        << cur.getLine() << " - "
        << cur.getColumn() << endl;

    cur.moveUp(doc);

    cout << "Move Up: "
        << cur.getLine() << " - "
        << cur.getColumn() << endl;


    // =================================
    // TEST 3: ACTIONSTACK
    // =================================
    cout << "\n========== TEST ACTIONSTACK ==========\n";

    actionStack st;

    st.push(action(INSERT_CHAR, 0, 0, "A"));
    st.push(action(INSERT_CHAR, 0, 1, "B"));
    st.push(action(INSERT_CHAR, 0, 2, "C"));

    cout << "\n--- Pop Stack ---\n";

    while (!st.isEmpty()) {
        action a = st.pop();

        cout << "Type: " << a.type
            << " | Line: " << a.line
            << " | Column: " << a.column
            << " | Content: " << a.content << endl;
    }


    // =================================
    // TEST 4: TEXTEDIT
    // =================================
    cout << "\n========== TEST TEXTEDIT ==========\n";

    textEdit editor;

    cout << "\n--- Insert Character ---\n";

    editor.insertChar('H');
    editor.insertChar('e');
    editor.insertChar('l');
    editor.insertChar('l');
    editor.insertChar('o');

    editor.display();

    cout << "\n--- Undo ---\n";
    editor.undo();
    editor.display();

    cout << "\n--- Undo Again ---\n";
    editor.undo();
    editor.display();

    cout << "\n--- Redo ---\n";
    editor.redo();
    editor.display();

    cout << "\n--- Redo Again ---\n";
    editor.redo();
    editor.display();

    cout << "\n--- Undo All ---\n";

    for (int i = 0; i < 5; i++)
        editor.undo();

    editor.display();

    cout << "\n--- Redo All ---\n";

    for (int i = 0; i < 5; i++)
        editor.redo();

    editor.display();

    return 0;
}
