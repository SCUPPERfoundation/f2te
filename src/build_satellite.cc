/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2015
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "build_satellite.h"

#include <sstream>

#include "commodities.h"
#include "fedmap.h"
#include "infra.h"
#include "misc.h"
#include "player.h"
#include "tokens.h"
#include "xml_parser.h"


Satellite::Satellite(FedMap *the_map,const std::string& the_name,const char **attribs)
{
	fed_map = the_map;
	name = the_name;
	total_builds = XMLParser::FindNumAttrib(attribs,"points",0);
	ok_status = true;
}

Satellite::Satellite(FedMap *the_map,Player *player,Tokens *tokens)
{
	static const std::string	too_late("Satellite Launch Facilities can only be \
built at Industrial level and above.\n");

	if(the_map->Economy() < Infrastructure::INDUSTRIAL)
	{
		 player->Send(too_late);
		 ok_status = false;
	}
	else
	{
		if(!CheckCommodity(player,tokens))
			ok_status = false;
		else
		{		
			fed_map = the_map;
			name = tokens->Get(1);
			name[0] = std::toupper(name[0]);
			total_builds = 1;

			if(!fed_map->AddProductionPoint(player,tokens->Get(2)))
				ok_status = false;
			else
			{
				std::ostringstream	buffer;
			 	buffer << "Your new satellite launch and control facility is delayed because of changes in the plans, ";
				buffer << "but, is eventually completed and places its first satellite in orbit. Its construction ";
				buffer << "and commissioning requirements spur the production of " << tokens->Get(2) << ".\n";
				player->Send(buffer);
				ok_status = true;
			}
		}
	}
}

Satellite::~Satellite()
{

}


bool	Satellite::Add(Player *player,Tokens *tokens)
{
	static const std::string	too_late("Satellite Launch Facilities can only be \
built at Industrial level and above.\n");

	if((fed_map->Economy() < Infrastructure::INDUSTRIAL))
	{
		 player->Send(too_late);
		 return(false);
	}

	std::ostringstream	buffer;
	if(total_builds < 5)
	{
		if(!CheckCommodity(player,tokens))
			return(false);

		if(fed_map->AddProductionPoint(player,tokens->Get(2)))
		{
		 	buffer << "Your satellite launch facility is completed on time and ";
			buffer << "within budget. Its day to day operational requirements ";
			buffer << "spur the production of " << tokens->Get(2) << ".\n";
			player->Send(buffer);
			total_builds++;
			return(true);
		}
		return(false);
	}

 	buffer << "Your satellite launch complexis completed somewhat later than ";
	buffer << "scheduled, but within budget. While the planetary defences ";
	buffer << "are undoubtedly more formidable as a result, the facility seems ";
	buffer << "to have little effect on the planet's overall production!\n";
	player->Send(buffer);
	total_builds++;
	return(true);
}

bool	Satellite::CheckCommodity(Player *player,Tokens *tokens)
{
	static const std::string	unknown("I don't know which commodity production you want to improve!\n");
	static const std::string	no_commod("You haven't said what commodity to add the production point to!\n");

	if(tokens->Size() < 3)
	{
		player->Send(no_commod);
		return(false);
	}

	if(Game::commodities->Find(tokens->Get(2)) == 0)
	{
		player->Send(unknown);
		return(false);
	}

	std::ostringstream	buffer;
	if(!Game::commodities->IsDefenceType(tokens->Get(2)))
	{
		buffer << "You cannot allocate a production point to " << tokens->Get(2);
			buffer << ", only to defence industry commodities.\n";
		player->Send(buffer);
		return(false);
	}
	return(true);
}

bool	Satellite::Demolish(Player *player)
{
	player->Send("Your proposal nearly incites a mutiny in the defense community, so you \
hastily withdraw it!\n");
	return(false);
}

void	Satellite::Display(Player *player)
{
	std::ostringstream	buffer;
	buffer << "    Satellite Facilities : " << total_builds << " built\n";
	player->Send(buffer);
}

void	Satellite::Write(std::ofstream& file)
{
	file << "  <build type='Satellite' points='" << total_builds << "'/>\n";
}

void	Satellite::XMLDisplay(Player *player)
{
	std::ostringstream	buffer;
	buffer << "<s-build-planet-info info='Satellite Facilities: " << total_builds << "'/>\n";
	player->Send(buffer);
}


