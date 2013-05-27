#include "Actor.h"
#include "ActorManager.h"
//------------------------------------------------------Actor基类
Actor::Actor()
	:m_body(NULL),
	m_shape(NULL),
	m_type(-1),
	m_deltaMass(0)
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
	bodyDef.userData = (Actor *)this;		//以便在碰撞检测找到

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


float MoveActor::swallow() {
	float mass = m_deltaMass;
	m_deltaMass = 0;

	if(mass>0) {											//吞噬
		setMass(this->getMass()+mass);
	}else if(mass<0) {									//被吞噬
		//减少的质量不得大于自身的质量
		mass = min(mass, this->getMass());
		setMass(this->getMass()-mass);
	}else {													//什么都不做

	}

	return mass;
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
	if(this->getRadius() <= this->getPushRadius()) {
		return NULL;
	}

	CCPoint bodyPos = m_shape->getPosition();		//角色位置
	b2Vec2 dir = b2Vec2(touchPos.x-bodyPos.x, touchPos.y-bodyPos.y);		//施加力方向
	dir.Normalize();	//将向量标准化

	//创建喷射物质
	float radius = this->getRadius()+this->getPushRadius();
	CCPoint launchPos = ccp(dir.x*radius+bodyPos.x,dir.y*radius+bodyPos.y);
	CCLayer *layer = (CCLayer *)m_shape->getParent();

	MoveActor *launchActor = (MoveActor *)ActorManager::shareActorManager()->createActor(Actor::AT_Move, layer, launchPos, this->getPushRadius());
	//计算喷射物质速度
	b2Vec2 launchVelocity = b2Vec2(dir.x*UNIT_SPEED,dir.y*UNIT_SPEED);
	float launchMass = launchActor->getMass();    //喷射物质质量
	float newMass = this->getMass()-launchMass;			   //喷射后角色质量
	//计算喷射物质反冲量 = - launchVelocity*launchMass
	b2Vec2 impluse = b2Vec2(-launchVelocity.x*launchMass, -launchVelocity.y*launchMass);
	this->setMass(newMass);		//设置质量

	//为控制角色施加冲量
	this->getBody()->ApplyLinearImpulse(impluse,m_body->GetPosition());
	//为喷射物设置速度
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

//方形区域
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

	//-----------------设置类型
	m_type = Actor::AT_Wall;
}

//圆形区域
WallActor::WallActor(b2World *world, CCLayer *layer, CCPoint pos, float radius) {
	//..................
}


SingalWallActor::SingalWallActor(b2World *world, CCLayer *layer, CCPoint pos, CCSize size) {
	//-----------------设置物理模型
	b2BodyDef bodyDef;
	bodyDef.position.Set(pos.x/PTM_RATIO,pos.y/PTM_RATIO);
	bodyDef.type =  b2_staticBody;
	bodyDef.userData = (Actor *)this;				//以便在碰撞检测找到

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

}

//--------------------------------------------------------------SunActor 

SunActor::SunActor(b2World *world, CCLayer *layer, CCPoint pos, float radius, float gravRadius) {

}