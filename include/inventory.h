/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2015
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef INVENTORY_H
#define INVENTORY_H


#include <bitset>
#include <list>
#include <string>
#include <sstream>

#include <ctime>

#include "obj_container.h"

class FedObject;
class	DBPlayer;
class ObjContainer;
class Player;

class	Inventory : public ObjContainer
{
private:
	static const std::string	keyring_desc;
	static const std::string	exec_key_desc;

public:
	static const int	MAX_INV_SIZE = 100;
	static const int	ONE_DAY;
	static const int	ONE_MONTH;

	enum		// bit flag values for general inventory flags
	{
		COMM, LAMP, SHIP_PERMIT, WARE_PERMIT, SPYSCREEN, ID_CARD, MEDAL,
		WED_RING, PRICE_CHECK_UPGRADE, PRICE_CHECK_PREMIUM, KEYRING, EXEC_KEY,
		TP_1, TP_31, MAX_INV_FLAGS
	};

	enum 		// bit flag values for keyring items
	{
		DONT_USE, 		OLD_KEYRING, 		OLD_EXEC_KEY,		MAX_KEY
	};

private:
	static const std::string	inv_names[];	// general inventory names
	static const std::string	key_names[];	// keyring item names
	
	std::string		owner;							// the inventory's owner's name
	std::bitset<MAX_KEY>			keyring;			// 'gift' keyring items
	std::bitset<MAX_INV_FLAGS>	inv_flags;		// permanent inventory
	std::time_t		customs_cert;		// time customs cert started (zero = no cert)
	std::time_t		price_check_sub;	// time price check sub started (zero = no cert)
	time_t			tp_rental;			// time teleporter rented (TP_1 only);

	Inventory(const Inventory& rhs);
	Inventory& operator=(const Inventory& rhs);

	int	Display2Watcher(Player *player,std::ostringstream& buffer);
	int	DisplayList(std::ostringstream& buffer);	// returns total objects added

	int	XMLDisplay2Watcher(Player *player,std::ostringstream& buffer);
	int	XMLDisplayList(std::ostringstream& buffer);
	int	XMLDisplay(Player *player,std::ostringstream& buff);


	void	DisplayCerts(std::ostringstream& buffer);
	void	DisplayKeyring(Player *player,bool self);
	void	DisplayPersonal(std::ostringstream& buffer);
	void	ProcessTimeCerts();

	void	XMLDisplayCerts(std::ostringstream& buffer);
	void	XMLDisplayKeyring(Player *player,bool self);
	void	XMLDisplayPersonal(std::ostringstream& buffer);

public:
	Inventory(std::string the_owner);
	Inventory(DBPlayer *rec);
	~Inventory()	{	}

	FedObject	*RemoveObject(const std::string & name);
	FedObject	*RemoveObjectIDName(const std::string& id_name);

	int	Display(Player *player,std::ostringstream& buff);
	int	DisplayInventory(Player *player);
	int	WeightCarried();

	bool	AddObject(FedObject *obj);
	bool	DestroyInvObject(const std::string& map_name,const std::string& id_name);
	bool	HasCustomsCert()					{ return(customs_cert != 0);			}
	bool	HasRemoteAccessCert()			{ return(price_check_sub != 0);		}
	bool	HasTeleporter(int which);
	bool	InvFlagSet(int which)			{ return(inv_flags.test(which));		}
	bool	IsInInventory(const std::string& star_name,const std::string& map_name,const std::string& id_name);

	void	Carry(Player *player,const std::string& obj_name);
	void	ChangeCustomsCert(int num_days);
	void	Clip(Player *player,const std::string& obj_name);
	void	CreateDBRec(DBPlayer *rec);
	void	DestroyInventory(const std::string& map_title);
	void	Doff(Player *player,const std::string& obj_name);
	void	FlipInvFlag(int which)			{ inv_flags.flip(which);				}
	void	MakeFluff(std::ostringstream& buffer);
	void	NewCustomsCert()					{ customs_cert =  std::time(0);		}
	void	NewRemoteCert(Player *player);
	void	Pocket(Player *player,const std::string& obj_name);
	void	SetInvFlag(int which)			{ inv_flags.set(which);					}
	void	SetTpRental();
	void	Store(const std::string& owner,DBObject *object_db);
	void	Unclip(Player *player);
	void	Wear(Player *player,const std::string& obj_name);
};

#endif
