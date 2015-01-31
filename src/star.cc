/*----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2015
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
----------------------------------------------------------------------*/

#include "star.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>


#include <cstdio>
#include <ctime>

#include <unistd.h>
#include <sys/dir.h>

#include "cartel.h"
#include "display_cabinet.h"
#include "fedmap.h"
#include "galaxy.h"
#include "infra.h"
#include "location.h"
#include "misc.h"
#include "player.h"
#include "player_index.h"
#include "review.h"
#include "star_parser.h"
#include "syndicate.h"


const int	Star::NAME_SIZE;

Star::Star(const std::string& star_name,const std::string& dir)
{
	name = star_name;
	directory = dir;
	hosp_map = 0;
	hosp_loc = Location::INVALID_LOC;
	cabinet = new DisplayCabinet();
WriteLog(star_name);
}

Star::~Star()
{
	Write();
	for(MapIndex::iterator iter = map_index.begin();iter != map_index.end();iter++)
		delete iter->second;
}


bool	Star::AddExile(const std::string player_name)
{
	if(!IsAnExile(player_name))
	{
		black_list.push_back(player_name);
		return(true);
	}
	return(false);
}

void	Star::AddMap(FedMap *fed_map)
{
	map_index[fed_map->Title()] = fed_map;
}

void	Star::AddMap(const char *map_name)
{
	char	file_name[MAXNAMLEN];
	std::snprintf(file_name,MAXNAMLEN,"maps/%s/%s",directory.c_str(),map_name);
	char	full_name[MAXNAMLEN];
	std::snprintf(full_name,MAXNAMLEN,"%s/%s.loc",HomeDir(),file_name);
	if(access(full_name,F_OK) == 0)
		AddMap(new FedMap(file_name,this,map_name));
	else
	{
		std::ostringstream	buffer;
		buffer << "*** Unable to find file '" << full_name << "' from maps.dat ***";
		WriteLog(buffer);
	}
}

void	Star::BuildDestruction()
{
	if(IsOpen())
	{
		for(MapIndex::iterator iter = map_index.begin();iter != map_index.end();iter++)
			iter->second->BuildDestruction();
	}
}

void	Star::CheckCartelCommodityPrices(Player *player,const Commodity *commodity,bool send_intro)
{
	for(MapIndex::iterator iter = map_index.begin();iter != map_index.end();iter++)
		iter->second->CheckCartelCommodityPrices(player,commodity,name,send_intro);
}

void	Star::DisplayExile(std::ostringstream& buffer)
{
	if(black_list.size() > 0)
	{
		buffer << "  " << name << ":\n";
		for(BlackList::iterator	iter = black_list.begin();iter != black_list.end();iter++)
			buffer << "    " << *iter << "\n";
	}
}

void	Star::DisplayFleets(Player *player)
{
	if(name == "Sol")
		return;

	for(MapIndex::iterator iter = map_index.begin();iter != map_index.end();iter++)
		iter->second->DisplayFleet(player);
}

void	Star::DisplaySystem(Player *player)
{
	std::ostringstream	buffer;
	buffer << "System information for the " << name << " system:\n";
	buffer << "  Member of the " << cartel << " Cartel\n";
	if(abandoned)
		buffer << "  System has been abandoned by its owner!\n";
	player->Send(buffer);
	for(MapIndex::iterator iter = map_index.begin();iter != map_index.end();iter++)
		iter->second->Display(player,false);

	if((map_index.begin()->second->Owner() == player->Name()) && (black_list.size() > 0))
	{
		buffer.str("");
		buffer << "Exiled players:\n";
		for(BlackList::iterator iter = black_list.begin();iter != black_list.end();iter++)
			buffer << "  " << *iter << "\n";
		player->Send(buffer);
	}
}

void	Star::EnforceExile(const std::string player_name)
{
	// deal with a target in the game
	Player	*target = Game::player_index->FindCurrent(player_name);
	if((target != 0) && (IsInSystem(player_name)))
	{
		target->ForcedMove("Sol","Earth",585);
		return;
	}

	// then deal with a logged off target
	target = Game::player_index->FindName(player_name);
	if(target != 0)
	{
		if(target->GetLocRec().star_name == name)
		{
			target->SafeHaven();
			Game::player_index->Save(target,PlayerIndex::NO_OBJECTS);
		}
	}
}

void	Star::Exile(Player *player,const std::string& pl_name)
{
	static const std::string	silly("Don't be silly!\n");

	std::string	player_name(pl_name);
	Normalise(player_name);
	if((player_name == "Bella") || (player_name == "Freya") ||
						(player_name == "Hazed") || (player_name == player->Name()))
	{
		player->Send(silly);
		return;
	}

	std::ostringstream	buffer;
	if(!AddExile(player_name))
	{
		buffer << player_name << " is already exiled from this system!\n";
		player->Send(buffer);
		return;
	}

	EnforceExile(player_name);
	buffer << player_name << " has been exiled from the " << name << " system!\n";
	player->Send(buffer);
	Game::review->Post(buffer);

	Player	*target = Game::player_index->FindName(player_name);
	if(target != 0)
	{
		buffer.str("");
		buffer << player->Name() << " has exiled you from the " << name << " system!\n";
		target->Send(buffer);
	}
}

FedMap	*Star::Find(const std::string& map_title)
{
	MapIndex::iterator	iter = map_index.find(map_title);
	if(iter != map_index.end())
		return(iter->second);
	else
		return(0);
}

FedMap	*Star::FindByName(const std::string& map_name)
{
	// note that if there are a lot of maps in Sol we may
	// need a name based index as well as a title based one
	for(MapIndex::iterator iter = map_index.begin();iter != map_index.end();iter++)
	{
		if(iter->second->Name() == map_name)
			return(iter->second);
	}
	return(0);
}

void	Star::FindLandingPad(LocRec *loc,const std::string& orbit)
{
	int	loc_no = -1;
	for(MapIndex::iterator iter = map_index.begin();iter != map_index.end();iter++)
	{
		if((loc_no = iter->second->LandingPad(orbit)) != -1)
		{
			loc->star_name = name;
			loc->map_name = iter->second->Title();
			loc->loc_no = loc_no;
			loc->fed_map = iter->second;
			return;
		}
	}
	loc->loc_no = -1;
}

LocRec	*Star::FindLink() const
{
	LocRec	*link = 0;
	for(MapIndex::const_iterator iter = map_index.begin();iter != map_index.end();iter++)
	{
		if((link = iter->second->FindLink()) != 0)
			break;
	}
	return(link);
}

FedMap	*Star::FindPlanet()
{
	for(MapIndex::iterator iter = map_index.begin();iter != map_index.end();iter++)
	{
		if(iter->second->LandingPad() != FedMap::NO_PAD)
			return(iter->second);
	}
	return(0);
}

FedMap	*Star::FindRandomMap(FedMap *except)
{
	if(!IsOpen())
		return(0);

	int	size = map_index.size();

	int counter, index;
	MapIndex::iterator	iter;
	for(int count = 0;count < 100;count++)	// make sure there is no infinite loop!
	{
		counter = rand() % size;
		for(index = 0,iter = map_index.begin();iter != map_index.end();iter++,index++)
		{
			if(index == counter)
				break;
		}
		if((iter != map_index.end()) && iter->second->ProvidesJobs() && (iter->second != except))
			return(iter->second);
	}
	return(0);
}

FedMap	*Star::FindTopLevelPlanet()
{
	int level = 0;
	FedMap	*fed_map = 0;

	for(MapIndex::iterator iter = map_index.begin();iter != map_index.end();iter++)
	{
		if(iter->second->Economy() > level)
		{
			level = iter->second->Economy();
			fed_map = iter->second;
		}
	}

	if(level == 0)
		return(0);
	else
		return(fed_map);
}

// this differs from FindTopLevelPlanet() in that bio planets don't count
FedMap	*Star::GetPrimaryPlanet()
{
	int level = 0;
	FedMap	*fed_map = 0;

	for(MapIndex::iterator iter = map_index.begin();iter != map_index.end();iter++)
	{
		FedMap	*f_map = iter->second;
		if((f_map->Economy() > level) && (f_map->Economy()  != Infrastructure::BIOLOGICAL))
		{
			level = f_map->Economy();
			fed_map = f_map;
		}
	}

	if(level == 0)
		return(0);
	else
		return(fed_map);
}

void	Star::InformerRingBust()
{
	for(MapIndex::iterator iter = map_index.begin();iter != map_index.end();iter++)
		iter->second->InformerRingBust();
}

bool	Star::IsAnExile(const std::string player_name)
{
	for(BlackList::iterator iter = black_list.begin();iter !=  black_list.end();iter++)
	{
		if(player_name == *iter)
			return(true);
	}
	return(false);
}

bool	Star::IsDiverting(const std::string& commod)
{
	Cartel	*tgt_cartel = Game::syndicate->Find(cartel);
	if(!IsDiverting())
		return(false);
	if(tgt_cartel == 0)
		return(false);
	else
		return(tgt_cartel->NeedsCommodity(commod));
}

bool	Star::IsInSystem(const std::string player_name)
{
	for(MapIndex::iterator iter = map_index.begin();iter != map_index.end();iter++)
	{
		if(iter->second->FindPlayer(player_name))
			return(true);
	}
	return(false);
}

bool	Star::IsOpen()
{
	LocRec	*link = 0;
	for(MapIndex::const_iterator iter = map_index.begin();iter != map_index.end();iter++)
	{
		if((link = iter->second->FindLink()) != 0)
			break;
	}
	bool is_open = link->fed_map->IsOpen(0);
	delete link;
	return(is_open);
}

bool	Star::IsOwner(Player *player)
{
	for(MapIndex::const_iterator iter = map_index.begin();iter != map_index.end();iter++)
	{
		if(iter->second->IsOwner(player))
			return(true);
	}
	return(0);
}

void	Star::LiquidateAllFuturesContracts()
{
	for(MapIndex::iterator iter = map_index.begin();iter != map_index.end();iter++)
		iter->second->LiquidateAllFuturesContracts();
}

const std::string&	Star::ListSystem(Player *player,int which_ones)
{
	static const std::string	is_closed("");
	static std::string	text;
	std::ostringstream	buffer;

	buffer << name;
	for(MapIndex::iterator iter = map_index.begin();iter != map_index.end();++iter)
	{
		if(iter->second->FileName().find("/space") != std::string::npos)
		{
			buffer << " - " << iter->second->Owner() << ": ";
			if(!iter->second->IsOpen(0))
			{
				if(which_ones == LIST_OPEN)
					text = is_closed;
				else
				{
					buffer << "(Closed)\n";
					text = buffer.str();
				}
				return(text);
			}
		}
	}
	std::string	star_name(buffer.str());

	for(MapIndex::iterator iter = map_index.begin();iter != map_index.end();++iter)
		buffer << iter->second->List(player,star_name);

	buffer << "\n";
	text = buffer.str();
	return(text);
}

void	Star::Load()
{
	std::ostringstream	buffer;
	buffer << HomeDir() << "/maps/" << directory << "/star.inf";
	std::FILE	*file = fopen(buffer.str().c_str(),"r");
	if(file == 0)	// Note: No .inf file is not an error at the moment
		return;

	StarParser	*parser = new StarParser(this);
	parser->Parse(file,buffer.str());
	delete parser;
	std::fclose(file);
}

void	Star::LoadDisplayCabinet()
{
	if(!cabinet->Load(directory))
	{
		delete cabinet;
		cabinet = 0;
	}
}

void	Star::MapStats(std::ofstream&	map_file)
{
	map_file << name << " (" << directory << ")" << std::endl;
	if((hosp_map != 0) && (hosp_loc != Location::INVALID_LOC))
		map_file << "Hospital at " << hosp_map->Title() << ":" << hosp_loc << std::endl;
	for(MapIndex::iterator iter = map_index.begin();iter != map_index.end();iter++)
		iter->second->MapStats(map_file);
	map_file << std::endl;
}

bool	Star::MarkAbandondedSystem()
{
	Player *owner = Owner();
	if(owner == 0)
	{
		std::ostringstream	buffer;
		buffer << "***" << name << " system has no owner!***";
		WriteLog(buffer);
		abandoned = true;
	}
	else
	{
		time_t last_time_on = owner->LastTimeOn();
		time_t now =  std::time(0);
		long	how_long = std::difftime(now,last_time_on);
		if(how_long > ABANDONED)
			abandoned = true;
		else
			abandoned = false;
	}
	return abandoned;
}

void	Star::MoveMobiles(){
	for(MapIndex::iterator iter = map_index.begin();iter != map_index.end();iter++)
		iter->second->MoveMobiles();
}

const std::string&	Star::NewCartel(const std::string& new_name)
{
	static std::string old_cartel;
	old_cartel = cartel;
	cartel = new_name;
	return(old_cartel);
}

unsigned Star::NumLocs()
{
	unsigned	total = 0;
	for(MapIndex::iterator iter = map_index.begin();iter != map_index.end();iter++)
		total += iter->second->NumLocs();
	return(total);
}

Player	*Star::Owner()
{
	return(Game::player_index->FindName(map_index.begin()->second->Owner()));
}

void	Star::Pardon(Player *player,const std::string& pl_name)
{
	std::string	player_name(pl_name);
	std::ostringstream	buffer;
	if(!IsAnExile(Normalise(player_name)))
	{
		buffer << player_name << " isn't exiled from this system!\n";
		player->Send(buffer);
		return;
	}

	for(BlackList::iterator iter = black_list.begin();iter != black_list.end();iter++)
	{
		if(player_name == *iter)
		{
			black_list.erase(iter);
			break;
		}
	}

	buffer << player_name << " has been pardoned and can now use the " << name << " system!\n";
	player->Send(buffer);
	Game::review->Post(buffer);

	Player	*target = Game::player_index->FindName(player_name);
	if(target != 0)
	{
		buffer.str("");
		buffer << player->Name() << " has pardoned you, and you may now visit the " << name << " system!\n";
		target->Send(buffer);
	}
}

void	Star::PremiumPriceCheck(const Commodity *commodity,std::ostringstream& buffer,int which)
{
	for(MapIndex::iterator iter = map_index.begin();iter != map_index.end();iter++)
		iter->second->PremiumPriceCheck(commodity,buffer,which);
}

void	Star::ProcessInfrastructure()
{
	std::ostringstream buffer;
	buffer << "Star: " << name;
	for(MapIndex::iterator iter = map_index.begin();iter != map_index.end();iter++)
		iter->second->ProcessInfrastructure();
}

void	Star::PromotePlanetOwners()
{
	for(MapIndex::iterator iter = map_index.begin();iter != map_index.end();iter++)
		iter->second->PromotePlanetOwners();
}

void	Star::RemotePriceCheck(Player *player,const Commodity *commodity) const
{
	for(MapIndex::const_iterator iter = map_index.begin();iter != map_index.end();iter++)
		iter->second->RemotePriceCheck(player,commodity);
}

void	Star::Report()
{
	for(MapIndex::iterator iter = map_index.begin();iter != map_index.end();iter++)
		iter->second->Report();
}

void	Star::RunStartupEvents()
{
	for(MapIndex::iterator iter = map_index.begin();iter != map_index.end();iter++)
		iter->second->RunStartupEvents();
}

void	Star::SendXMLPlanetNames(Player *player)
{
	int num_planets = 0;
	std::ostringstream	buffer;
	for(MapIndex::iterator iter = map_index.begin();iter != map_index.end();iter++)
	{
		if(iter->second->FileName().find("/space") == std::string::npos)
		{
			if((name == "Sol") &&
							((iter->second->Title() == "Help") || (iter->second->Title() == "System") ||
							(iter->second->Title() == "Hilbert") || (iter->second->Title() == "Horsell") ||
							(iter->second->Title() == "Snark")))
				continue;

			buffer.str("");
			buffer << "<s-planet-name name='" << iter->second->Title() << "'/>\n";
			player->Send(buffer);
			num_planets++;
		}
	}

	if(num_planets == 0)
	{
		buffer.str("");
		std::ostringstream	buffer;
		buffer << "<s-planet-name name='No Planets'/>\n";
		player->Send(buffer);
	}
}

void	Star::UpdateExchanges()
{
	for(MapIndex::iterator iter = map_index.begin();iter != map_index.end();iter++)
		iter->second->UpdateExchange();
}

void	Star::Write()
{
	std::ostringstream	buffer;
	char	full_name[MAXNAMLEN];
	std::snprintf(full_name,MAXNAMLEN,"%s/maps/%s/star.inf",HomeDir(),directory.c_str());
	std::ofstream	file(full_name,std::ios::out);
	if(!file)
	{
		buffer << "Can't write to file " << full_name;
		WriteLog(buffer);
		WriteErrLog(buffer.str());
		return;
	}

	file << "<?xml version=\"1.0\"?>\n";
	file << "<star name='" << name << "' cartel='" << cartel << "'";
	if(flags.test(NO_BUILD))
		file << " build='no'";
	if(flags.test(DIVERT))
		file << " divert='yes'";
	file << ">\n\n";

	if(black_list.size() > 0)
	{
		file << "   <black-list>\n";
		for(BlackList::iterator iter = black_list.begin();iter != black_list.end();iter++)
			file << "      <exile name='" << *iter << "'/>\n";
		file << "   </black-list>\n\n";
	}

	file << "</star>" << std::endl;
	buffer.str("");
	buffer << HomeDir() << "/maps/" << directory << "/cabinet.xml";
	if(cabinet != 0)
		cabinet->Store(buffer.str());
	WriteLoaderFile();
	return;
}


/*--------------- Work in Progress for new parser ---------------*/

void	Star::WriteLoaderFile()
{
	std::ostringstream	buffer;
	buffer << HomeDir() << "/maps/" << directory << "/loader.xml";
	std::ofstream	file(buffer.str().c_str(),std::ios::out);
	if(!file)
	{
		buffer << "Can't write to file " << HomeDir() << "/maps/" << directory << "/loader.xml";;
		WriteLog(buffer);
		WriteErrLog(buffer.str());
		return;
	}

	file << "<?xml version=\"1.0\"?>\n";
	file << "<star name='" << name << "' directory='" << directory << "'>\n";
	for(MapIndex::iterator iter = map_index.begin();iter != map_index.end();++iter)
	{
		std::string	map_file_name(iter->second->FileName());
		int index = map_file_name.find_last_of("/") + 1;
		file << "   <map name='" << map_file_name.substr(index) << "'/>\n";
	}
	file << "</star>\n";
}

/*
<?xml version="1.0"?>
<star name='Sol' directory='sol'>
	<map name='brass'/>
	<map name='castillo'/>
	<map name='doris'/>
	<map name='earth'/>
	<map name='graveyard'/>
	<map name='help'/>
	<map name='hunt'/>
	<map name='lattice'/>
	<map name='magellan'/>
	<map name='magrathea'/>
	<map name='mars'/>
	<map name='mercury'/>
	<map name='moon'/>
	<map name='paradise'/>
	<map name='pearl'/>
	<map name='phobos'/>
	<map name='rhea'/>
	<map name='silk'/>
	<map name='space'/>
	<map name='starbase1'/>
	<map name='sumatra'/>
	<map name='system'/>
	<map name='titan'/>
	<map name='venus'/>
</star>
*/
