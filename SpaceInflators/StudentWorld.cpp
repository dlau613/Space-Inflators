#include "StudentWorld.h"
#include "actor.h"
#include <iostream>
#include <sstream>
#include <iomanip>
GameWorld* createStudentWorld()
{
    return new StudentWorld();
}

StudentWorld::StudentWorld()
	: m_nStars(0), m_nActiveAliens(0), m_RoundNumber(1), m_AliensDestroyed(0)
{
}
StudentWorld::~StudentWorld()
{
	delete ptrToShip;//is this necessary?
	for (int i = 0; i < m_actors.size(); i++)
	{
		delete m_actors[i];
		//m_actors.erase(m_actors.begin()+i);
	}
}

bool StudentWorld::addActor(int type, int x, int y, int firedBy)//fired by alien(2) or player(1) or not fired (0) 
{
	if (type < 1 || type > 8)//id numbers for all actors except playership and star
		return false;
	if (firedBy < 0 || firedBy > 2)
		return false;
	switch (type)
	{
	case IID_NACHLING:
		//m_actors.push_back(new Nachling(this, m_RoundNumber));
		break;
	case IID_WEALTHY_NACHLING:
		//m_actors.push_back(new WealthyNachling(this, m_RoundNumber));
		break;
	case IID_SMALLBOT:
		//m_actors.push_back(new Smallbot(this, m_RoundNumber));
		break;
	case IID_BULLET:
		m_actors.push_back(new SepticBullet(this, x, y, firedBy));
		break;
	case IID_TORPEDO:
		m_actors.push_back(new FlatulenceTorpedo(this, x, y, firedBy));
		break;
	case IID_FREE_SHIP_GOODIE:
		m_actors.push_back(new FreeShipGoodie(this, x, y));
		break;
	case IID_ENERGY_GOODIE:
		m_actors.push_back(new EnergyGoodie(this, x, y));
		break;
	case IID_TORPEDO_GOODIE:
		m_actors.push_back(new TorpedoGoodie(this, x, y));
		break;
	return true;
	}
	return true;
}
	
int StudentWorld::numberOfActiveAliens() const
{
	return m_nActiveAliens;
}

int StudentWorld::getRoundNumber() const
{
	return m_RoundNumber;
}

void StudentWorld::updateDisplayText()
{
	int score = getScore();
	int round = getRoundNumber();
	double energyPercent = ptrToShip->getEnergyPct();
	int torpedos = ptrToShip->getNumTorpedos();
	int shipsLeft = getLives();
	string s = formatDisplay(score, round, energyPercent, torpedos, shipsLeft);
	setGameStatText(s);
}

string StudentWorld::formatDisplay(int score, int round, double energyPercent, int torpedos, int shipsLeft)
{
	ostringstream oss;
	oss.fill('0');
	oss << "Score: " << setw(7) << score << "  Round: " << setw(2) << round << "  Energy: " 
		<< energyPercent << "%  Torpedoes: " << setw(3) << torpedos << "  Ships: " << setw(2) << shipsLeft;

	return oss.str();
}

PlayerShip* StudentWorld::getPlayerShip()
{
	return ptrToShip;
}

int StudentWorld::getNumAlienFiredProjectiles() const
{
	int count = 0;
	for (int i = 0; i < m_actors.size(); i++)
	{
		Bullet* p = dynamic_cast<Bullet*>(m_actors[i]);
		if (p != NULL)
		{
			if (p->firedBy() == 2)
				count++;
		}
	}
	return count;
}

PlayerShip* StudentWorld::getCollidingPlayer(Actor* a)
{
	if (a->getX() == ptrToShip->getX() && a->getY() == ptrToShip->getY())
		return ptrToShip;
	else
		return NULL;
}

list<Alien*> StudentWorld::getCollidingAliens(Actor* a)
{
	list<Alien*> collidedWith;
	for (int i = 0; i < m_actors.size(); i++)
	{
		if (m_actors[i]->getX() == a->getX() && m_actors[i]->getY() == a->getY())
		{
			Alien* b = dynamic_cast<Alien*>(m_actors[i]);
			if (b != NULL)
				collidedWith.push_front(b);
		}
	}
	return collidedWith;
}

void StudentWorld::init()
{
	ptrToShip = new PlayerShip(this);
	m_nActiveAliens = 0;
	m_AliensDestroyed = 0;
	m_nStars = 0;
}
int StudentWorld::move()
{
		//ADD NEW ALIENS OR STARS 
	if (testParamsProvided() == true)
	{
		if (m_actors.size() == 0)
		/*int countAliensAndGoodies = 0;
		for (int i = 0; i <m_actors.size(); i++)
		{
			Alien* a = dynamic_cast<Alien*>(m_actors[i]);
			if (a != NULL)
				countAliensAndGoodies++;
			Goodie* g = dynamic_cast<Goodie*>(m_actors[i]);
			if (g != NULL)
				countAliensAndGoodies++;
		}
		if (countAliensAndGoodies = 0)*/
		{
		int whichActor = getTestParam(TEST_PARAM_ACTOR_INDEX);
		if (whichActor == TEST_PARAM_NACHLING)
			m_actors.push_back(new Nachling(this, m_RoundNumber));
		if (whichActor == TEST_PARAM_WEALTHY_NACHLING)
			m_actors.push_back(new WealthyNachling(this,m_RoundNumber));
		if (whichActor == TEST_PARAM_SMALLBOT)
			m_actors.push_back(new Smallbot(this, m_RoundNumber));
		if (whichActor == TEST_PARAM_GOODIE_ENERGY)
			m_actors.push_back(new EnergyGoodie(this, 15, 39));
		if (whichActor == TEST_PARAM_GOODIE_TORPEDO)
			m_actors.push_back(new TorpedoGoodie(this, 15, 39));
		if (whichActor == TEST_PARAM_GOODIE_FREE_SHIP)
			m_actors.push_back(new FreeShipGoodie(this, 15, 39));
		}
	}
	///////////////
	else
	{
	int maxAliens = int(m_RoundNumber * .5 + 2);
	if (m_nActiveAliens < maxAliens && m_nActiveAliens < 4 * m_RoundNumber - m_AliensDestroyed)
	{
		int random1 = rand() % 100;
		if (random1 < 70)
		{
			int random2 = rand() %100;
			if (random2 < 20)
			{
				m_actors.push_back(new WealthyNachling(this, m_RoundNumber));
				m_nActiveAliens++;
			}
			else
			{
				m_actors.push_back(new Nachling(this, m_RoundNumber));
				m_nActiveAliens++;
			}	
		}
		else 
		{
			m_actors.push_back(new Smallbot(this, m_RoundNumber));
			m_nActiveAliens++;
		}
	}
	}
		int random3 = rand() % 100;
		if (random3 < 33)
		{
			m_actors.push_back(new Star(this));
			//m_nStars++;
		}
		//UPDATE THE GAME STATUS LINE
		updateDisplayText();

		//GIVE EACH ACTOR A CHANCE TO DO SOMETHING
		if (ptrToShip->isStillAlive())
			ptrToShip->doSomething();
		for (int i = 0; i < m_actors.size(); i++)
		{
			if (m_actors[i]->isStillAlive() == true)
				m_actors[i]->doSomething();
		}
		//DELETE DEAD ACTORS
		for (int i = 0; i <m_actors.size(); i++)//WHY DOES THIS MAKE IT CRASH
		{
			if (m_actors[i]->isStillAlive() == false)
			{
				Alien* a = dynamic_cast<Alien*>(m_actors[i]);
				if (a != NULL)
				{
					m_nActiveAliens--;
					if (a->diedByCollision() == false)
						m_AliensDestroyed++;	
				}
				delete m_actors[i];
				m_actors.erase(m_actors.begin()+i);
				i--;
			}
		}

		if (m_AliensDestroyed >= m_RoundNumber * 4)
		{
			m_RoundNumber++;
			m_AliensDestroyed = 0;
		}
		if (ptrToShip->isStillAlive() == false)
		{
			decLives();
			return GWSTATUS_PLAYER_DIED;
		}
		else

			return GWSTATUS_CONTINUE_GAME;
		//decLives();
	//return GWSTATUS_PLAYER_DIED;// This code is here merely to allow the game to build, run, and terminate after hitting enter a few times 
}

void StudentWorld::cleanUp()
{
	delete ptrToShip;//is this necessary?
	for (int i = 0; i < m_actors.size(); i++)
	{
		delete m_actors[i];
		m_actors.erase(m_actors.begin()+i);
	}
}

// Students:  Add code to this file (if you wish), StudentWorld.h, actor.h and actor.cpp