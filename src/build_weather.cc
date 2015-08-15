/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2015
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "build_weather.h"

#include <sstream>

#include "commodities.h"
#include "disaffection.h"
#include "efficiency.h"
#include "fedmap.h"
#include "infra.h"
#include "misc.h"
#include "output_filter.h"
#include "player.h"
#include "tokens.h"
#include "xml_parser.h"

Weather::Weather(FedMap *the_map,const std::string& the_name,const char **attribs)
{
	fed_map = the_map;
	name = the_name;
	total_builds = XMLParser::FindNumAttrib(attribs,"points",0);
	ok_status = true;
}

Weather::Weather(FedMap *the_map,Player *player,Tokens *tokens)
{
	const std::string	success("The opening ceremony for the planet's first weather \
control station is marred by attempted sabotage from a group calling themselves 'The \
Weathermen'. Fortunately the attempt is inept and the opening is successfully completed.\n");
	const std::string	not_allowed("Weather control stations can only be built at \
technological levels and above!\n");

	int	economy = the_map->Economy();
	if(economy < Infrastructure::TECHNICAL)
	{
		player->Send(not_allowed,OutputFilter::DEFAULT);
		ok_status = false;
	}
	else
	{
		fed_map = the_map;
		name = tokens->Get(1);
		name[0] = std::toupper(name[0]);
		total_builds = 1;
		player->Send(success,OutputFilter::DEFAULT);
		ok_status = true;
	}
}

Weather::~Weather()
{

}


bool	Weather::Add(Player *player,Tokens *tokens)
{
	const std::string	success("The opening of another weather control station is a low key \
affair, in order to avoid drawing the attention of Weathermen saboteurs.\n");
	const std::string	over("The latest weather control station is, strictly speaking, unneeded, \
but it provides a useful insurance in case the Weathermen succeed in sabotaging one of the \
other stations!\n");
	const std::string	not_allowed("Weather control stations can only be built at \
technological levels and above!\n");

	int	economy = fed_map->Economy();
 	if(economy < Infrastructure::TECHNICAL)
	{
		player->Send(not_allowed,OutputFilter::DEFAULT);
		return(false);
	}

	if(++total_builds <= 5)
		player->Send(success,OutputFilter::DEFAULT);
	else
		player->Send(over,OutputFilter::DEFAULT);
	return(true);
}

void	Weather::Display(Player *player)
{
	std::ostringstream	buffer;
	buffer << "    Weather Stations: " << total_builds << " built\n";
	player->Send(buffer,OutputFilter::DEFAULT);
}

bool	Weather::RequestResources(Player *player,const std::string& recipient,int quantity)
{
	if(recipient == "Floating")
	{
		if(total_builds >= quantity)
			return(true);
	}
	return(false);
}

void	Weather::UpdateDisaffection(Disaffection *discontent)
{
	discontent->TotalWeatherPoints((total_builds > 5) ? 10 : (total_builds * 2));
}

void	Weather::UpdateEfficiency(Efficiency *efficiency)
{
	efficiency->TotalWeatherPoints((total_builds <= 5) ? total_builds : 5);
}

void	Weather::Write(std::ofstream& file)
{
	file << "  <build type='Weather' points='" << total_builds << "'/>\n";
}

void	Weather::XMLDisplay(Player *player)
{
	std::ostringstream	buffer;
	buffer << "Weather Stations: " << total_builds;
	AttribList attribs;
	std::pair<std::string,std::string> attrib(std::make_pair("info",buffer.str()));
	attribs.push_back(attrib);
	player->Send("",OutputFilter::BUILD_PLANET_INFO,attribs);
}




