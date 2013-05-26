#include "LogoScene.h"
#include "MenuScene.h"


bool LogoLayer::init() {
	bool bRet = false;
	do {
		CCSize winSize = CCDirector::sharedDirector()->getWinSize();
		//create background
		CCLayerColor *bgColor = CCLayerColor::create(ccc4(255,255,255,1));
		CC_BREAK_IF(!bgColor);
		bgColor->setPosition(ccp(winSize.width/2,winSize.height/2));
		this->addChild(bgColor);

		//create logo


		//create particle
		CCParticleSystemQuad *emitter = CCParticleSystemQuad::create("Logo_bg.plist");
		CC_BREAK_IF(!emitter);
		emitter->setPosition(ccp(winSize.width/2,winSize.height/2));
		this->addChild(emitter, 10);

		//go into game scene
		CCCallFunc *callSelectorAction = CCCallFunc::create(this, callfunc_selector(LogoLayer::goMenu)); 
		this->runAction(CCSequence::create(
										CCDelayTime::create(0.5f),
                                        callSelectorAction, 
                                        NULL)); 

		bRet = true;
	}while(0);

	return bRet;
}

void LogoLayer::goMenu( )  {
	MenuScene *menuScene = MenuScene::create();
	CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(1.2f, menuScene,ccc3(255,255,255)));
}