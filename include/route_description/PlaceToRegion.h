#ifndef PLACETOREGION_H
#define PLACETOREGION_H

#include "ontologenius/OntologyManipulator.h"

#include <vector>
#include <string>

class PlaceToRegion
{
public:
  PlaceToRegion(OntologyManipulator* onto) {onto_ = onto; }
  ~PlaceToRegion(){}

  std::vector<std::string> place2region(std::string place);

private:
  OntologyManipulator* onto_;

};

#endif
