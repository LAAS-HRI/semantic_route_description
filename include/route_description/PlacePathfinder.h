#ifndef PLACEPATHFINDER_H
#define PLACEPATHFINDER_H

#include "ontologenius/OntologyManipulator.h"
#include "route_description/Routes.h"

#include <vector>
#include <string>

class PlacePathfinder
{
public:
  PlacePathfinder(OntologyManipulator* onto) {onto_ = onto; }
  ~PlacePathfinder(){}

  routes_t find(std::string& my_place, std::string& goal_place, std::string& region);

private:
  OntologyManipulator* onto_;

  void removeNotInRegion(std::vector<std::string>& corridors, std::string& region);
};

#endif
