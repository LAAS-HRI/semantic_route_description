#ifndef CORRIDORPATHFINDER_H
#define CORRIDORPATHFINDER_H

#include "ontologenius/OntologyManipulator.h"
#include "route_description/Routes.h"

#include <vector>
#include <string>

class CorridorPathfinder
{
public:
  CorridorPathfinder(onto::OntologyManipulator* onto) {onto_ = onto; }
  ~CorridorPathfinder(){}

  routes_t find(std::string& my_corridor, std::string& goal_corridor, size_t& max_step);

  void displayRoutes(routes_t& routes);
  void displayRoute(route_t& route);

private:
  onto::OntologyManipulator* onto_;
};

#endif
