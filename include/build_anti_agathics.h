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

#ifndef ANTIAGATHICS_H
#define ANTIAGATHICS_H

#include <string>

#include "enhancement.h"

class Disaffection;
class	Fedmap;
class Player;
class Population;
class	Tokens;

class	AntiAgathics : public Enhancement
{
public:
	AntiAgathics(FedMap *the_map,const std::string& the_name,const char **attribs);
	AntiAgathics(FedMap *the_map,Player *player,Tokens *tokens);
	virtual ~AntiAgathics();

	const std::string&	Name()							{ return(name);	}

	bool	Add(Player *player,Tokens *tokens);

	void	Display(Player *player);
	void	UpdateDisaffection(Disaffection *discontent);
	void	UpdatePopulation(Population *population);
	void	Write(std::ofstream& file);
	void	XMLDisplay(Player *player);
};

#endif
