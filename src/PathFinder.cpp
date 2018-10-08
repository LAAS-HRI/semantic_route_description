#include "route_description/PathFinder.h"

#include <iostream>
#include <chrono>
#include <ctime>
#include <algorithm>

#include "route_description/PlaceToRegion.h"
#include "route_description/RegionPathfinder.h"
#include "route_description/PlacePathfinder.h"
#include "route_description/CostComputer.h"

PathFinder::PathFinder(ros::NodeHandle* n) : onto_(n)
{
  n_ = n;
  onto_.close();
}

void PathFinder::find(std::string from_place, std::string to_place, std::string personas, bool signpost, route_t region_route)
{
  if(region_route.size() == 0)
    find(from_place, to_place, personas, signpost);
  else
  {
    personas_ = personas;

    std::vector<std::string> to_places;
    to_places.push_back(to_place);

    for(size_t i = 0; i < to_places.size(); i++)
    {
      find(from_place, to_places[i], region_route);
      std::cout << to_places[i] << std::endl;
    }
  }
}

void PathFinder::find(std::string from_place, std::string to_place, std::string personas, bool signpost)
{
  personas_ = personas;
  completed_routes_step_index_ = 0;

  std::vector<std::string> to_places;
  if(signpost == true)
  {
    to_places = onto_.individuals.getOn(to_place, "isReferencedBy");
  }
  to_places.push_back(to_place);

  route_t empty_route;
  for(size_t i = 0; i < to_places.size(); i++)
  {
    find(from_place, to_places[i], empty_route);
    std::cout << to_places[i] << std::endl;
  }
  computeCost(to_place);
  std::cout << "printFinalRoutes" << std::endl;
}

void PathFinder::find(std::string from_place, std::string to_place, route_t region_route)
{
  std::chrono::time_point<std::chrono::system_clock> start, end;
  start = std::chrono::system_clock::now();

  if(testToPlace(to_place) == true)
  {
    if(testToPlace(from_place) == true)
    {
      init();
      to_regions(from_place, to_place);
      if(region_route.size() == 0)
        getRegionRoutes();
      else
        routes_.push_back(region_route);

      for(size_t i = 0; i < routes_.size(); i++)
      {
        regions_costs_step_.push_back((routes_[i].size() - 1) / 2. + 1);
      }

      appendFromAndTo(from_place, to_place);

      createPlace2Place();
      getCompleteRoutes(to_place);
      std::cout << "getCompleteRoutes" << std::endl;
      getFineRoutes();
      std::cout << "getFineRoutes" << std::endl;
      printFinalRoutes();

      completed_routes_step_index_ = completed_routes_.size();
    }
    else
      std::cout << "[ERROR] initial position must be a place" << std::endl;
  }
  else
    std::cout << "[ERROR] goal must be a place" << std::endl;

  end = std::chrono::system_clock::now();
  int elapsed_microseconds = std::chrono::duration_cast<std::chrono::microseconds>(end-start).count();
  std::time_t end_time = std::chrono::system_clock::to_time_t(end);

  std::cout << "finished computation at " << std::ctime(&end_time)
            << "elapsed time: " << elapsed_microseconds << "us\n";

  std::cout << onto_.individuals.nb() << " requests done" << std::endl;
  onto_.reset();
}

void PathFinder::findRegions(std::string from_place, std::string to_place, std::string personas, bool signpost)
{
  personas_ = personas;

  std::vector<std::string> to_places;
  if(signpost == true)
  {
    to_places = onto_.individuals.getOn(to_place, "isReferencedBy");
  }
  to_places.push_back(to_place);

  for(size_t i = 0; i < to_places.size(); i++)
    findRegions(from_place, to_places[i]);
}

void PathFinder::findRegions(std::string from_place, std::string to_place)
{
  std::chrono::time_point<std::chrono::system_clock> start, end;
  start = std::chrono::system_clock::now();

  if(testToPlace(to_place) == true)
  {
    init();
    to_regions(from_place, to_place);
    getRegionRoutes();

    for(size_t i = 0; i < routes_.size(); i++)
    {
      completed_routes_.push_back(routes_[i]);
      goals_.push_back(to_place);
    }

    computeCost(to_place);
  }
  else
    std::cout << "[ERROR] goal must be a place" << std::endl;

  end = std::chrono::system_clock::now();
  int elapsed_microseconds = std::chrono::duration_cast<std::chrono::microseconds>(end-start).count();
  std::time_t end_time = std::chrono::system_clock::to_time_t(end);

  std::cout << "finished computation at " << std::ctime(&end_time)
            << "elapsed time: " << elapsed_microseconds << "us\n";

  std::cout << onto_.individuals.nb() << " requests done" << std::endl;
  onto_.reset();
}

void PathFinder::init()
{
  from_region_.clear();
  to_region_.clear();
  routes_.clear();
  regions_costs_step_.clear();
}

void PathFinder::to_regions(std::string from_place, std::string to_place)
{
  PlaceToRegion place_to_region(&onto_);

  if(onto_.individuals.isA(from_place, "place"))
    from_region_ = place_to_region.place2region(from_place);
  else if(onto_.individuals.isA(from_place, "path"))
  {
    std::vector<std::string> tmp = onto_.individuals.getOn(from_place, "isIn");
    if(tmp.size())
      from_region_.push_back(tmp[0]);
  }
  else if(onto_.individuals.isA(from_place, "region"))
    from_region_.push_back(from_place);

  to_region_ = place_to_region.place2region(to_place);

  std::cout << "From :";
  for(size_t i = 0; i < from_region_.size(); i++)
    std::cout << from_region_[i] << " ";
  std::cout << std::endl;

  std::cout << "To   :";
  for(size_t i = 0; i < to_region_.size(); i++)
    std::cout << to_region_[i] << " ";
  std::cout << std::endl;
}

void PathFinder::getRegionRoutes()
{
  RegionPathfinder region_pathfinder(&onto_);
  for(size_t from = 0; from < from_region_.size(); from++)
    for(size_t to = 0; to < to_region_.size(); to++)
    {
      routes_t tmp = region_pathfinder.find(from_region_[from], to_region_[to]);
      routes_.insert(routes_.end(), tmp.begin(), tmp.end());
    }
}

void PathFinder::appendFromAndTo(std::string from_place, std::string to_place)
{
  for(size_t i = 0; i < routes_.size(); i++)
  {
    routes_[i].insert(routes_[i].begin(), from_place);
    routes_[i].push_back(to_place);
  }
}

void PathFinder::createPlace2Place()
{
  PlacePathfinder place_pathfinder(&onto_);

  for(size_t i = 0; i < routes_.size(); i++)
  {
    for(size_t place_i = 0; place_i + 2 < routes_[i].size(); place_i+=2)
    {
      std::string id = routes_[i][place_i] + ":" + routes_[i][place_i + 1] + ":" + routes_[i][place_i + 2];
      if(place2place_.find(id) == place2place_.end())
      {
        routes_t corridors = place_pathfinder.find(routes_[i][place_i], routes_[i][place_i + 2], routes_[i][place_i + 1]);
        place2place_[id] = corridors;
      }
    }
  }
}

void PathFinder::getCompleteRoutes(std::string to_place)
{
  std::vector<float> regions_costs;
  for(size_t i = 0; i < routes_.size(); i++)
  {
    routes_t tmp_routes;
    {
      std::vector<std::string> tmp;
      tmp.push_back(routes_[i][0]);
      tmp_routes.push_back(tmp);
    }
    for(size_t place_i = 0; place_i + 2 < routes_[i].size(); place_i+=2)
    {
      std::string id = routes_[i][place_i] + ":" + routes_[i][place_i + 1] + ":" + routes_[i][place_i + 2];

      if(place2place_[id].size() != 0)
        append(tmp_routes, place2place_[id]);
      else
        append(tmp_routes, routes_[i][place_i+1]);
      append(tmp_routes, routes_[i][place_i+2]);
    }
    completed_routes_.insert(completed_routes_.end(), tmp_routes.begin(), tmp_routes.end());
    for(size_t route_i = 0; route_i < tmp_routes.size(); route_i++)
    {
      goals_.push_back(to_place);
      regions_costs.push_back(regions_costs_step_[i]);
    }
  }
  regions_costs_.insert(regions_costs_.end(), regions_costs.begin(), regions_costs.end());
}

void PathFinder::printFinalRoutes()
{
  std::cout << " completed routes are : " << std::endl;
  RegionPathfinder region_pathfinder(&onto_);
  region_pathfinder.displayRoutes(completed_routes_);
}

void PathFinder::computeCost(std::string goal)
{
  CostComputer cost(&onto_, n_);
  costs_ = cost.compute(completed_routes_, goals_, goal, personas_, regions_costs_);

  for(size_t route_i = 0; route_i < costs_.size(); route_i++)
    std::cout << costs_[route_i] << std::endl;
}

bool PathFinder::testToPlace(std::string to_place)
{
  return onto_.individuals.isA(to_place, "place");
}

void PathFinder::getFineRoutes()
{
  routes_t tmp = completed_routes_;
  for(size_t i = completed_routes_step_index_; i < completed_routes_.size();)
  {
    bool erase = false;
    for(size_t j = 0; j < completed_routes_[i].size(); j++)
      if(onto_.individuals.isA(completed_routes_[i][j], "region") == true)
      {
        erase = true;
        break;
      }

    if(erase == true)
    {
      completed_routes_.erase(completed_routes_.begin() + i);
      goals_.erase(goals_.begin() + i);
      regions_costs_.erase(regions_costs_.begin() + i);
    }
    else
      i++;
  }

  if(completed_routes_.size() - completed_routes_step_index_ == 0)
    completed_routes_ = tmp;
}
