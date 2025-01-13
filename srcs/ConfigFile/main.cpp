#include "ConServer.hpp"
#include <cstdlib>
#include <vector>


int main (int ac, char **av)
{
	if (ac != 2)
		return (std::cout<< "bad arguments\n", 1);
	try{

		std::vector<Conserver> servers(	parseConfigFile(av[1]));
		std::map<std::string, std::string> Location = servers[0].getLocation("/upload");
		for (std::map<std::string, std::string>::iterator pam_it = Location.begin();
		  pam_it != Location.end(); ++pam_it)
		  {
			std::cout <<pam_it->first<<  " : " << pam_it->second<< "|"<<std::endl;
		  }
	}
	catch(std::string err)
	{
		std:: cout << err<<std::endl;
		exit(EXIT_FAILURE);
	}
}