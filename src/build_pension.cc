/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2015
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "build_pension.h"

#include <sstream>

#include "commodities.h"
#include "disaffection.h"
#include "fedmap.h"
#include "infra.h"
#include "misc.h"
#include "player.h"
#include "tokens.h"
#include "xml_parser.h"

const std::string	Pension::success = "Your new pension scheme is widely applauded as \
fair and equitable. However there are those who feel that it is a step on the \
slippery road to state sponsored socialism, and as such opposed its promulgation.\n";

Pension::Pension(FedMap *the_map,const std::string& the_name,const char **attribs)
{
	fed_map = the_map;
	name = the_name;
	total_builds = XMLParser::FindNumAttrib(attribs,"points",0);
	ok_status = true;
}

Pension::Pension(FedMap *the_map,Player *player,Tokens *tokens)
{
	static const std::string	not_allowed("Pensions are not available below resource level!\n");

	int	economy = the_map->Economy();
	if((economy < Infrastructure::RESOURCE))
	{
		player->Send(not_allowed);
		ok_status = false;
	}
	else
	{
		fed_map = the_map;
		name = tokens->Get(1);
		name[0] = std::toupper(name[0]);
		total_builds = 1;
		player->Send(success);
		ok_status = true;
	}
}

Pension::~Pension()
{

}


bool	Pension::Add(Player *player,Tokens *tokens)
{
	total_builds++;
	player->Send(success);
	return(true);
}

void	Pension::Display(Player *player)
{
	std::ostringstream	buffer;
	if(total_builds > 1)
		buffer << "    Pensions: " << total_builds << " levels\n";
	else
		buffer << "    Pensions: 1 level\n";
	player->Send(buffer);
}

void	Pension::UpdateDisaffection(Disaffection *discontent)
{
	if(total_builds <= 10)
		discontent->TotalPensionPoints(total_builds);
	else
		discontent->TotalPensionPoints(10);
}

void	Pension::Write(std::ofstream& file)
{
	file << "  <build type='Pension' points='" << total_builds<< "'/>\n";
}

void	Pension::XMLDisplay(Player *player)
{
	std::ostringstream	buffer;
	buffer << "<s-build-planet-info info='Pension level: " << total_builds << "'/>\n";
	player->Send(buffer);
}



