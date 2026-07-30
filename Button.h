#pragma once
#include<iostream>
#include<graphics.h>
using namespace std;

class Button {
private:
	int x=0, y=0;
	int width=100, height=40;
	wstring text=L"";

	//颜色变量
	COLORREF normalColor = RGB(70, 130, 180);   //正常颜色
	COLORREF hoverColor = RGB(100, 160, 220);   //悬停颜色
	COLORREF clickColor = RGB(40, 90, 140);     //点击颜色

	COLORREF currentColor = normalColor;


	//按钮状态
	bool isHover = false;
	bool isPressed = false;

public:
	Button(){}

	Button(int X, int Y, int w, int h, const wstring& t) :x(X),y(Y),width(w),height(h),text(t){}
	
	void draw() {
		//根据状态决定颜色
		if (isPressed){
			currentColor = clickColor;
		}
		else if (isHover){
			currentColor = hoverColor;
		}
		else{
	        currentColor = normalColor;
		}

		//设置按钮样式
		setfillcolor(currentColor);
		setlinecolor(BLACK);

		//绘制圆角矩形按钮
		fillroundrect(x,y,x + width,y + height,15,15);

		//绘制文字
		setbkmode(TRANSPARENT);
		settextcolor(WHITE);
		settextstyle(height - 15,0,L"微软雅黑");
		//文字居中
		int tx = x + (width - textwidth(text.c_str())) / 2;
		int ty = y + (height - textheight(text.c_str())) / 2;

		outtextxy(tx,ty,text.c_str());

	}
	
	bool isClicked(int mouseX, int mouseY) {
		if (mouseX >= x && mouseX <= x + width && mouseY >= y && mouseY <= y + height) {
			return true;
		}
		else return false;
	}

	void setHover(bool flag){
		isHover = flag;
	}//设置悬停状态

	void setPressed(bool flag){
		isPressed = flag;
	}//设置点击状态

	void setText(const wstring& t){
		text = t;
	}//修改文字

	void setPosition(int X, int Y){
		x = X;
		y = Y;
	}//修改位置

	void setSize(int w, int h){
		width = w;
		height = h;
	}//修改大小
};
