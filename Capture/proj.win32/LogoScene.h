#ifndef LOGOSCENE_H
#define LOGOSCENE_H


#include "cocos2d.h"
USING_NS_CC;


class LogoLayer : public CCLayer {
public:
	virtual bool init();
	void goMenu();

public:
	CREATE_FUNC(LogoLayer);
};

class LogoScene : public CCScene {
public:
	virtual bool init() {this->addChild(LogoLayer::create());return true;}
	CREATE_FUNC(LogoScene);
};

#endif