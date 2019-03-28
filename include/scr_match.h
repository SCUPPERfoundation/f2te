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

#ifndef MATCH_H
#define MATCH_H

#include "script.h"

#include <string>

class EventNumber;
class FedMap;
class MsgNumber;

class Match : public Script
{
protected:
	std::string	id_name;
	std::string	phrase;
	MsgNumber	*lo, *hi;
	EventNumber	*ev_num;

public:
	Match(const char **attrib,FedMap *fed_map);
	virtual	~Match();

	int	Process(Player *player);
};

#endif
