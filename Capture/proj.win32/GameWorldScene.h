#ifndef GAMEWORLDSCENE_H
#define GAMEWORLDSCENE_H

#include "Box2D/Box2D.h"
#include "cocos2d.h"
USING_NS_CC;

class ActorManager;
class GameWorldLayer : public CCLayer {
public:
	GameWorldLayer() ;
	~GameWorldLayer() {}
	static GameWorldLayer* create(int level)  {
		GameWorldLayer *pRet = new GameWorldLayer (); 
		if (pRet && pRet->init(level)) { 
			pRet->autorelease();
		}else {
			delete pRet;
			pRet = NULL;
		}
		return pRet;
	}
	virtual bool init(int level);
    virtual void ccTouchesBegan(CCSet *pTouches,CCEvent *pEvent);
    virtual void ccTouchesMoved(CCSet *pTouches, CCEvent *pEvent);
    virtual void ccTouchesEnded(CCSet *pTouches, CCEvent *pEvent);
	void draw();
	void update(float dt); 

private:
	ActorManager *m_ActorManager;
	CCRect m_worldRect;  //�������
	int m_level;	//�ȼ���
	int m_beginNum; //�����ĵ���
	float m_beginLen;  //��ָ��ľ���
};




//-------------------------------------------------------game world scene
class GameWorldScene : public CCScene {
public:
	virtual bool init(int level) {this->addChild(GameWorldLayer::create(level));return true;}
	static GameWorldScene* create(int level)  {
		GameWorldScene *pRet = new GameWorldScene (); 
		if (pRet && pRet->init(level)) { 
			pRet->autorelease();
		}else {
			delete pRet;
			pRet = NULL;
		}
		return pRet;
	}
};

#endif