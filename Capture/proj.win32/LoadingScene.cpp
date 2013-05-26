#include "LoadingScene.h"
#include "GameResourceManager.h"
#include "GameWorldScene.h"


bool LoadingLayer::init() {

	bool bRet = false;
	do{

		CCSize winSize = CCDirector::sharedDirector()->getWinSize();
		//create background
		CCSprite *pBG = CCSprite::create("carbon_fibre_v2.png",CCRect(0,0,winSize.width, winSize.height));
		CC_BREAK_IF(!pBG);
		pBG->setPosition(ccp(winSize.width/2,winSize.height/2));
		ccTexParams params =  {GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT};
		pBG->getTexture()->setTexParameters(&params);
		this->addChild(pBG, -1);

		//create loaing bar
		CCLabelTTF *pLabel = CCLabelTTF::create("", "gantz.ttf", 50);
		CC_BREAK_IF(!pLabel);
		pLabel->setPosition(ccp(winSize.width/2,winSize.height/2+50));
		pLabel->setTag(111);
		this->addChild(pLabel);

		CCSprite* pLoadingBg = CCSprite::create("ProgressBar1_bg.png");
		this->addChild(pLoadingBg);
		pLoadingBg->setPosition(ccp(winSize.width/2,winSize.height/2));

		CCProgressTimer *pLoadingFg = CCProgressTimer::create(CCSprite::create("ProgressBar1_fg.png"));
		pLoadingFg->setMidpoint(ccp(0,0));
		pLoadingFg->setType(kCCProgressTimerTypeBar);
		pLoadingFg->setBarChangeRate(ccp(0.5,0));
		pLoadingFg->setPosition(ccp(winSize.width/2,winSize.height/2));
		pLoadingFg->setTag(112);

		CCProgressTo *to = CCProgressTo::create(10,100);
		pLoadingFg->runAction(to);
		this->addChild(pLoadingFg);


		//loading game resource from files 
		GameResourceManager *resourceManager = GameResourceManager::sharedResource();
		resourceManager->loadResource(this);


		bRet = true;
	}while(0);

	return bRet;
}

void LoadingLayer::loadingCallBack(CCObject* obj) {
	static int num = 0;
	num++;
	char tmp[50];
	int per = ((float)num/10)*100;
	sprintf(tmp,"Loading: %d%%",per);
	((CCLabelTTF *)this->getChildByTag(111))->setString(tmp);

	//set loading bar scope
	((CCProgressTimer *)this->getChildByTag(112))->setBarChangeRate(ccp(1-per/100.0,0));

	if(10 == num) {
		GameWorldScene *gameScene = GameWorldScene::create(1);
		CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(1.2f, gameScene,ccc3(0,0,0)));
	}
}