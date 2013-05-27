#include "GameWorldScene.h"
#include "Actor.h"
#include "ActorManager.h"

GameWorldLayer::GameWorldLayer() {
	m_ActorManager = ActorManager::shareActorManager();
}

bool GameWorldLayer::init(int level) {
	m_level = level;
	bool bRet = false;
	do {
		CCSize winSize = CCDirector::sharedDirector()->getWinSize();
		//create background
		CCSprite *pBG = CCSprite::create("bg1.jpg");
		CC_BREAK_IF(!pBG);
		pBG->setAnchorPoint(ccp(0,0));
		pBG->setPosition(ccp(0,0));
		this->addChild(pBG, -1);
		m_worldRect = pBG->getTextureRect();


		CCSize size = pBG->getTextureRect().size;
		m_ActorManager->createWrapWall(this, ccp(0,0), size);
		m_ActorManager->createDebug();
		m_ActorManager->createActor(Actor::AT_Control,(CCLayer *)this,ccp(winSize.width/2,winSize.height/2),50);
		setTouchEnabled( true );
		this->schedule(schedule_selector(GameWorldLayer::update));

		bRet = true;
	}while(0);

	return bRet;
}

void GameWorldLayer::update(float dt) {
	m_ActorManager->updateWorld(dt);

	//是否结束游戏
	if(m_ActorManager->isOver()) {
		//do somthing
		return;
	}
	
	//更新视野
	CCSize winSize = CCDirector::sharedDirector()->getWinSize();
	CCPoint actorPos = m_ActorManager->getControlActor()->getShape()->getPosition();
	//屏幕位置与背景移动相反
	CCPoint screenPos = ccp(-this->getPosition().x,-this->getPosition().y);
	//新位置
	CCPoint newPos = this->getPosition();


	//保持角色在屏幕中间
	newPos.x += winSize.width/2 - (actorPos.x - screenPos.x);
	newPos.y += winSize.height/2 - (actorPos.y - screenPos.y);

	//屏幕不超过背景
	newPos = ccpClamp(newPos,ccp(-m_worldRect.getMinX(),-m_worldRect.getMinY()),
		ccp(winSize.width-m_worldRect.getMaxX(),winSize.height-m_worldRect.getMaxY()));
	this->setPosition(newPos);
}


void GameWorldLayer::ccTouchesBegan(CCSet *pTouches, CCEvent *pEvent) {
    CCPoint location,pos[20];  
	int num=0;
    for (CCSetIterator iter = pTouches->begin(); iter != pTouches->end(); iter++)  {  
        CCTouch* pTouch = (CCTouch*)(*iter);  
		location = pTouch->getLocation();  
        pos[num++] = convertToNodeSpace(location);
    }//遍历取出每个触摸点坐标 

	m_beginNum = num;
	//当触摸点大于2时
	if(num>=2) {
		m_beginLen = ccpDistance(pos[0],pos[1]);
	}

}

void GameWorldLayer::ccTouchesMoved(CCSet *pTouches, CCEvent *pEvent) {
    CCPoint location,pos[20];  
	int num=0;
	float endLen,scale;

    for (CCSetIterator iter = pTouches->begin(); iter != pTouches->end(); iter++)  {  
        CCTouch* pTouch = (CCTouch*)(*iter);  
		location = pTouch->getLocation();  
        pos[num++] = convertToNodeSpace(location);
    }//遍历取出每个触摸点坐标

	//当触摸点大于2时
	if(num >= 2) {
		 endLen = ccpDistance(pos[0],pos[1]);
		//负数为合拢，正数为分开
		float count = endLen - m_beginLen;
		scale = max(this->getScale()+count,1);
		this->setScale(scale);
	}

	m_beginLen = endLen;
	
}

void GameWorldLayer::ccTouchesEnded(CCSet *pTouches, CCEvent *pEvent) {
     CCPoint location,pos[20];  
	int num=0;
    for (CCSetIterator iter = pTouches->begin(); iter != pTouches->end(); iter++)  {  
        CCTouch* pTouch = (CCTouch*)(*iter);  
		location = pTouch->getLocation();  
        pos[num++] = convertToNodeSpace(location);
    }//遍历取出每个触摸点坐标  
    
	//单点触摸
	if(m_beginNum == 1)
		m_ActorManager->launch(pos[0]);
}

void GameWorldLayer::draw() {
	
    CCLayer::draw();
    ccGLEnableVertexAttribs( kCCVertexAttribFlag_Position );
    kmGLPushMatrix();
	m_ActorManager->getWorld()->DrawDebugData();
    kmGLPopMatrix();
    CHECK_GL_ERROR_DEBUG();
}