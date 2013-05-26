#ifndef GAMECANVAS_H
#define GAMECANVAS_H

#include "cocos2d.h"
USING_NS_CC;

#define BORDERCOLOR ccc4f(100/255.0f,100/255.0f,100/255.0f,1)
#define FILLCOLOR ccc4f(1,1,1,1)

class GameCanvas {
public:
	GameCanvas();
	~GameCanvas();
	CCRenderTexture *getRenderTexture() {return m_rt;}
	void drawPoint(CCPoint p, ccColor4F color = BORDERCOLOR);
	void drawLine(CCPoint p1, CCPoint p2, ccColor4F color = BORDERCOLOR);
	void drawRect(CCPoint p1, CCPoint p2, ccColor4F color = BORDERCOLOR);
	void drawSolidRect(CCPoint p1, CCPoint p2, ccColor4F bColor= BORDERCOLOR, ccColor4F fColor = FILLCOLOR);
	void drawCircle(CCPoint p, int radius, ccColor4F color = BORDERCOLOR);
	void drawSolidCircle(CCPoint p, int radius, ccColor4F bColor = BORDERCOLOR, ccColor4F fColor = FILLCOLOR);
	void clear();
private:
	CCRenderTexture *m_rt;
};

#endif
