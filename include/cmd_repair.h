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

#ifndef CMDREPAIR_H
#define CMDREPAIR_H

#include <string>

class	FedMap;
class Player;
class	Ship;
class	Tokens;

class	RepairParser
{
private:
	static const std::string	vocab[];
	static const int	NO_NOUN;

	int	FindNoun(const std::string& noun);

	void	RepairDepot(Player *player,Tokens *tokens,const std::string& line);
	void	RepairFactory(Player *player,Tokens *tokens,const std::string& line);
	void	RepairShip(Player *player,Tokens *tokens);

public:
	RepairParser()		{	}
	~RepairParser()	{	}

	void	Process(Player *player,Tokens *tokens,const std::string& line);
};

#endif
