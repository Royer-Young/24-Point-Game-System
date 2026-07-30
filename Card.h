#pragma once
#include<iostream>
#include<string>
#include<graphics.h>
using namespace std;

class Card {
private:
	int x = 0, y = 0;
	int width=80, height=120;
	int number=0;
	bool isUsed=false;

	IMAGE cardImg;      // 数字朝上的图
	IMAGE backImg;      // 已使用/背面的图

public:
	Card() {}

	Card(int X, int Y, int n) :x(X), y(Y), number(n), width(130), height(190), isUsed(false) {
		wstring path = L"cardImage\\" + to_wstring(n) + L".png";
		loadimage(&cardImg, path.c_str(), width, height);
		loadimage(&backImg, L"cardImage\\back.png", width, height);
	}

	void draw() {
		if (isUsed) {
			putimage(x, y, &backImg);
		}
		else {
			putimage(x, y, &cardImg);
		}
	};

	bool isClicked(int mx, int my) {
		if (isUsed){
			return false;
		}
		if (mx >= x &&mx <= x + width &&my >= y &&my <= y + height){
			return true;
		}

		return false;
	};

	int getNumber() {
		return number;
	};

	void setUsed(bool u) {
		isUsed = u;
	}

	bool getUsed() const {
		return isUsed;
	}
};



