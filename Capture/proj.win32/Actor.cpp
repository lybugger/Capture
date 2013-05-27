#include "Actor.h"
#include "ActorManager.h"
//------------------------------------------------------Actor����
Actor::Actor()
	:m_body(NULL),
	m_shape(NULL),
	m_type(-1),
	m_deltaMass(0)
 {
	//�Զ�����id
	static int id=0;
	m_id = id++;
}

Actor::~Actor() {
	if(m_shape) {
		m_shape->getParent()->removeChild(m_shape);
		m_shape=NULL;
	}
	if(m_body) {
		m_body->GetWorld()->DestroyBody(m_body);
		m_body=NULL;
	}
}


//-----------------------------------------------------------MoveActor
//���캯��
MoveActor::MoveActor(b2World *world, CCLayer *layer, CCPoint pos, float radius) {
	//-----------------��������ģ��
	b2BodyDef bodyDef;
	bodyDef.position.Set(pos.x/PTM_RATIO,pos.y/PTM_RATIO);
	bodyDef.type =  b2_dynamicBody;
	bodyDef.userData = (Actor *)this;		//�Ա�����ײ����ҵ�

	//����box
	b2CircleShape circleShape;
	circleShape.m_radius = radius/PTM_RATIO;
	b2Body *circle = world->CreateBody(&bodyDef);

	//���üо�
	b2FixtureDef fixtureDef;
	fixtureDef.density = 10;			//�ܶ�
	fixtureDef.friction = 0.0f;			//����Ħ��
	fixtureDef.restitution = 1.0f;		//�����帴ԭ
	fixtureDef.shape = &circleShape;
	//fixtureDef.filter.groupIndex = -1;
	circle->CreateFixture(&fixtureDef);
	m_body = circle;

	//---------------������ʾģ��
	CCSprite *shape = CCSprite::create();
	shape->setTextureRect(CCRect(0,0, radius*2, radius*2));
	shape->retain();
	layer->addChild(shape);
	m_shape = shape;

	//-----------------��������
	m_type = Actor::AT_Move;
	
}


float MoveActor::swallow() {
	float mass = m_deltaMass;
	m_deltaMass = 0;

	if(mass>0) {											//����
		setMass(this->getMass()+mass);
	}else if(mass<0) {									//������
		//���ٵ��������ô������������
		mass = min(mass, this->getMass());
		setMass(this->getMass()-mass);
	}else {													//ʲô������

	}

	return mass;
}


//--------------------------------------------------------------ControlActor
ControlActor::ControlActor(b2World *world, CCLayer *layer, CCPoint pos, float radius)
		:MoveActor(world, layer, pos, radius)
{
	//-----------------��������
	m_type = Actor::AT_Control;
}

//��ɫ��������
Actor *ControlActor::launch(CCPoint touchPos) {
	//�жϽ�ɫ��СΪ��λ��С
	if(this->getRadius() <= this->getPushRadius()) {
		return NULL;
	}

	CCPoint bodyPos = m_shape->getPosition();		//��ɫλ��
	b2Vec2 dir = b2Vec2(touchPos.x-bodyPos.x, touchPos.y-bodyPos.y);		//ʩ��������
	dir.Normalize();	//��������׼��

	//������������
	float radius = this->getRadius()+this->getPushRadius();
	CCPoint launchPos = ccp(dir.x*radius+bodyPos.x,dir.y*radius+bodyPos.y);
	CCLayer *layer = (CCLayer *)m_shape->getParent();

	MoveActor *launchActor = (MoveActor *)ActorManager::shareActorManager()->createActor(Actor::AT_Move, layer, launchPos, this->getPushRadius());
	//�������������ٶ�
	b2Vec2 launchVelocity = b2Vec2(dir.x*UNIT_SPEED,dir.y*UNIT_SPEED);
	float launchMass = launchActor->getMass();    //������������
	float newMass = this->getMass()-launchMass;			   //������ɫ����
	//�����������ʷ����� = - launchVelocity*launchMass
	b2Vec2 impluse = b2Vec2(-launchVelocity.x*launchMass, -launchVelocity.y*launchMass);
	this->setMass(newMass);		//��������

	//Ϊ���ƽ�ɫʩ�ӳ���
	this->getBody()->ApplyLinearImpulse(impluse,m_body->GetPosition());
	//Ϊ�����������ٶ�
	launchActor->getBody()->SetLinearVelocity(launchVelocity);
	return launchActor;
}


//--------------------------------------------------------------WallActor
WallActor::~WallActor() {
	for(list<SingalWallActor *>::iterator itr = m_wall.begin();itr!=m_wall.end();) {
		delete *itr;
		itr = m_wall.erase(itr);
	}
}

//��������
WallActor::WallActor(b2World *world, CCLayer *layer, CCPoint pos, CCSize size) {
	int thick = 20;
	m_wall.push_back(
		new SingalWallActor(world, layer, CCPoint(pos.x + size.width/2, pos.y + size.height),CCSize(size.width,thick)));
	m_wall.push_back(
		new SingalWallActor(world, layer, CCPoint(pos.x + size.width/2, pos.y),CCSize(size.width,thick)));
	m_wall.push_back(
		new SingalWallActor(world, layer, CCPoint(pos.x, pos.y + size.height/2),CCSize(thick,size.height)));
	m_wall.push_back(
		new SingalWallActor(world, layer, CCPoint(pos.x + size.width, pos.y + size.height/2),CCSize(thick,size.height)));

	//-----------------��������
	m_type = Actor::AT_Wall;
}

//Բ������
WallActor::WallActor(b2World *world, CCLayer *layer, CCPoint pos, float radius) {
	//..................
}


SingalWallActor::SingalWallActor(b2World *world, CCLayer *layer, CCPoint pos, CCSize size) {
	//-----------------��������ģ��
	b2BodyDef bodyDef;
	bodyDef.position.Set(pos.x/PTM_RATIO,pos.y/PTM_RATIO);
	bodyDef.type =  b2_staticBody;
	bodyDef.userData = (Actor *)this;				//�Ա�����ײ����ҵ�

	//����box
	b2PolygonShape boxShape;
	boxShape.SetAsBox(size.width/PTM_RATIO/2, size.height/PTM_RATIO/2);
	b2Body *box = world->CreateBody(&bodyDef);

	//���üо�
	b2FixtureDef fixtureDef;
	fixtureDef.density = 10;			//�ܶ�
	fixtureDef.friction = 0.0f;			//����Ħ��
	fixtureDef.restitution = 1.0f;		//�����帴ԭ
	fixtureDef.shape = &boxShape;
	box->CreateFixture(&fixtureDef);
	m_body = box;

	//-----------------������ʾģ��
	//Ϊbox��������
	CCSprite *shape = CCSprite::create();
	shape->setTextureRect(CCRect(0,0, size.width,size.height));
	shape->retain();
	layer->addChild(shape);
	m_shape = shape;

}

//--------------------------------------------------------------SunActor 

SunActor::SunActor(b2World *world, CCLayer *layer, CCPoint pos, float radius, float gravRadius) {

}