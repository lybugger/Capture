#ifndef Actor_H
#define Actor_H

#include "Box2D/Box2D.h"
#include "cocos2d.h"
#include <list>
USING_NS_CC;

#define PTM_RATIO 32
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
	virtual ~Actor();
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
	float m_deltaMass;		//质量增量
};


//-----------------------------------------------------------MoveActor
class MoveActor : public Actor{
public:
	MoveActor(b2World *world, CCLayer *layer, CCPoint pos, float radius);
	virtual float swallow();
	void setDeltaMass(float mass) {m_deltaMass = mass;}
	float getDeltaMass() {return m_deltaMass;}
	float getMass() {return m_body->GetMass();}
	void setMass(float mass) {	
		//改变外形
		m_body->GetFixtureList()->GetShape()->m_radius *= sqrt(mass/this->getMass());
		//改变质量
		b2MassData massData;
		m_body->GetMassData(&massData);
		massData.mass = mass;
		m_body->SetMassData(&massData);
	}
	float getRadius() {return m_body->GetFixtureList()->GetShape()->m_radius*PTM_RATIO;}
	void setRadius(float radius) {m_body->GetFixtureList()->GetShape()->m_radius = radius/PTM_RATIO;}
	//抛射物质质量大于1小于50；
	float getPushMass() {
		float mass = max(1,this->getMass()/50);
		mass = min(mass,50);
		return mass;
	}
	float getPushRadius() {
		float radius = sqrt(this->getPushMass()/this->getMass())*this->getRadius();
		return radius;
	}

};


//-----------------------------------------------------------WallActor
class SingalWallActor : public Actor {
public:
	SingalWallActor(b2World *world, CCLayer *layer, CCPoint pos, CCSize size);
};

class WallActor : public Actor {
public:
	virtual ~WallActor();
	WallActor(b2World *world, CCLayer *layer, CCPoint pos, CCSize size);
	WallActor(b2World *world, CCLayer *layer, CCPoint pos, float radius);
protected:
	std::list<SingalWallActor *> m_wall;
};


//----------------------------------------------------------ControlActor
class ControlActor : public MoveActor {
public:
	ControlActor(b2World *world, CCLayer *layer, CCPoint pos, float radius);
	Actor *launch(CCPoint touchPos);
};


//-----------------------------------------------------------SunActor
class SunActor : public Actor {
public:
	SunActor(b2World *world, CCLayer *layer, CCPoint pos, float radius, float gravRadius);


protected:
	float m_gravity;
	float m_gravRadius;
}


#endif
