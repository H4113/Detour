#include "tourism.h"
#include "database.h"
#include "pathfinder.h"
#include "utils.h"

static bool getBoundingCoords(const std::vector<Coordinates> &path, QTouristicLocationsOptions &options)
{
	std::vector<Coordinates>::const_iterator it = path.begin();
	if(it != path.end())
	{
		options.minLongitude = it->longitude;
		options.maxLongitude = it->longitude;
		options.minLatitude = it->latitude;
		options.maxLatitude = it->latitude;

		for(; it != path.end(); ++it)
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

		return true;
	}
	return false;
}

void BuildTouristicPath(const ResultNode *result, const std::vector<Coordinates> &initialPath, std::vector<Coordinates> &finalPath, std::vector<TouristicPlace> &places, const TouristicFilter &filter)
{
	QTouristicLocationsOptions options = {0, 0, 0, 0, filter};
	
	getBoundingCoords(initialPath, options);
	
	finalPath.clear();
}

