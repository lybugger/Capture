#ifndef Actor_H
#define Actor_H

#include "Box2D/Box2D.h"
#include "cocos2d.h"
USING_NS_CC;

#define PTM_RATIO 32
#define UNIT_MASS 3.0679617f
#define UNIT_AREA 314.15926f
#define UNIT_RADIUS 10
#define UNIT_SPEED 20

//----------------------------------------------Actor基类
class Actor {
public:
	enum ActorType {
		AT_Move,
		AT_Control,
		AT_Wall
	};

public:
	Actor();
	~Actor();
	b2Body *getBody() {return m_body;}
	void setBody(b2Body *body) {m_body = body;}
	CCSprite *getShape() {return m_shape;}
	void setShape(CCSprite *shape) {m_shape = shape;}
	int getType() {return m_type;}
	void setType(int type) {m_type=type;}
	int getId() {return m_id;}
	void setId(int id) {m_id = id;}

protected:
	b2Body *m_body;		//物理模型
	CCSprite *m_shape;	//显示模型
	int m_type;				//类型
	int m_id;					//唯一标示符
};


//-----------------------------------------------------------MoveActor
class MoveActor : public Actor{
public:
	MoveActor(b2World *world, CCLayer *layer, CCPoint pos, float radius);
	virtual float reduce(float rMass);
	virtual float increase(float iMass);
	float getMass() {return m_body->GetMass();}
	void setMass(float mass) {m_body->GetFixtureList()->GetShape()->m_radius *= mass/this->getMass();}
};


//-----------------------------------------------------------WallActor
class WallActor : public Actor{
public:
	WallActor(b2World *world, CCLayer *layer, CCPoint pos, CCSize size);
};


//----------------------------------------------------------ControlActor
class ControlActor : public MoveActor{
public:
	ControlActor(b2World *world, CCLayer *layer, CCPoint pos, float radius);
	Actor *launch(CCPoint touchPos);
};

#endif
