#ifndef REGIONPATHFINDER_H
#define REGIONPATHFINDER_H

#include "ontologenius/OntologyManipulator.h"
#include "route_description/Routes.h"

#include <vector>
#include <string>

class RegionPathfinder
{
public:
  RegionPathfinder(OntologyManipulator* onto) {onto_ = onto; }
  ~RegionPathfinder(){}

  routes_t find(std::string my_region, std::string goal_region);

  void displayRoutes(routes_t routes);
  void displayRoute(route_t route);

private:
  OntologyManipulator* onto_;
};

#endif
