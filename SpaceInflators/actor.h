#ifndef _ACTOR_H_
#define _ACTOR_H_

#include "GraphObject.h"
class StudentWorld;

// Students:  Add code to this file, actor.cpp, StudentWorld.h, and StudentWorld.cpp

class Actor : public GraphObject
{
public:
	Actor(StudentWorld* ptr, int id, int x, int y);
	virtual ~Actor();
	virtual void doSomething() = 0;
	bool isStillAlive();	
	StudentWorld* getWorld();
	void setAlive(bool m);
	
private:
	bool m_alive;
	StudentWorld* m_StudentWorld;//pg23 hint at bottom
};

class Ship : public Actor
{
public:
	Ship(StudentWorld* ptr, int id, int x, int y, int startEnergy);
	int getEnergy() const;
	double getEnergyPct() const;
	void decreaseEnergy(int amount);
	void restoreFullEnergy();
	bool fire(int type, int firedBy); //pure virtual because only want playership 
							//and aliens to fire //CAN I JUST HAVE ONE FIRE FUNCTION?
private:
	int m_energy;

};
class PlayerShip : public Ship//public Actor
{
public:
	PlayerShip(StudentWorld* ptr);
	virtual ~PlayerShip();
	virtual void doSomething();
	int getNumTorpedos() const;
	void addTorpedos(int n);
	void damage(int amount, bool hitByProjectile);//(int amount, bool hitByProjectile)
	void checkForCollisionsWithAliens(PlayerShip* p);
	//virtual void fire();
private:
	int m_torpedos;
	bool fired;
};

class Alien : public Ship
{
public:
	Alien(StudentWorld* ptr, int id, int startEnergy, int worth); 
	virtual ~Alien();
	virtual void damage(int points, bool hitByProjectile);
	bool diedByCollision();
	virtual void maybeDropGoodie() = 0;
	virtual void doSomething() = 0;//do i need this here since its in actor and i wont have any pure aliens
	
private:
	int m_Worth;
	bool m_diedByCollision;
};

class NachlingBase : public Alien
{
public:
	NachlingBase(StudentWorld* ptr, int id, int round, int worth);
	virtual ~NachlingBase();
	virtual void doSomething();
	void setCanAct(bool m);
private:
	int m_state;
	bool m_canAct;
	int m_randDir;
	int m_HMD;
	int m_HMR;
	int m_MDB;
};

class Nachling : public NachlingBase
{
public:
	Nachling(StudentWorld* ptr, int round);
	virtual ~Nachling();
	virtual void maybeDropGoodie();
private:
	
};

class WealthyNachling : public NachlingBase
{
public:
	WealthyNachling(StudentWorld* ptr, int round);
	virtual ~WealthyNachling();
	virtual void doSomething();
	virtual void maybeDropGoodie();
private:
	bool m_malfunctioning;
	int m_malfunctionTime;
};

class Smallbot : public Alien
{
public:
	Smallbot(StudentWorld* world, int round);
	virtual ~Smallbot();
	virtual void damage(int points, bool hitByProjectile);
	void setJustHit(bool j);
	virtual void maybeDropGoodie();
	virtual void doSomething();
	
private:
	bool m_canAct;
	bool m_justHit;
};

class Bullet :public Actor
{
public:
	Bullet(StudentWorld* ptr, int bulletType, int x, int y, int firedBy, int damagePoints); 
	virtual ~Bullet();
	void doSomething();
	int firedBy() const;
	void checkForCollision(Bullet* b);
private:
	int m_firedBy; //1 is playership 2 is alien
	int m_damagePoints;
	int m_bulletType;//to be used in doSomething to determine damage
};

class SepticBullet : public Bullet
{
public:
	SepticBullet(StudentWorld* ptr, int x, int y, int firedBy);
	virtual ~SepticBullet();
private:
};

class FlatulenceTorpedo :public Bullet
{
public:
	FlatulenceTorpedo(StudentWorld* ptr, int x ,int y, int firedBy);
	virtual ~FlatulenceTorpedo();
private:
};

class Goodie : public Actor
{
public:
	Goodie(StudentWorld* ptr, int id, int x, int y); 
	virtual ~Goodie();
	virtual void doSomething();
	virtual void doSpecialAction(PlayerShip* p) = 0;
private:
	int m_goodieTickLifeTime;
	int m_ticksLeftToLive;
	int m_actEvery3Ticks;
};

class TorpedoGoodie : public Goodie
{
public:
	TorpedoGoodie(StudentWorld* ptr, int x, int y);
	virtual ~TorpedoGoodie();
	virtual void doSpecialAction(PlayerShip* p);
private:
};

class EnergyGoodie : public Goodie
{
public:
	EnergyGoodie(StudentWorld* ptr, int x, int y);
	virtual ~EnergyGoodie();
	virtual void doSpecialAction(PlayerShip* p);
private:
};

class FreeShipGoodie : public Goodie
{
public:
	FreeShipGoodie(StudentWorld* ptr, int x, int y);
	virtual ~FreeShipGoodie();
	virtual void doSpecialAction(PlayerShip* p);
private:
};

class Star : public Actor
{
public:
	Star(StudentWorld* ptr);
	virtual ~Star();
	virtual void doSomething();
private:
};

#endif // _ACTOR_H_