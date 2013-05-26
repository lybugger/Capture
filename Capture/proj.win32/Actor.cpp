#include "Actor.h"
#include "ActorManager.h"
//------------------------------------------------------Actor基类
Actor::Actor()
	:m_body(NULL),
	m_shape(NULL),
	m_type(-1)
 {
	//自动产生id
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
//构造函数
MoveActor::MoveActor(b2World *world, CCLayer *layer, CCPoint pos, float radius) {
	//-----------------设置物理模型
	b2BodyDef bodyDef;
	bodyDef.position.Set(pos.x/PTM_RATIO,pos.y/PTM_RATIO);
	bodyDef.type =  b2_dynamicBody;

	//创建box
	b2CircleShape circleShape;
	circleShape.m_radius = radius/PTM_RATIO;
	b2Body *circle = world->CreateBody(&bodyDef);

	//设置夹具
	b2FixtureDef fixtureDef;
	fixtureDef.density = 10;			//密度
	fixtureDef.friction = 0.0f;			//滑动摩擦
	fixtureDef.restitution = 1.0f;		//弹性体复原
	fixtureDef.shape = &circleShape;
	//fixtureDef.filter.groupIndex = -1;
	circle->CreateFixture(&fixtureDef);
	m_body = circle;

	//---------------设置显示模型
	CCSprite *shape = CCSprite::create();
	shape->setTextureRect(CCRect(0,0, radius*2, radius*2));
	shape->retain();
	layer->addChild(shape);
	m_shape = shape;

	//-----------------设置类型
	m_type = Actor::AT_Move;
	
}

//被吃
float MoveActor::reduce(float rMass) {
	//减少的质量不得大于自身的质量
	float mass = min(rMass,this->getMass());
	setMass(this->getMass()-mass);

	return mass;
}

//吞噬
float MoveActor::increase(float iMass) {
	//增加质量
	setMass(this->getMass()+iMass);
	return iMass;
}


//--------------------------------------------------------------ControlActor
ControlActor::ControlActor(b2World *world, CCLayer *layer, CCPoint pos, float radius)
		:MoveActor(world, layer, pos, radius)
{
	//-----------------设置类型
	m_type = Actor::AT_Control;
}

//角色抛射物质
Actor *ControlActor::launch(CCPoint touchPos) {
	//判断角色大小为单位大小
	if(m_body->GetFixtureList()->GetShape()->m_radius*PTM_RATIO <= UNIT_RADIUS+20) {
		return NULL;
	}

	CCPoint bodyPos = m_shape->getPosition();		//角色位置
	b2Vec2 dir = b2Vec2(touchPos.x-bodyPos.x, touchPos.y-bodyPos.y);		//施加力方向
	dir.Normalize();	//将向量标准化

	//创建喷射物质
	float radius = m_body->GetFixtureList()->GetShape()->m_radius*PTM_RATIO+UNIT_RADIUS;
	CCPoint launchPos = ccp(dir.x*radius+bodyPos.x,dir.y*radius+bodyPos.y);
	CCLayer *layer = (CCLayer *)m_shape->getParent();

	MoveActor *launchActor = (MoveActor *)ActorManager::shareActorManager()->createActor(Actor::AT_Move, layer, launchPos,UNIT_RADIUS);
	//计算喷射物质速度
	b2Vec2 launchVelocity = b2Vec2(dir.x*UNIT_SPEED,dir.y*UNIT_SPEED);
	float launchMass = launchActor->getMass();    //喷射物质质量
	float newMass = this->getMass()-launchMass;			   //喷射后角色质量
	//计算喷射物质反冲量 = - launchVelocity*launchMass
	b2Vec2 impluse = b2Vec2(-launchVelocity.x*launchMass, -launchVelocity.y*launchMass);
	this->setMass(newMass);		//设置质量

	//为控制角色施加冲量
	//this->getBody()->ApplyLinearImpulse(impluse,m_body->GetPosition());
	//为喷射物设置速度
	//launchActor->getBody()->SetLinearVelocity(launchVelocity);
	return launchActor;
}


//--------------------------------------------------------------WallActor
WallActor::WallActor(b2World *world, CCLayer *layer, CCPoint pos, CCSize size) {
	//-----------------设置物理模型
	b2BodyDef bodyDef;
	bodyDef.position.Set(pos.x/PTM_RATIO,pos.y/PTM_RATIO);
	bodyDef.type =  b2_staticBody;

	//创建box
	b2PolygonShape boxShape;
	boxShape.SetAsBox(size.width/PTM_RATIO/2, size.height/PTM_RATIO/2);
	b2Body *box = world->CreateBody(&bodyDef);

	//设置夹具
	b2FixtureDef fixtureDef;
	fixtureDef.density = 10;			//密度
	fixtureDef.friction = 0.0f;			//滑动摩擦
	fixtureDef.restitution = 1.0f;		//弹性体复原
	fixtureDef.shape = &boxShape;
	box->CreateFixture(&fixtureDef);
	m_body = box;

	//-----------------设置显示模型
	//为box披上外衣
	CCSprite *shape = CCSprite::create();
	shape->setTextureRect(CCRect(0,0, size.width,size.height));
	shape->retain();
	layer->addChild(shape);
	m_shape = shape;

	//-----------------设置类型
	m_type = Actor::AT_Wall;
}