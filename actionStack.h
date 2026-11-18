#pragma once
//#5 quan ly stack undo redo
#include <iostream>
#include <string>
using namespace std;

#include "action.h"

class actionNode {
public:
	action data;
	actionNode* pNext;

	actionNode(action a) {
		data = a;
		pNext = nullptr;
	}
};

class actionStack {
private:
	actionNode* pTop = nullptr;
public:
	~actionStack();

	bool isEmpty();

	void push(action ac);
	action pop();

	void clear();
};

actionStack::~actionStack() {
	clear();
}

bool actionStack::isEmpty() {
	return pTop == nullptr;
}

void actionStack::push(action ac) {
	actionNode* pNew = new actionNode(ac);

	pNew->pNext = pTop;
	pTop = pNew;
}

action actionStack::pop(){
	if (isEmpty()) {
		return action();
	}

	actionNode* temp = pTop;
	action result = temp->data;

	pTop = pTop->pNext;

	delete temp;
	return result;
}

void actionStack::clear() {
	while (!isEmpty()) {
		actionNode* temp = pTop;
		pTop = pTop->pNext;
		delete temp;
	}
}
