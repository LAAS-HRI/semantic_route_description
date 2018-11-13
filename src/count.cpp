#include "ontoloGenius/utility/OntologyManipulator.h"
#include <iostream>
#include "ros/ros.h"

#include <vector>
#include <string>

#include <chrono>
#include <ctime>

int main(int argc, char** argv)
{
  ros::init(argc, argv, "count");

  ros::NodeHandle n;
  OntologyManipulator onto(&n);
  onto.close();

  std::cout << "pathIntersection => " << onto.individuals.getType("pathIntersection").size() << std::endl;
  std::cout << "path => " << onto.individuals.getType("path").size() << std::endl;
  std::cout << "shop => " << onto.individuals.getType("shop").size() << std::endl;
  std::cout << "interface => " << onto.individuals.getType("interface").size() << std::endl;
  std::cout << "region => " << onto.individuals.getType("region").size() << std::endl;
  std::cout << "place => " << onto.individuals.getType("place").size() << std::endl;

  ros::spin();

  return 0;
}
