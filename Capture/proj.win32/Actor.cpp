#include "Actor.h"
#include "ActorManager.h"
//------------------------------------------------------Actor����
Actor::Actor()
	:m_body(NULL),
	m_shape(NULL),
	m_type(-1)
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

//����
float MoveActor::reduce(float rMass) {
	//���ٵ��������ô������������
	float mass = min(rMass,this->getMass());
	setMass(this->getMass()-mass);

	return mass;
}

//����
float MoveActor::increase(float iMass) {
	//��������
	setMass(this->getMass()+iMass);
	return iMass;
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
	if(m_body->GetFixtureList()->GetShape()->m_radius*PTM_RATIO <= UNIT_RADIUS+20) {
		return NULL;
	}

	CCPoint bodyPos = m_shape->getPosition();		//��ɫλ��
	b2Vec2 dir = b2Vec2(touchPos.x-bodyPos.x, touchPos.y-bodyPos.y);		//ʩ��������
	dir.Normalize();	//��������׼��

	//������������
	float radius = m_body->GetFixtureList()->GetShape()->m_radius*PTM_RATIO+UNIT_RADIUS;
	CCPoint launchPos = ccp(dir.x*radius+bodyPos.x,dir.y*radius+bodyPos.y);
	CCLayer *layer = (CCLayer *)m_shape->getParent();

	MoveActor *launchActor = (MoveActor *)ActorManager::shareActorManager()->createActor(Actor::AT_Move, layer, launchPos,UNIT_RADIUS);
	//�������������ٶ�
	b2Vec2 launchVelocity = b2Vec2(dir.x*UNIT_SPEED,dir.y*UNIT_SPEED);
	float launchMass = launchActor->getMass();    //������������
	float newMass = this->getMass()-launchMass;			   //������ɫ����
	//�����������ʷ����� = - launchVelocity*launchMass
	b2Vec2 impluse = b2Vec2(-launchVelocity.x*launchMass, -launchVelocity.y*launchMass);
	this->setMass(newMass);		//��������

	//Ϊ���ƽ�ɫʩ�ӳ���
	//this->getBody()->ApplyLinearImpulse(impluse,m_body->GetPosition());
	//Ϊ�����������ٶ�
	//launchActor->getBody()->SetLinearVelocity(launchVelocity);
	return launchActor;
}


//--------------------------------------------------------------WallActor
WallActor::WallActor(b2World *world, CCLayer *layer, CCPoint pos, CCSize size) {
	//-----------------��������ģ��
	b2BodyDef bodyDef;
	bodyDef.position.Set(pos.x/PTM_RATIO,pos.y/PTM_RATIO);
	bodyDef.type =  b2_staticBody;

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

	//-----------------��������
	m_type = Actor::AT_Wall;
}