#pragma once
#include <string>
#include "ofMain.h"

// namespace alias
using glm::vec2;
using std::string;
class CrossLine
{
public:
	vec2 start;
	vec2 end;
	float progress = 0;
	float totalSegements = 100;
	float speed = .5f;
	string label1 = "";
	string label2 = "";
	bool showing = false;
	float lineWidth = 4;
	CrossLine(vec2 start,vec2 end,string label1,string label2);
	void setLabel1(string label1);
	void setLabel2(string label2);
	void show();
	void hide();
	void update();
	void draw();
	void setLineWidth(float width);
	void setThin(bool thin);
	//Helper method position
	void setStart(float startX, float startY);
	void setEnd(float endX, float endY);
private:
	ofTrueTypeFont fontNormal;  // Font normal/tipis
	ofTrueTypeFont fontBold;    // Font tebal
	bool useThin = false;
};

