#include <iostream>
#include <map>

#include "tourism.h"
#include "pathfinder.h"
#include "utils.h"
#include "database.h"

struct TouristicClosestNode
{
	double distance;
	ResultNode *closest;
	TouristicPlace *place;
};

static bool getBoundingCoords(const std::vector<Coordinates> &path, QTouristicLocationsOptions &options)
{
	const double MARGIN_LONGITUDE = 0.01;
	const double MARGIN_LATITUDE = 0.01;
	
	std::vector<Coordinates>::const_iterator it = path.begin();
	if(it != path.end())
	{
		options.minLongitude = it->longitude;
		options.maxLongitude = it->longitude;
		options.minLatitude = it->latitude;
		options.maxLatitude = it->latitude;

		for(++it; it != path.end(); ++it)
		{
			if(it->longitude < options.minLongitude)
				options.minLongitude = it->longitude;
			if(it->longitude > options.maxLongitude)
				options.maxLongitude = it->longitude;
			if(it->latitude < options.minLatitude)
				options.minLatitude = it->latitude;
			if(it->latitude > options.maxLatitude)
				options.maxLatitude = it->latitude;
		}

		options.minLongitude -= MARGIN_LONGITUDE;
		options.maxLongitude += MARGIN_LONGITUDE;
		options.minLatitude -= MARGIN_LATITUDE;
		options.maxLatitude += MARGIN_LATITUDE;

		return true;
	}
	return false;
}

static bool getTouristicClosestNodeOf(const TouristicPlace &place, const Path &resultPath, TouristicClosestNode &closestNode)
{
	ResultNode *node = resultPath.result;
	double minDist;

	closestNode.place = (TouristicPlace*)&place;
	closestNode.closest = node;
	if(!node)
		return false;	
	
	minDist = squareDist2(place.location, *(node->point));
	while(node)
	{
		double tmp = squareDist2(place.location, *(node->point));
		if(tmp < minDist)
		{
			minDist = tmp;
			closestNode.closest = node;
		}
		node = node->next;
	}

	closestNode.distance = minDist;

	return true;
}

bool BuildTouristicPath(const Path &resultPath, const std::vector<Coordinates> &initialPath, std::vector<Coordinates> &finalPath, std::vector<TouristicPlace> &places, const TouristicFilter &filter, Database *db)
{
	const unsigned int N_TOURISTIC_PLACES = 3;

	QTouristicLocationsOptions options = {0, 0, 0, 0, filter};
	std::vector<TouristicPlace> touristicPlaces;

	if(!filter.gastronomy && !filter.patrimony && !filter.accomodation)
		return false;

	getBoundingCoords(initialPath, options);

	pthread_mutex_t m;
	QuerySQL query;
	query.options = options;
	query.places = &touristicPlaces;
	query.mutex = &m;

	pthread_mutex_init(&m, 0);
	pthread_mutex_lock(&m);
	db->AddQuery(query);
	pthread_mutex_lock(&m);

	if(touristicPlaces.size() > 0)
	{
		std::multimap<double, TouristicClosestNode> orderedPlaces;
		std::multimap<double, TouristicClosestNode*> orderedPlacesFromStart;
		unsigned int i = 0;
		const Coordinates **points;
		bool error = false;
		Path newPath;
		
		// Find the minimal distance between each place and all the intersections
		for(std::vector<TouristicPlace>::const_iterator it = touristicPlaces.begin();
			it != touristicPlaces.end();
			++it)
		{
			TouristicClosestNode closestNode;
			if(getTouristicClosestNodeOf(*it, resultPath, closestNode))
				orderedPlaces.insert(std::pair<double, TouristicClosestNode>(closestNode.distance, closestNode));
		}

		// Choose only the closest ones
		places.clear();
		places.resize(N_TOURISTIC_PLACES);
		for(std::multimap<double, TouristicClosestNode>::iterator it = orderedPlaces.begin();
			it != orderedPlaces.end() && i < N_TOURISTIC_PLACES;
			++it, ++i)
		{
			double dist = squareDist2(*(resultPath.realStart), it->second.place->location);
			orderedPlacesFromStart.insert(std::pair<double, TouristicClosestNode*>(dist, &(it->second)));
			places[i] = *(it->second.place);
		}
	
		// Create points array	
		points = new const Coordinates*[2 + orderedPlacesFromStart.size()]; // +2 because of start and goal
		points[0] = resultPath.realStart;
		points[1 + orderedPlacesFromStart.size()] = resultPath.realGoal;
		i = 1;
		for(std::multimap<double, TouristicClosestNode*>::const_iterator it = orderedPlacesFromStart.begin();
			it != orderedPlacesFromStart.end();
			++it, ++i)
		{
			points[i] = &(it->second->place->location);
		}

		// Create subpaths
		for(i = 0; i < 1 + N_TOURISTIC_PLACES && !error; ++i)
		{
			Path subpath;
			if(!PathFinder::Astar(*(points[i]), *(points[i+1]), i == 0 ? newPath : subpath))
				error = true;
			else if (i != 0) // Extend the first path
			{
				ResultNode *node;
				ResultNode *prec = 0;

				for(node = newPath.result; node; node = node->next)
					prec = node;

				if(!prec) // Should be always false
				{
					std::cerr << "[TouristicPath] An unknown error has occurred..." << std::endl;
					error = true;
				}
				else
				{
					prec->next = subpath.result;
					newPath.realGoal = subpath.realGoal;
					newPath.closestCoordGoal = subpath.closestCoordGoal;
				}
			}
		}
		delete [] points;
	
		if(error)
			return false;

		finalPath.clear();
		if(!PathFinder::BuildPath(newPath, finalPath))
			return false;
		FreePathResult(&newPath);

		return true;
	}
	else 
	{
		return false;
	}

	return false;
}

