#ifndef COSTCOMPUTER_H
#define COSTCOMPUTER_H

#include "ontologenius/OntologyManipulator.h"
#include "route_description/Routes.h"

#include "ros/ros.h"
#include <string>
#include <vector>

struct Costs
{
  float salient;
  float accessible;
  float confortable;
  float secured;
  float explicable;
  float fast;
};

struct Personna
{
  std::string name;
  struct Costs costs;
};

class CostComputer
{
  static const struct Personna lambda_persona;
  static const struct Personna disabled_persona;
  static const struct Personna knowing_persona;
  static const struct Personna not_knowing_persona;
  static const struct Personna young_persona;
  static const struct Personna old_persona;

public:
  CostComputer(onto::OntologyManipulator* onto, ros::NodeHandle* n);
  ~CostComputer(){}

  std::vector<float> compute(routes_t& routes, std::vector<std::string>& gloals, std::string& goal, std::string personas = "");
  std::vector<float> compute(routes_t& routes, std::vector<std::string>& gloals, std::string& goal, std::string& personas, std::vector<float>& pre_costs);

private:
  onto::OntologyManipulator* onto_;
  ros::NodeHandle* n_;

  struct Costs costs_;

  std::vector<std::string> splitPersonnas(std::string& text);
  void getParam(std::string& persona_name);
  void updateCosts(struct Costs cost);
  float getParamCost(std::string& param);
  void putInRange();
};

#endif
