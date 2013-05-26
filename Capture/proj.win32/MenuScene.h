#ifndef MENUSCENE_H
#define MENUSCENE_H


#include "cocos2d.h"
USING_NS_CC;

//-------------------------------------------Menu
class MenuLayer : public CCLayer {
public:
	virtual bool init();
	void goLevel(CCObject *obj);
	 void  show();
	 void hide();
	 void HideAniCompled();
public:
	CREATE_FUNC(MenuLayer);
};



//-------------------------------------------Level
class LevelLayer : public MenuLayer {
public:
	virtual bool init();
	void goLoading(CCObject *obj);

public:
	CREATE_FUNC(LevelLayer);
};



//---------------------------------------------Scene
class MenuScene : public CCScene {
public:
	virtual bool init() {

		CCSize winSize = CCDirector::sharedDirector()->getWinSize();
		//create background
		CCSprite *pBG = CCSprite::create("Space.jpg");
		pBG->setPosition(ccp(winSize.width/2,winSize.height/2));
		this->addChild(pBG);

		//create menu title
		CCLabelTTF* pLabel = CCLabelTTF::create("Capture", "gantz.ttf", 100);
		CCLabelTTF* pLabel2 = CCLabelTTF::create("Capture", "gantz.ttf", 100);
		pLabel->setColor(ccc3(0,0,0));
		pLabel2->setColor(ccc3(255,255,255));
		pLabel->setPosition(ccp(winSize.width/2,winSize.height/4*3));
		pLabel2->setPosition(ccp(winSize.width/2+3,winSize.height/4*3+1));
		this->addChild(pLabel,2);
		this->addChild(pLabel2,1);




		MenuLayer *menuLayer = MenuLayer::create();
		LevelLayer *levelLayer = LevelLayer::create();
		levelLayer->setVisible(false);
		this->addChild(menuLayer,1,1); 
		this->addChild(levelLayer,1,2);
		return true;
	
	}
	CREATE_FUNC(MenuScene);
};



#endif