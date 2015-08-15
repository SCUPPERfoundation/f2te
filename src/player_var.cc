/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2015
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include	"player_var.h"

#include	<sstream>

#include "misc.h"
#include "output_filter.h"
#include "player.h"
#include "xml_parser.h"

PlayerVariable::~PlayerVariable()
{
	for(PlayerVarItemTable::iterator iter = pvi_table.begin();iter != pvi_table.end();++iter)
		delete iter->second;
}


void	PlayerVariable::Add(PlayerVarItem *variable)
{
	variable->last_used = time(0);
	Delete(variable->key);
	pvi_table[variable->key] = variable;
}

void	PlayerVariable::Add(const char **attrib)
{
	PlayerVarItem	*item = new PlayerVarItem;

	const std::string	*key_str = XMLParser::FindAttrib(attrib,"key");
	if(key_str != 0)
		item->key = *key_str;
	const std::string	*value_str = XMLParser::FindAttrib(attrib,"value");
	if(value_str != 0)
		item->value = *value_str;
	item->last_used = time(0);
	
	if((key_str == 0) || (value_str == 0))
	{
		std::ostringstream	buffer;
		buffer << "Corrupt entry for " << variable_name << " in disk variables table!\n";
		WriteLog(buffer);
	}
	else
	{
		Delete(item->key);
		pvi_table[item->key] = item;
	}
}

void	PlayerVariable::Delete(std::string& key) // does nothing if no such item
{
	PlayerVarItemTable::iterator iter = pvi_table.find(key);
	if(iter != pvi_table.end())
	{
		PlayerVarItem	*temp = iter->second;
		pvi_table.erase(iter);
		delete temp;
	}
}

void	PlayerVariable::Display(Player *player)
{
	std::ostringstream	buffer;
	
	buffer << variable_name << (is_temporary ? " [temporary]" : " [permanent]") << "\n";
	if(pvi_table.size() == 0)
		buffer << "  No values set for this variable.\n";
	else
	{
		for(PlayerVarItemTable::iterator iter = pvi_table.begin();iter != pvi_table.end();++iter)
		{
			buffer << "  " << iter->first << "   " << iter->second->value << " [last used ";
			buffer << ((std::time(0) - iter->second->last_used)/(60*60*24)) << " days ago]\n";
		}
	}

	if(player == 0)
		WriteLog(buffer);
	else
		player->Send(buffer,OutputFilter::DEFAULT);
}

void	PlayerVariable::Update(std::string& key,std::string& value)
{
	PlayerVarItemTable::iterator iter = pvi_table.find(key);
	if(iter != pvi_table.end())	// item already exists 
	{
		iter->second->value = value;
		iter->second->last_used = time(0);
	}
	else // create item
	{
		PlayerVarItem	*item = new PlayerVarItem;
		item->key =  key;
		item->value = value;
		item->last_used = time(0);
		pvi_table[item->key] = item;
	}
}

const std::string&	PlayerVariable::Value(std::string& key)
{
	static const std::string	unknown("unknown");

	PlayerVarItemTable::iterator iter = pvi_table.find(key);
	if(iter != pvi_table.end())
		return(iter->second->value);
	else
		return(unknown);
}

void	PlayerVariable::Write(std::ofstream& file)
{
	if(!is_temporary && (pvi_table.size() > 0))
	{
		file << "    <player-var name='" << variable_name << "'>\n";
		for(PlayerVarItemTable::iterator iter = pvi_table.begin();iter != pvi_table.end();iter++)
			file << "      <var-item key='" << iter->first << "' value='" << iter->second->value << "'/>\n";
		file << "    </player-var>" << std::endl;
	}
}

