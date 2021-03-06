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

#ifndef	BUY_H
#define	BUY_H

#include <string>

class	Player;
class	Tokens;

class	BuyParser
{
public:
	enum { UNKNOWN, SENSORS, JAMMERS };

private:
	static const std::string	vocab[];
	static const int	NO_NOUN;

	int	FindNoun(const std::string& noun);

	void	BuyFactory(Player *player,Tokens *tokens);
	void	BuyFuel(Player *player,Tokens *tokens);
	void	BuyFutures(Player *player,Tokens *tokens);
	void	BuyJammers(Player *player,Tokens *tokens,const std::string& line);
	void	BuyMissiles(Player *player,Tokens *tokens,const std::string& line);
	void	BuyPizza(Player *player,Tokens *tokens,const std::string& line);
	void	BuyRegistry(Player *player);
	void	BuyRound(Player *player,Tokens *tokens,const std::string& line);
	void	BuySensors(Player *player,Tokens *tokens,const std::string& line);
	void	BuyShares(Player *player,Tokens *tokens,const std::string& line);
	void	BuyShip(Player *player);
	void	BuyTreasury(Player *player,Tokens *tokens,const std::string& line);

public:
	BuyParser()		{	}
	~BuyParser()	{	}

	void	Process(Player *player,Tokens *tokens,const std::string& line);
};

#endif

