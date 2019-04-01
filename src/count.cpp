#include "ontoloGenius/utility/OntologyManipulator.h"
#include <iostream>
#include "ros/ros.h"

#include <vector>
#include <string>

#include <chrono>
#include <ctime>

#include <curl/curl.h>
#include <fstream>
#include <sstream>

size_t callbackfunction(void *ptr, size_t size, size_t nmemb, void* userdata)
{
    FILE* stream = (FILE*)userdata;
    if (!stream)
    {
        printf("!!! No stream\n");
        return 0;
    }

    size_t written = fwrite((FILE*)ptr, size, nmemb, stream);
    return written;
}

bool download(const char* url, const char* out)
{
    FILE* fp = fopen(out, "wb");
    if (!fp)
    {
        printf("!!! Failed to create file on the disk\n");
        return false;
    }

    CURL* curlCtx = curl_easy_init();
    curl_easy_setopt(curlCtx, CURLOPT_URL, url);
    curl_easy_setopt(curlCtx, CURLOPT_WRITEDATA, fp);
    curl_easy_setopt(curlCtx, CURLOPT_WRITEFUNCTION, callbackfunction);
    curl_easy_setopt(curlCtx, CURLOPT_FOLLOWLOCATION, 1);

    CURLcode rc = curl_easy_perform(curlCtx);
    if (rc)
    {
        printf("!!! Failed to download: %s\n", url);
        return false;
    }

    long res_code = 0;
    curl_easy_getinfo(curlCtx, CURLINFO_RESPONSE_CODE, &res_code);
    if (!((res_code == 200 || res_code == 201) && rc != CURLE_ABORTED_BY_CALLBACK))
    {
        printf("!!! Response code: %d\n", res_code);
        return false;
    }

    curl_easy_cleanup(curlCtx);

    fclose(fp);

    return true;
}

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

  size_t done = 0;
  size_t all = 0;

  std::cout << std::endl << "list of place with URL: " << std::endl;
  std::vector<std::string> places = onto.individuals.getType("place");
  for(auto& place : places)
  {
    if(!onto.individuals.isA(place, "pathIntersection"))
      if(!onto.individuals.isA(place, "interface"))
      {
        std::vector<std::string> logos = onto.individuals.getOn(place, "hasLogoURL");
        if(logos.size() > 0)
        {
          done++;
          std::cout << "\t-" << place << std::endl;
          for(auto& logo : logos)
          {
            logo = logo.substr(7);
            std::cout << "\t\t-" << logo << std::endl;
            size_t split = 0;
            size_t last_split = 0;
            while((split = logo.find("/", last_split)) != std::string::npos)
              last_split = split + 1;
            std::string file_name = logo.substr(last_split);
            std::cout << " : " << file_name << " : " << std::endl;
            //download(logo.c_str(), file_name.c_str());
          }
        }
      }
  }

  std::cout << std::endl << "list of place without URL: " << std::endl;
  for(auto& place : places)
  {
    if(!onto.individuals.isA(place, "pathIntersection"))
      if(!onto.individuals.isA(place, "interface"))
      {
        all++;
        std::vector<std::string> logos = onto.individuals.getOn(place, "hasLogoURL");
        if(logos.size() < 1)
          std::cout << "\t-" << place << std::endl;
      }
  }

  std::cout << done << "/" << all << std::endl;

  ros::spin();

  return 0;
}

/*<ontologenius:hasLogoURL rdf:datatype="http://www.w3.org/2001/XMLSchema#string"></ontologenius:hasLogoURL>*/
