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

void Actor::update(float dt) {
		//------------------���� actor shape
		if(m_shape != NULL) {
			//����λ��
			m_shape->setPosition(ccp(m_body->GetPosition().x*PTM_RATIO,m_body->GetPosition().y*PTM_RATIO));
			//���½Ƕ�
			m_shape->setRotation(-1*CC_RADIANS_TO_DEGREES(m_body->GetAngle()));
			//���´�С
			m_shape->setScaleX(m_body->GetFixtureList()->GetShape()->m_radius*PTM_RATIO*2/
				m_shape->getTexture()->getContentSize().width);
			m_shape->setScaleY(m_body->GetFixtureList()->GetShape()->m_radius*PTM_RATIO*2/
				m_shape->getTexture()->getContentSize().width);
		}
}


void Actor::createCircle(b2World *world, CCLayer *layer, CCPoint pos, float radius, bool isStatic) {
	//-----------------��������ģ��
	b2BodyDef bodyDef;
	bodyDef.position.Set(pos.x/PTM_RATIO,pos.y/PTM_RATIO);
	bodyDef.type = isStatic?b2_staticBody : b2_dynamicBody;
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
	createShape(layer,CCSize(radius*2,radius*2));
}


void Actor::createRectangle(b2World *world, CCLayer *layer, CCPoint pos, CCSize size, bool isStatic) {
	//-----------------��������ģ��
	b2BodyDef bodyDef;
	bodyDef.position.Set(pos.x/PTM_RATIO,pos.y/PTM_RATIO);
	bodyDef.type = isStatic?b2_staticBody : b2_dynamicBody;
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
	createShape(layer,size);
}


//-----------------------------------------------------------MoveActor
//���캯��
MoveActor::MoveActor(b2World *world, CCLayer *layer, CCPoint pos, float radius)
	:	m_deltaMass(0)
{
	
	createCircle(world, layer, pos, radius);
	//-----------------��������
	m_type = Actor::AT_Move;
	
}

void MoveActor::update(float dt) {
	//���ø���
	Actor::update(dt);
	//-------------------��������
	this->swallow();
}

void MoveActor::dealContact(Actor* actor) {
	MoveActor *moveActor = static_cast<MoveActor *>(actor);
	ActorManager *actorManager = ActorManager::shareActorManager();
	//Ԥ�����ͬʱ��ײ
	if(this->getDeltaMass() != 0 || moveActor->getDeltaMass() !=0) return;

	if(this->getMass() > moveActor->getMass()) {
		this->setDeltaMass(moveActor->getMass());
		this->setDeltaMass(-moveActor->getMass());
		actorManager->clearActor(moveActor);
	}else {
		moveActor->setDeltaMass(this->getMass());
		moveActor->setDeltaMass(-this->getMass());
		actorManager->clearActor(this);
	}
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

void MoveActor::createShape(CCLayer *layer, CCSize size) {
	//Ϊbox��������
	CCSprite *shape = CCSprite::create("ball.png");
	//shape->setTextureRect(CCRect(0,0, size.width,size.height));
	shape->setScaleX(size.width/shape->getTexture()->getContentSize().width);
	shape->setScaleY(size.height/shape->getTexture()->getContentSize().width);
	//shape->retain();

	  CCParticleFlower* sun = CCParticleFlower::create();
    sun->setTexture(CCTextureCache::sharedTextureCache()->addImage("fire.png"));
	sun->setScale(4);
	sun->setPosition(shape->getContentSize().width/2,shape->getContentSize().height/2);
    sun->setTotalParticles(130);
    sun->setLife(0.6f);
	shape->addChild(sun);


	layer->addChild(shape);
	m_shape = shape;
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
	if(this->getRadius() <= this->getPushRadius()*3) {
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
	b2Vec2 launchVelocity = b2Vec2(dir.x*UNIT_SPEED,dir.y*UNIT_SPEED)+m_body->GetLinearVelocity();
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
	createRectangle(world,layer,pos,size,true);
	//-----------------��������
	m_type = Actor::AT_SingleWall;
}

//--------------------------------------------------------------SunActor 

SunActor::SunActor(b2World *world, CCLayer *layer, CCPoint pos, float radius, float gravRadius) {
	//----------------------------------------------------------------------------̫��ʵ��
	createCircle(world,layer,pos,radius,true);
	m_sunMass = 20000;
	m_G  = 0.1f;
	m_gravRadius = gravRadius;
	//--------------------------------------------------------------------��������
	setGravityAndVelocity();
	
	//-----------------��������
	m_type = Actor::AT_Sun;
}

void SunActor::update(float dt) {
	
	Actor::update(dt);
	ActorManager *actorManager = ActorManager::shareActorManager();
	list<Actor *> *actors = actorManager->getActors();
	b2Vec2 gravity;

	for(list<Actor *>::iterator itr = actors->begin();itr!=actors->end();itr++) {
		gravity  = getGravityFromActor(*itr);
		(*itr)->setGravity(gravity);
		(*itr)->getBody()->ApplyForce(gravity,m_body->GetPosition());
	}


}

void SunActor::dealContact(Actor* actor) {
	MoveActor *moveActor = static_cast<MoveActor *>(actor);
	ActorManager *actorManager = ActorManager::shareActorManager();
	//Ԥ�����ͬʱ��ײ
	if(moveActor->getDeltaMass() !=0) return;

	moveActor->setDeltaMass(-moveActor->getMass());
	actorManager->clearActor(moveActor);

}

void SunActor::setGravityAndVelocity() {
	ActorManager *actorManager = ActorManager::shareActorManager();
	list<Actor *> *actors = actorManager->getActors();
	b2Vec2 gravity, velocity;

	for(list<Actor *>::iterator itr = actors->begin();itr!=actors->end();itr++) {
		gravity  = getGravityFromActor(*itr);
		(*itr)->setGravity(gravity);
		velocity = getVelocityFromActor(*itr);
		(*itr)->getBody()->ApplyForce(gravity,m_body->GetPosition());
		(*itr)->getBody()->SetLinearVelocity(velocity);
	}

}


b2Vec2 SunActor::getGravityFromActor(Actor *actor) {
	/*
		//����������ʽF=GMm/(R^2)
	//��ΪsunΪ��̬û������Ĭ��Ϊ100000��G��Ϊ0.0001
	b2Body *body = actor->getBody();
	b2Vec2 v = body->GetPosition();
	b2Vec2 force;
	float F;
	v -= this->getBody()->GetPosition();
	F = m_G*m_sunMass*body->GetMass()/(v.LengthSquared());
	v.Normalize();
	force = b2Vec2(-v.x*F,-v.y*F);
	*/
	b2Body *body = actor->getBody();
	b2Vec2 planetDistance = this->getBody()->GetPosition();
	planetDistance -= body->GetPosition();
	float finalDistance = planetDistance.Length();
	b2Vec2 force=b2Vec2(0,0);

	if(finalDistance <= m_gravRadius) {
		float F = m_G*m_sunMass*body->GetMass()/(planetDistance.LengthSquared());
		planetDistance.Normalize();
		force = b2Vec2(planetDistance.x*F,planetDistance.y*F);
	}

	return force;
}


b2Vec2 SunActor::getVelocityFromActor(Actor *actor) {
	//�����˶�F=m��^2r=mv^2/r    =>   v = sqrt(F*r/m)

	b2Body *body = actor->getBody();
	b2Vec2 v = body->GetPosition();
	b2Vec2 velocity;
	v -= this->getBody()->GetPosition();
	float speed = sqrt(actor->getGravity().Length()*v.Length()/actor->getBody()->GetMass());
	//����Ϊ˳ʱ�뷽��
	v.Normalize();


	velocity = b2Vec2(-v.y*speed,v.x*speed);
	return velocity;
}

