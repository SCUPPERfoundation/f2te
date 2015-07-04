/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2015
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef DISAFFECTION_H
#define DISAFFECTION_H

class	Infrastructure;
class Player;

class Disaffection
{
private:
	Infrastructure	*infra;

	int	discontent;

	int	airlane;
	int	atmosphere;
	int	biodiv;
	int	coal;
	int	dole;
	int	floating;
	int	genetic;
	int	informer;
	int	insurance;
	int	leisure;
	int	longevity;
	int	oil;
	int	pension;
	int	police;
	int	pollution;
	int	radio;
	int	riot_police;
	int 	solar;
	int	surveillance;
	int	weather;

	int	airlane_change;
	int	atmosphere_change;
	int	biodiv_change;
	int	coal_change;
	int	dole_change;
	int	floating_change;
	int	genetic_change;
	int	informer_change;
	int	insurance_change;
	int	leisure_change;
	int	longevity_change;
	int	oil_change;
	int	pension_change;
	int	police_change;
	int	pollution_change;
	int	radio_change;
	int	riot_police_change;
	int	solar_change;
	int	surveillance_change;
	int	unemployment_change;
	int	weather_change;

	void	CalculateDiscontent();

	int	CalculateAirLane()		{ discontent -= airlane;	 	return(-airlane);		}
	int	CalculateAtmos();
	int	CalculateBioDiv()			{ discontent -= biodiv;			return(-biodiv);		}
	int	CalculateCoal();
	int	CalculateDole()			{ discontent -= (dole * 3);	return(-(dole * 3));	}
	int	CalculateFloating()		{ discontent += floating;	 	return(floating);		}
	int	CalculateGenetic()		{ discontent -= genetic; 		return(-genetic);		}
	int	CalculateInformer()		{ discontent -= informer;		return(-informer);	}
	int	CalculateInsurance()		{ discontent -= insurance; 	return(-insurance);	}
	int	CalculateLeisure()		{ discontent -= leisure;		return(-leisure);		}
	int	CalculateLongevity();
	int	CalculateOil()				{ discontent += oil; 			return(oil);			}
	int	CalculatePension()		{ discontent -= pension; 		return(-pension);		}
	int	CalculatePolice();
	int	CalculatePollution();
	int	CalculateRadio()			{ discontent -= radio; 			return(-radio);		}
	int	CalculateRiotPolice();
	int	CalculateSurveillance();
	int	CalculateSolar()			{ discontent += solar; 			return(solar);			}
	int	CalculateUnemployment();
	int	CalculateWeather()		{ discontent -= weather; 		return(-weather);		}

public:
	Disaffection(Infrastructure *infrastructure);
	~Disaffection()	{	}

	int 	Update();

	void	Display(Player *player);
	void	TotalAirLanePoints(int num_pts)				{	airlane += num_pts;				}
	void	TotalAtmosPoints(int num_pts)					{	atmosphere += num_pts;			}
	void	TotalBioDivPoints(int num_pts)				{	biodiv += num_pts;				}
	void	TotalCoalPoints(int num_pts)					{	coal += num_pts;					}
	void	TotalDolePoints(int num_pts)					{	dole += num_pts;					}
	void	TotalFloatingPoints(int num_pts)				{	floating += num_pts;				}
	void	TotalGeneticPoints(int num_pts)				{	genetic += num_pts;				}
	void	TotalInformerPoints(int num_pts)				{	informer += num_pts;				}
	void	TotalInsurancePoints(int num_pts)			{	insurance += num_pts;			}
	void	TotalLeisurePoints(int num_pts)				{	leisure += num_pts;				}
	void	TotalLongevityPoints(int num_pts)			{	longevity += num_pts;			}
	void	TotalOilPoints(int num_pts)					{	oil += num_pts;					}
	void	TotalPensionPoints(int num_pts)				{	pension += num_pts;				}
	void	TotalPolicePoints(int num_pts)				{	police += num_pts;				}
	void	TotalPollutionPoints(int num_pts)			{	pollution += num_pts;			}
	void	TotalRadioPoints(int num_pts)					{  radio += num_pts;					}
	void	TotalRiotPolicePoints(int num_points)		{	riot_police += num_points;		}
	void	TotalSolarPoints(int num_points)				{	solar += num_points;				}
	void	TotalSurveillancePoints(int num_points)	{	surveillance += num_points;	}
	void	TotalWeatherPoints(int num_points)			{	weather += num_points;			}
};

#endif
