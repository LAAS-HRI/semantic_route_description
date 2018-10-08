#ifndef PATHFINDER_H
#define PATHFINDER_H

#include <string>
#include <vector>
#include <map>

#include "ros/ros.h"

#include "ontoloGenius/utility/OntologyManipulator.h"
#include "route_description/Routes.h"

class PathFinder
{
public:
  PathFinder(ros::NodeHandle* n);
  ~PathFinder() {}

  void find(std::string from_place, std::string to_place, std::string personas, bool signpost, route_t region_route);
  void find(std::string from_place, std::string to_place, std::string personas, bool signpost = false);
  void find(std::string from_place, std::string to_place, route_t region_route);
  void findRegions(std::string from_place, std::string to_place, std::string personas, bool signpost = false);
  void findRegions(std::string from_place, std::string to_place);

  routes_t getRoutes() {return completed_routes_; }
  std::vector<float> getCosts() {return costs_; }
  std::vector<std::string> getGoals() {return goals_; }

private:
  ros::NodeHandle* n_;
  OntologyManipulator onto_;

  std::vector<std::string> from_region_;
  std::vector<std::string> to_region_;
  std::string personas_;
  routes_t routes_;
  std::map<std::string, routes_t> place2place_;
  std::vector<float> regions_costs_step_;
  std::vector<float> regions_costs_;

  size_t completed_routes_step_index_;
  routes_t completed_routes_;
  std::vector<float> costs_;
  std::vector<std::string> goals_;

  void init();
  void to_regions(std::string from_place, std::string to_place);
  void getRegionRoutes();
  void appendFromAndTo(std::string from_place, std::string to_place);
  void createPlace2Place();
  void getCompleteRoutes(std::string to_place);
  void printFinalRoutes();
  void computeCost(std::string goal);

  bool testToPlace(std::string to_place);
  std::string toPlace(std::string name);

  void getFineRoutes();
};

#endif
