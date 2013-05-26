#include "MenuScene.h"
#include "LoadingScene.h"

bool MenuLayer::init() {
	bool bRet = false;
	do {

		CCSize winSize = CCDirector::sharedDirector()->getWinSize();

		//creat menu item
		CCPoint offset = ccp(85,27);
		CCMenuItemImage *pStart = CCMenuItemImage::create("btn.png","btn2.png",this,menu_selector(MenuLayer::goLevel));
		CCLabelTTF *text = CCLabelTTF::create("Start", "Artbrush.ttf", 30);
		text->setPosition(offset);
		pStart->addChild(text);
		CCMenuItemImage *pSetting = CCMenuItemImage::create("btn.png","btn2.png",this,menu_selector(MenuLayer::goLevel));
		text = CCLabelTTF::create("Setting", "Artbrush.ttf", 30);
		text->setPosition(offset);
		pSetting->addChild(text);
		CCMenuItemImage *pEnd = CCMenuItemImage::create("btn.png","btn2.png",this,menu_selector(MenuLayer::goLevel));
		text = CCLabelTTF::create("Exit", "Artbrush.ttf", 30);
		text->setPosition(offset);
		pEnd->addChild(text);
		CCMenu *pMenu = CCMenu::create(pStart,pSetting,pEnd,NULL);
		pMenu->alignItemsVerticallyWithPadding(10);
		pMenu->setPosition(ccp(winSize.width/2,winSize.height/3));
		this->addChild(pMenu);

		bRet = true;
	}while(0);

	return bRet;
}

void MenuLayer::goLevel(CCObject *obj) {
	LevelLayer *levelLayer = (LevelLayer *)this->getParent()->getChildByTag(2);
	this->hide();
	levelLayer->show();
}

void MenuLayer::show() {
	this->setVisible(true);
	this->setPosition(ccp(CCDirector::sharedDirector()->getWinSize().width,0));
	CCMoveTo *move = CCMoveTo::create(0.5f,ccp(0,0));
    this->runAction(move);
}

void MenuLayer::hide() {
	CCMoveTo *move = CCMoveTo::create(0.5f,ccp(-CCDirector::sharedDirector()->getWinSize().width,0));
	CCSequence *seq = CCSequence::create(move, CCCallFunc::create(this, callfunc_selector(MenuLayer::HideAniCompled)),NULL);
	this->runAction(seq);
}

void MenuLayer::HideAniCompled()
{
    this->setVisible(false);
}

//--------------------------------------------------------------level layer
bool LevelLayer::init() {
		bool bRet = false;
	do {

		CCSize winSize = CCDirector::sharedDirector()->getWinSize();
		CCPoint offset = ccp(170,100);
		for(int i=0;i<3;i++) {
			for(int j=0;j<3;j++) {
				CCMenuItemImage *level = CCMenuItemImage::create("btn.png","btn2.png",this,menu_selector(LevelLayer::goLoading));
				CCMenu *pMenu = CCMenu::createWithItem(level);	
				pMenu->setPosition(offset.x+220*j,80*i+offset.y);
				CCMenuItem *item = CCMenuItem::create();
				char str[10];
				std::sprintf(str,"%d ",(j * 4 + i + 1));
				CCLabelTTF *text = CCLabelTTF::create(str , "Artbrush.ttf", 30);
				text->setColor(ccc3(0,0,0));
				pMenu->addChild(item);
				item->addChild(text);
				this->addChild(pMenu);
			}
		}

		bRet = true;
	}while(0);

	return bRet;
}

void LevelLayer::goLoading(CCObject *obj) {
	LoadingScene *loadingScene = LoadingScene::create();
	CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(1.2f, loadingScene,ccc3(255,255,255)));
}