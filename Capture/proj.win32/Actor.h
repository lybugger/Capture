#ifndef Actor_H
#define Actor_H

#include "Box2D/Box2D.h"
#include "cocos2d.h"
#include <list>
USING_NS_CC;

#define PTM_RATIO 32
#define UNIT_SPEED 20

//----------------------------------------------Actor����
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
	b2Body *m_body;		//����ģ��
	CCSprite *m_shape;	//��ʾģ��
	int m_type;				//����
	int m_id;					//Ψһ��ʾ��
	float m_deltaMass;		//��������
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
		//�ı�����
		m_body->GetFixtureList()->GetShape()->m_radius *= sqrt(mass/this->getMass());
		//�ı�����
		b2MassData massData;
		m_body->GetMassData(&massData);
		massData.mass = mass;
		m_body->SetMassData(&massData);
	}
	float getRadius() {return m_body->GetFixtureList()->GetShape()->m_radius*PTM_RATIO;}
	void setRadius(float radius) {m_body->GetFixtureList()->GetShape()->m_radius = radius/PTM_RATIO;}
	//����������������1С��50��
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
