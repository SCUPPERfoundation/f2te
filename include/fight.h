/*-----------------------------------------------------------------------
		       Copyright (c) Alan Lenton 1985-2016
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef FIGHT_H
#define FIGHT_H

#include "equipment.h"
#include "fight_info.h"
#include "loc_rec.h"

class Player;

// Handler for a single fight - does no updating - just computes the result
class Fight
{
private:
	static const int MISSILE_BASE_HIT = 50;
	static const int DEFENCE_LASER_HIT = 15;
	static const int MAX_TELEMETRY = 5;

	enum 	{ LASER_DIST, INTERMED_DIST_1, INTERMED_DIST_2, MISSILE_DIST };
	enum	{ CLOSING_IN, MOVING_OUT, NOT_MOVING };

	LocRec	loc_rec;
	Player	*aggressor;
	Player	*victim;
	std::string		aggressor_name;
	std::string		victim_name;
	int		spacing;
	int 		aggressor_telemetry;
	int 		victim_telemetry;

	FightInfoIn		attacker_in;
	FightInfoIn		defender_in;
	FightInfoOut	defender_out;

	void	CalculateDamage();
	void	ClearFightInfoIn(FightInfoIn& info);
	void	ClearFightInfoOut(FightInfoOut& info);

public:
	Fight(const LocRec& loc, Player *att, Player *def);
	~Fight()	{	}

	Player	*GetOtherPlayer(Player *player);
	bool		Launch(Player *player); // true indicates missile actually launched
	bool		Participant(Player *att,Player *def);

	void		Flee(Player *player);
	void		CloseRange(Player *player);
	void		OpenRange(Player *player);
	void		Fire(Player *player, int what);
};

#endif
