/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hidriouc <hidriouc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/20 16:54:05 by hidriouc          #+#    #+#             */
/*   Updated: 2025/01/22 11:30:25 by hidriouc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include <Server.hpp>
#include <cstddef>
#include <string>
#include <sys/socket.h>
#include <sys/syslimits.h>

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
	listening.push_back(lsn);
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
		if (!It_map->first.find("error_page") && !It_map->second.find(std::to_string(ERRNumber)))
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

