/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meserghi <meserghi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/20 16:54:05 by hidriouc          #+#    #+#             */
/*   Updated: 2025/01/25 18:42:19 by meserghi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include <Server.hpp>
#include <chrono>
#include <cstddef>
#include <cstdlib>
#include <string>
#include <sys/socket.h>
#include <sys/syslimits.h>
#include <limits>

void	Conserver::addAttribute(const std::string& key, const std::string& value)
{
	serv_attributes[key] = value;
}
void	Conserver::addLocation(const std::map<std::string, std::string> loc_attribute)
{
	location_list.push_back(loc_attribute);
}
void Conserver::addPath(std::string path)
{
	AllPaths.push_back(path);
}
void Conserver::addlistening(std::pair<std::string, std::string > lsn)
{
	std::vector<std::pair<std::string, std::string> >::iterator it = listening.begin();
	while (it != listening.end())
	{
		if (it->second == lsn.second)
		{
			std::cerr << "duplicate port << "<<lsn.second<< " >>!"<<std::endl;
			exit(EXIT_FAILURE);
		}
		it++;
	}
	listening.push_back(lsn);
}
long long	Conserver::getBodySize()
{
	return client_max_body_size;
}
void	Conserver::addBodySize(std::string value)
{
	client_max_body_size = 0;
	for(size_t i = 0; i < value.size(); i++)
	{
		client_max_body_size = client_max_body_size * 10 + (value[i] - 48);
		if (client_max_body_size  < 0 || !(value[i] >= '0' && value[i] <= '9')){
			std::cerr<< "Invalid value of client_max_body_size !"<<std::endl;
			exit(EXIT_FAILURE);
		}
	}
	if (value.empty())
		client_max_body_size = -1;
}
std::vector<std::string> Conserver::getphats()
{
	return AllPaths;
}
std::string	Conserver::getAttributes(std::string att) 
{
	std::map<std::string, std::string>::iterator It_map;
	for (It_map = serv_attributes.begin(); It_map != serv_attributes.end(); ++It_map)
	{
		if (It_map->first == att)
			return It_map->second;
	}
	return "";
}
std::list<std::map<std::string, std::string> >	Conserver::getlist() 
{
	return  location_list;
}

std::string  Conserver::getErrorPage(int ERRNumber)
{
	std::map<std::string, std::string>::iterator It_map;
	for (It_map = serv_attributes.begin(); It_map != serv_attributes.end(); ++It_map)
	{
		if (!It_map->first.find("error_page") && !It_map->second.find(Utility::ToStr(ERRNumber)))
		{
			std::string str;
			size_t beg = It_map->second.find_first_of(' ');
			str = It_map->second.substr(beg, It_map->second.length() - beg + 1);
			beg = str.find_first_not_of(' ');
			return str.substr(beg, str.length() - beg + 1);
		}
	}
	return "";
	
}
std::vector<std::pair<std::string, std::string> >  Conserver::getlistening()
{
	return listening;
}

std::map<std::string, std::string> Conserver::getLocation(std::string valueToFind)
{
	// Iterate through the list
	for (std::list<std::map<std::string, std::string> >::iterator listIt = location_list.begin();
	 listIt != location_list.end(); ++listIt)
	{
		// Iterate through the map
		for (std::map<std::string, std::string>::iterator mapIt = listIt->begin();
		 mapIt != listIt->end(); ++mapIt) {
		// Check if the value matches
			
			if (mapIt->second == valueToFind) {
			return *listIt; // Return the entire map that contains the value
		}
		}
	}
	// Return an empty map if the value is not found
	return std::map<std::string, std::string>();
}
