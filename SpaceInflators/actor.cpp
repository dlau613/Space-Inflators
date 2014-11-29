#include "actor.h"

#include "StudentWorld.h"
//ACTOR IMPLEMENTATION
Actor::Actor(StudentWorld* ptr, int id, int x, int y)//does every actor need a pointer?
	: GraphObject(id, x, y), m_alive(true), m_StudentWorld(ptr) 
{
	setVisible(true);
}

Actor::~Actor()
{}

void Actor::setAlive(bool m)
{
	m_alive = m;
}

bool Actor::isStillAlive()
{
	return m_alive;
}


void Actor::doSomething()
{}

StudentWorld* Actor::getWorld()
{
	//m_StudentWorld = ptrToShip;
	return m_StudentWorld;
}

//SHIP IMPLEMENTATION
Ship::Ship(StudentWorld* ptr, int id, int x, int y, int startEnergy)
	:Actor(ptr, id, x, y), m_energy(startEnergy)
{
}

int Ship::getEnergy() const
{
	return m_energy;
}

double Ship::getEnergyPct() const
{
	return m_energy * 2;
}

void Ship::decreaseEnergy(int amount)
{
	m_energy -= amount;
}

void Ship::restoreFullEnergy()
{
	m_energy = 50;
}

bool Ship::fire(int type, int firedBy)//alein(2) player(1)
{
	if (type != IID_BULLET && type != IID_TORPEDO)
		return false;
	if (firedBy != 1 && firedBy != 2)
		return false;
	if (firedBy == 1)
	{
		getWorld()->addActor(type, getX(), getY()+1, firedBy);
		if (type == IID_TORPEDO)
			getWorld()->playSound(SOUND_PLAYER_TORPEDO);
		else
			getWorld()->playSound(SOUND_PLAYER_FIRE);
	}
	if (firedBy == 2)
	{
		getWorld()->addActor(type, getX(), getY()-1, firedBy);

	}
	return true;
}



//PLAYER SHIP IMPLEMENTATION
PlayerShip::PlayerShip(StudentWorld* ptr)
	: Ship(ptr, IID_PLAYER_SHIP, VIEW_WIDTH/2, 1, 50), fired(false), m_torpedos(0)
{}

PlayerShip::~PlayerShip()
{}

int PlayerShip::getNumTorpedos() const
{
	return m_torpedos;
}

void PlayerShip::addTorpedos(int n)
{
	m_torpedos += n;
}

void PlayerShip::damage(int amount, bool hitByProjectile)
{
	decreaseEnergy(amount);
	if (getEnergy() <= 0)
	{
		setVisible(false);
		setAlive(false);
		getWorld()->playSound(SOUND_PLAYER_DIE);
	}
	else 
		getWorld()->playSound(SOUND_PLAYER_HIT);
}

void PlayerShip::checkForCollisionsWithAliens(PlayerShip* p)
{
	list<Alien*> collidedWith = getWorld()->getCollidingAliens(p);
	int count = 0;
	for (list<Alien*>::iterator q = collidedWith.begin(); q != collidedWith.end(); q++)
	{
		(*q)->damage(0, false);
		count++;
		if (count == 1)
			damage(15, false);
		getWorld()->playSound(SOUND_ENEMY_PLAYER_COLLISION);
	}
}

void PlayerShip::doSomething()
{//check for collision with aliens before it moves and after it moves
	//have goodies check for collisoins with player
	//projectiles check for collisions with players and aliens
	checkForCollisionsWithAliens(this);
	if (getEnergy() <= 0)
		return;
	int key;
	if (getWorld()->getKey(key))//WHY DOESNT IT WORK AS SMOOTHLY
	{
		//user hit a key this tick
		switch (key)
		{
		case KEY_PRESS_LEFT://move player to the left
			if (getX() > 0)
				moveTo(getX()-1, getY());
			break;
		case KEY_PRESS_RIGHT://move player to the right
			if (getX() < VIEW_WIDTH-1)
				moveTo(getX()+1, getY());
			break;
		case KEY_PRESS_UP://move player up
			if (getY()< VIEW_HEIGHT-1)
				moveTo(getX(), getY()+1);
			break;
		case KEY_PRESS_DOWN://move player down
			if (getY() > 0)
				moveTo(getX(), getY()-1);
			break;
		case KEY_PRESS_SPACE:
			if (fired)
				fired = false;
			else
			{
				if(getY() != VIEW_HEIGHT -1)
				{
					fire(IID_BULLET, 1);//fire and set fired to true
					fired = true;
				}
			}
			break;
		case KEY_PRESS_TAB:
			if (m_torpedos > 0)
			{
				if (fired)
					fired = false;
				else
				{
					if(getY() != VIEW_HEIGHT -1)
					{
						fire(IID_TORPEDO, 1);
						fired = true;
						m_torpedos--;
					}
				}
			}
			break;	
		}		
	}
	else 
		fired = false;
	return;//for (int i = 0; i < m_actors.size(); i++)
}

//STAR IMPLEMENTATION
Star::Star(StudentWorld* ptr)//constructor
	:Actor(ptr, IID_STAR, rand() % 30, VIEW_HEIGHT-1)
{
	
}

void Star::doSomething()
{
	if (getY() >= 0)
		moveTo(getX(), getY()-1);
	else
	{
		setVisible(false);
		setAlive(false);
		//StudentWorld move() should delete it
	}	
	return;
}

Star::~Star()
{}


//BULLET IMPLEMENTATION
Bullet::Bullet(StudentWorld* ptr, int bulletType, int x, int y, int firedBy, int damagePoints)
	: Actor(ptr, bulletType, x, y), m_bulletType(bulletType) , m_damagePoints(damagePoints), m_firedBy(firedBy)//1 is playership, 2 is alien
{}

Bullet::~Bullet()
{}

int Bullet::firedBy() const
{
	return m_firedBy;
}

void Bullet::checkForCollision(Bullet* b)
{
	if (m_firedBy == 1)
	{
		//TURN THIS INTO A FUNCTION 
		bool collided = false;
		list<Alien*> collidedWith = getWorld()->getCollidingAliens(b);
		for (list<Alien*>::iterator p = collidedWith.begin(); p != collidedWith.end(); p++)
		{
			(*p)->damage(m_damagePoints, true);
			collided = true;
		}
		if (collided == true)
		{
			setAlive(false);
			setVisible(false);
		}
	}
	if (m_firedBy == 2)
	{
		PlayerShip* collideWith = getWorld()->getCollidingPlayer(b);
		if (collideWith != NULL)
		{
			collideWith->damage(m_damagePoints, true);
			setAlive(false);
			setVisible(false);
		}
	}
}

void Bullet::doSomething()
{
	checkForCollision(this);
	////////////////////////
	if (m_firedBy == 1)
		if(getY() <= VIEW_HEIGHT-1)
			moveTo(getX(), getY()+1);
		else
		{
			setVisible(false);
			setAlive(false);
		}
	
	if (m_firedBy == 2)
		if(getY() >= 0)
			moveTo(getX(), getY()-1);
		else 
		{
			setVisible(false);
			setAlive(false);
		}
	////////////////////////
	checkForCollision(this);
	return;
	//check for collision
}
//SEPTIC BULLET IMPLEMENTATION
SepticBullet::SepticBullet(StudentWorld* ptr, int x, int y, int firedBy)
	: Bullet(ptr, IID_BULLET, x, y, firedBy, 2)
{}

SepticBullet::~SepticBullet()
{}

 //FLATULENCE TORPEDO IMPLEMENTATION
FlatulenceTorpedo::FlatulenceTorpedo(StudentWorld* ptr, int x, int y, int firedBy)
	: Bullet(ptr, IID_TORPEDO, x, y, firedBy, 8)
{}

FlatulenceTorpedo::~FlatulenceTorpedo()
{}

//ALIEN IMPLEMENTATION
Alien::Alien(StudentWorld* ptr, int id, int startEnergy, int worth)
	: Ship(ptr, id, rand() % 30, VIEW_HEIGHT-1, startEnergy) , m_Worth(worth), m_diedByCollision(true)//, m_justHit(false)
{}

Alien::~Alien()
{}

bool Alien::diedByCollision()
{
	return m_diedByCollision;
}
/*
void Alien::setJustHit(bool j)
{
	m_justHit = j;
}

bool Alien::getJustHit()
{
	return m_justHit;
}*/

void Alien::damage(int points, bool hitByProjectile)
{
	if (hitByProjectile == true)
	{
		decreaseEnergy(points);		
	}
	else
		decreaseEnergy(getEnergy());
	if (getEnergy() <= 0)
	{
		if (hitByProjectile == true)
		{
			getWorld()->increaseScore(m_Worth);
			m_diedByCollision = false;			
		}
		setAlive(false);
		getWorld()->playSound(SOUND_ENEMY_DIE);
		//////
		if (m_diedByCollision == false)
		{
			Smallbot* s = dynamic_cast<Smallbot*>(this);
			if (s != NULL)
				s->maybeDropGoodie();
			WealthyNachling* w = dynamic_cast<WealthyNachling*>(this);
			if (w != NULL)
				w->maybeDropGoodie();
		}
		//////
	}
	else
		getWorld()->playSound(SOUND_ENEMY_HIT);
	return;
}

void Alien::maybeDropGoodie()
{
	return;
}
//NACHLING BASE IMPLEMENTATION
NachlingBase::NachlingBase(StudentWorld* ptr, int id, int round, int worth)
	: Alien(ptr, id, round , worth), m_state(0), m_canAct(true)
{
}

NachlingBase::~NachlingBase()
{}

void NachlingBase::setCanAct(bool m)
{
	m_canAct = m;
}

void NachlingBase::doSomething()
{
	if (m_canAct == false)
	{
		m_canAct = true;
		return;
	}
	if (m_canAct != false)
	{
		m_canAct = ! m_canAct;
		if (m_state == 0)
		{
			if (getWorld()->getPlayerShip()->getX() != getX())
			{
				int p = rand() % 100;
				if (p >33)
				{
					if (getWorld()->getPlayerShip()->getX()>getX())
						moveTo(getX()+1,getY()-1);
					else
						moveTo(getX()-1,getY()-1);
					return;
				}
			}
			else
			{
				if (getX() != 0 || getX() != VIEW_WIDTH-1)
				{
					m_state = 1;
					int dr = 29 - getX();
					if (getX() < dr)
						m_MDB = getX();
					else 
						m_MDB = dr;
					if (m_MDB > 3)
						m_HMD = (rand() % 3) + 1;
					else
						m_HMD = m_MDB;
					int rando = rand() % 2;//0 left, 1 right
					if (rando == 0)
						m_randDir = -1;
					else
						m_randDir = 1;
					m_HMR = m_HMD;
				}
			}
			if (getY() >= 0)
				moveTo(getX(), getY()-1);
			else
			{
				setVisible(false);
				setAlive(false);
			}
			return;
		}
		if (m_state == 1)
		{
			if (getWorld()->getPlayerShip()->getY() > getY())
			{
				m_state = 2;
				return;
			}
			if (m_HMR == 0)
			{
				m_randDir = -1 * m_randDir;
				m_HMR = 2 * m_HMD;
			}
			else
				m_HMR--;
			if (m_randDir >0)
				moveTo(getX() + 1, getY());
			else 
				moveTo(getX() - 1, getY());
			int chancesOfFiring = int(10/getWorld()->getRoundNumber());
			int p1 = rand() % chancesOfFiring;
			if (p1 == 0)
			{
				int numAlienBullets = getWorld()->getNumAlienFiredProjectiles();
				if (numAlienBullets < getWorld()->getRoundNumber() * 2)
				{
					getWorld()->addActor(IID_BULLET, getX(), getY()-1, 2);
					getWorld()->playSound(SOUND_ENEMY_FIRE);
				}	
			}
			int p2 = rand() % 20;
			if (p2 == 0)
					m_state = 2;
			return;
		}
		if (m_state == 2)
		{
			if (getY() == VIEW_HEIGHT-1)
			{
				m_state = 0;
				return;
			}
			if (getX() == 0)
				m_randDir = 1;
			if (getX() == VIEW_WIDTH-1)
				m_randDir = -1;
			else 
			{
				int p = rand() % 2;
				if (p == 0)
					m_randDir = 1;
				else 
					m_randDir = -1;
			}
			moveTo(getX()+m_randDir, getY()+1);
			return;
		}
	}
}

//NACHLING IMPLEMENTATION
Nachling::Nachling(StudentWorld* ptr, int round)
	: NachlingBase(ptr, IID_NACHLING, (5 * ((round * .1) + .9)), 1000)
{}

Nachling::~Nachling()
{}


void Nachling::maybeDropGoodie()
{return;}

//WEALTHY NACHLING IMPLEMENTATION
WealthyNachling::WealthyNachling(StudentWorld* ptr, int round)
	: NachlingBase(ptr, IID_WEALTHY_NACHLING, 8 * ((round * .1) + .9), 1200), m_malfunctioning(false)
{}

WealthyNachling::~WealthyNachling()
{}

void WealthyNachling::doSomething()
{
	if (m_malfunctioning)
	{
		if (m_malfunctionTime == 0)
			m_malfunctioning = false;
		m_malfunctionTime--;
		return;
	}
	else 
	{
		int p = rand() % 200;
		if (p == 1)
		{
			m_malfunctioning = true;
			m_malfunctionTime = 30;
		}
	}
	NachlingBase::doSomething();
	setCanAct(true);
}

void WealthyNachling::maybeDropGoodie()
{
	int p = rand() % 100;
	if (p < 33)
	{
		int p2 = rand() % 100;
		if (p2 <50)
			getWorld()->addActor(IID_ENERGY_GOODIE, getX(), getY(), 0);
		else
			getWorld()->addActor(IID_TORPEDO_GOODIE, getX(), getY(), 0);
	}
	return;
}

//SMALL BOT IMPLEMENTATION
Smallbot::Smallbot(StudentWorld* ptr, int round)
	: Alien(ptr, IID_SMALLBOT, 12 * ((round * .1 + .9)), 1500), m_canAct(true), m_justHit(false)
{}

Smallbot::~Smallbot()
{}

void Smallbot::setJustHit(bool j)
{
	m_justHit = j;
}

void Smallbot::maybeDropGoodie()
{
	int p = rand() % 100;
	if (p <33)
		getWorld()->addActor(IID_FREE_SHIP_GOODIE, getX(), getY(), 0);
	return;
}

void Smallbot::damage(int points, bool hitByProjectile)
{
	Alien::damage(points, hitByProjectile);
	if (hitByProjectile == true)
		m_justHit = true;
}

void Smallbot::doSomething()
{
	if (m_canAct == false)
	{
		m_canAct = true;
		setJustHit(false);
		return;
	}
	else 
	{
		m_canAct = false;
		int p = rand() % 100;
		if (m_justHit == true)
		{
			if (p <33)
			{
				if (getX() == 0)
					moveTo(getX()+1,getY()-1);
				if (getX() == VIEW_WIDTH-1)
					moveTo(getX()-1,getY()-1);
				else
				{
					int p2 = rand() % 2;
					if (p2 ==0)//move left
						moveTo(getX()-1, getY()-1);
					else
						moveTo(getX()+1, getY()-1);
				}
			}
		}
		if (m_justHit == false || p >= 33)
		{
			moveTo(getX(), getY()-1);
		}
		if (getX() == getWorld()->getPlayerShip()->getX())
		{
			int numAlienBullets = getWorld()->getNumAlienFiredProjectiles();
			if (numAlienBullets < getWorld()->getRoundNumber() * 2)
			{
				int q = int((100.0/getWorld()->getRoundNumber()) + 1);
				int chanceFire = rand() % q;
				if (chanceFire == 0)
					getWorld()->addActor(IID_TORPEDO, getX(), getY()-1, 2);
				else
					getWorld()->addActor(IID_BULLET, getX(), getY()-1, 2);
				getWorld()->playSound(SOUND_ENEMY_FIRE);
			}
		}
		if(getY()<0)
		{
			setVisible(false);
			setAlive(false);
		}
	}
	setJustHit(false);
	return;
}

//GOODIE IMPLEMENTATION
Goodie::Goodie(StudentWorld* ptr, int id,  int x, int y)
	: Actor(ptr, id, x, y), m_ticksLeftToLive(double(100/getWorld()->getRoundNumber() +30)), m_goodieTickLifeTime(100/getWorld()->getRoundNumber() +30), m_actEvery3Ticks(0)
{}

Goodie::~Goodie()
{}

void Goodie::doSomething()
{
	PlayerShip* collidedWith = getWorld()->getCollidingPlayer(this);
	if (collidedWith != NULL)
		doSpecialAction(collidedWith);
	else
	{
		setBrightness((double(m_ticksLeftToLive)/m_goodieTickLifeTime) + .2);
		if (m_ticksLeftToLive == 0)
		{
			setVisible(false);
			setAlive(false);
			return;
		}
		m_actEvery3Ticks++;
		if (m_actEvery3Ticks == 3)
		{
			if (getY() >=0)
				moveTo(getX(), getY()-1);
			else
			{
				setVisible(false);
				setAlive(false);
				return;
			}
			m_actEvery3Ticks = 0;
		}
	}
	m_ticksLeftToLive--;
	collidedWith = getWorld()->getCollidingPlayer(this);
	if (collidedWith != NULL)
		doSpecialAction(collidedWith);

}

//ENERGY GOODIE IMPLEMENTATION
EnergyGoodie::EnergyGoodie(StudentWorld* ptr, int x, int y)
	: Goodie(ptr, IID_ENERGY_GOODIE, x, y)
{}

EnergyGoodie::~EnergyGoodie()
{}

void EnergyGoodie::doSpecialAction(PlayerShip* p)
{
	p->getWorld()->playSound(SOUND_GOT_GOODIE);
	p->getWorld()->increaseScore(5000);
	p->restoreFullEnergy();
	setVisible(false);
	setAlive(false);
}

//TORPEDO GOODIE IMPLEMENTATION
TorpedoGoodie::TorpedoGoodie(StudentWorld* ptr, int x, int y)
	: Goodie(ptr, IID_TORPEDO_GOODIE, x, y)
{}

TorpedoGoodie::~TorpedoGoodie()
{}

void TorpedoGoodie::doSpecialAction(PlayerShip* p)
{
	p->getWorld()->playSound(SOUND_GOT_GOODIE);
	p->getWorld()->increaseScore(5000);
	p->addTorpedos(5);
	setVisible(false);
	setAlive(false);
}

//FREE SHIP GOODIE IMPLEMENTATION
FreeShipGoodie::FreeShipGoodie(StudentWorld* ptr, int x, int y)
	: Goodie(ptr, IID_FREE_SHIP_GOODIE, x, y)
{}

FreeShipGoodie::~FreeShipGoodie()
{}

void FreeShipGoodie::doSpecialAction(PlayerShip* p)
{
	p->getWorld()->increaseScore(5000);
	p->getWorld()->playSound(SOUND_GOT_GOODIE);
	p->getWorld()->incLives();
	setVisible(false);
	setAlive(false);
}

// Students:  Add code to this file (if you wish), actor.h, StudentWorld.h, and StudentWorld.cpp