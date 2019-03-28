/*-----------------------------------------------------------------------
                          Federation 2
              Copyright (c) 1985-2018 Alan Lenton

This program is free software: you can redistribute it and /or modify 
it under the terms of the GNU General Public License as published by 
the Free Software Foundation: either version 2 of the License, or (at 
your option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY: without even the implied warranty of 
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
General Public License for more details.

You can find the full text of the GNU General Public Licence at
           http://www.gnu.org/copyleft/gpl.html

Programming and design:     Alan Lenton (email: alan@ibgames.com)
Home website:                   www.ibgames.net/alan
-----------------------------------------------------------------------*/

#ifndef MOVE_H
#define MOVE_H

#include <string>

#include "script.h"

class	FedMap;
class	Player;

class	Move : public Script
{
protected:
	enum	{ MV_PLAYER, MV_OBJECT, MV_UNKNOWN	};

	int	what, who;
	bool	silent;
	std::string	id_name;
	int	loc_num;

	int	ProcessPlayerMove(Player *player);
	int	ProcessObjectMove(Player *player);

public:
	Move(const char **attrib,FedMap	*fed_map);
	virtual	~Move();

	int	Process(Player *player);
};

#endif
