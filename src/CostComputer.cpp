#include "route_description/CostComputer.h"

#include <iostream>
#include <limits>
#include <map>

const struct Personna CostComputer::lambda_persona = {"lambda", {0.2,0.2,0.3,0.2,0.2,0.6}};
const struct Personna CostComputer::disabled_persona = {"disabled", {0.5,2.0,0.9,0.7,0.5,0.0}};
const struct Personna CostComputer::knowing_persona = {"knowing", {0.1,0.5,0.6,0.5,0.2,0.7}};
const struct Personna CostComputer::not_knowing_persona = {"notKnowing", {0.9,0.5,0.2,0.7,0.7,0.2}};
const struct Personna CostComputer::young_persona = {"young", {0.3,0.5,0.2,0.9,0.6,0.9}};
const struct Personna CostComputer::old_persona = {"old", {0.3,0.8,0.9,0.9,0.6,0.1}};

CostComputer::CostComputer(onto::OntologyManipulator* onto)
{
  onto_ = onto;

  costs_.salient = 0;
  costs_.accessible = 0;
  costs_.confortable = 0;
  costs_.secured = 0;
  costs_.explicable = 0;
  costs_.fast = 0;
}

std::vector<std::string> CostComputer::splitPersonnas(std::string& text)
{
  std::vector<std::string> personas;
  size_t start = 0;
  size_t stop = 0;

  while(stop != std::string::npos)
  {
    stop = text.find("_", start);
    if(stop != std::string::npos)
    {
      std::string persona = text.substr(start, stop - start);
      personas.push_back(persona);

      start = stop +1;
    }
  }
  std::string persona = text.substr(start, stop - start);
  personas.push_back(persona);

  for(size_t i = 0; i < personas.size(); i++)
    std::cout << personas[i] << std::endl;

  return personas;
}

void CostComputer::getParam(std::string& persona_name)
{
  if(persona_name == lambda_persona.name)
    updateCosts(lambda_persona.costs);
  else if(persona_name == disabled_persona.name)
    updateCosts(disabled_persona.costs);
  else if(persona_name == knowing_persona.name)
    updateCosts(knowing_persona.costs);
  else if(persona_name == not_knowing_persona.name)
    updateCosts(not_knowing_persona.costs);
  else if(persona_name == young_persona.name)
    updateCosts(young_persona.costs);
  else if(persona_name == old_persona.name)
    updateCosts(old_persona.costs);
}

void CostComputer::updateCosts(struct Costs cost)
{
  if(cost.salient > costs_.salient)
    costs_.salient = cost.salient;

  if(cost.accessible > costs_.accessible)
    costs_.accessible = cost.accessible;

  if(cost.confortable > costs_.confortable)
    costs_.confortable = cost.confortable;

  if(cost.secured > costs_.secured)
    costs_.secured = cost.secured;

  if(cost.explicable > costs_.explicable)
    costs_.explicable = cost.explicable;

  if(cost.fast > costs_.fast)
    costs_.fast = cost.fast;
}

void CostComputer::putInRange()
{
  costs_.salient++;
  costs_.accessible++;
  costs_.confortable++;
  costs_.secured++;
  costs_.explicable++;
  costs_.fast++;
}

float CostComputer::getParamCost(std::string& param)
{
  float cost = 1;
  if(param == "salient")
    cost = costs_.salient;
  else if(param == "insignifiant")
    cost = 1/costs_.salient;
  else if(param == "accessible")
    cost = costs_.accessible;
  else if(param == "inaccessible")
    cost = 1/costs_.accessible;
  else if(param == "confortable")
    cost = costs_.confortable;
  else if(param == "unconfortable")
    cost = 1/costs_.confortable;
  else if(param == "secured")
    cost = costs_.confortable;
  else if(param == "unsecured")
    cost = 1/costs_.confortable;
  else if(param == "explicable")
    cost = costs_.confortable;
  else if(param == "unexplicable")
    cost = 1/costs_.confortable;
  else if(param == "fast")
    cost = costs_.fast;
  else if(param == "slow")
    cost = 1/costs_.fast;

  if((cost > 2) || (cost < 1/2.))
    return std::numeric_limits<float>::infinity();
  else
    return 1/cost;
}

std::vector<float> CostComputer::compute(routes_t& routes, std::vector<std::string>& gloals, std::string& goal, std::string personas)
{
  std::vector<std::string> personas_splitted = splitPersonnas(personas);
  for(size_t i = 0; i < personas_splitted.size(); i++)
    getParam(personas_splitted[i]);

  putInRange();

  std::vector<float> result;
  for(size_t route_i = 0; route_i < routes.size(); route_i++)
    if(gloals[route_i] == goal)
      result.push_back(routes[route_i].size());
    else
      result.push_back(routes[route_i].size() * 2); //signpost over-cost

  float elementCost;
  std::vector<std::string> havingCost = onto_->individuals.getRelatedFrom("hasCost");
  for(size_t i = 0; i < havingCost.size(); i++)
  {
    elementCost = 1.0;
    std::vector<std::string> cost = onto_->individuals.getOn(havingCost[i], "hasCost");
    for(size_t j = 0; j < cost.size(); j++)
      elementCost = elementCost * getParamCost(cost[j]);

    for(size_t route_i = 0; route_i < routes.size(); route_i++)
    {
      if(std::find(routes[route_i].begin(), routes[route_i].end(), havingCost[i]) != routes[route_i].end())
        result[route_i] = result[route_i]*elementCost;
    }
  }

  return result;
}

std::vector<float> CostComputer::compute(routes_t& routes, std::vector<std::string>& gloals, std::string& goal, std::string& personas, std::vector<float>& pre_costs)
{
  std::vector<std::string> personas_splitted = splitPersonnas(personas);
  for(size_t i = 0; i < personas_splitted.size(); i++)
    getParam(personas_splitted[i]);

  putInRange();

  std::vector<float> result;
  size_t nb_routes = routes.size();
  for(size_t route_i = 0; route_i < nb_routes; route_i++)
    if(gloals[route_i] == goal)
      result.push_back(routes[route_i].size());
    else
      result.push_back(routes[route_i].size() * 2); //signpost over-cost

  float elementCost;
  std::vector<std::string> havingCost = onto_->individuals.getRelatedFrom("hasCost");
  for(size_t i = 0; i < havingCost.size(); i++)
  {
    elementCost = 1.0;
    std::vector<std::string> cost = onto_->individuals.getOn(havingCost[i], "hasCost");
    for(size_t j = 0; j < cost.size(); j++)
    {
      elementCost = elementCost * getParamCost(cost[j]);
    }

    for(size_t route_i = 0; route_i < nb_routes; route_i++)
    {
      if(std::find(routes[route_i].begin(), routes[route_i].end(), havingCost[i]) != routes[route_i].end())
        result[route_i] = result[route_i]*elementCost;
    }
  }

  if(result.size() == pre_costs.size())
  {
    for(size_t i = 0; i < result.size(); i++)
      result[i] = result[i] * pre_costs[i];
  }
  else
    std::cout << "Pre_costs not took into account : " << result.size() << " | " << pre_costs.size() << std::endl;

  return result;
}
