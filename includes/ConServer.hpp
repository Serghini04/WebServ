/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hidriouc <hidriouc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/20 16:02:15 by hidriouc          #+#    #+#             */
/*   Updated: 2025/01/25 11:49:56 by hidriouc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <map>
#include <list>
#include <iostream>
#include <cstddef>
#include <cstdlib>
#include <fstream>
#include <iterator>
#include <string>
#include <utility>
#include <vector>
#include <sstream>
#include <ostream>
class Conserver {
	private:
		std::map<std::string, std::string> serv_attributes;
		std::list<std::map<std::string, std::string> > location_list;
		std::vector<std::pair<std::string, std::string> > listening;
		std::vector <std::string> AllPaths;
		long long client_max_body_size;
		
	public:
		void addAttribute(const std::string& key, const std::string& value);
		void addPath(std::string path);
		std::vector<std::string> getphats();
		void addLocation(const std::map<std::string, std::string> loc_attribute);
		void addlistening(std::pair<std::string, std::string> listen);
		std::vector<std::pair<std::string, std::string> >  getlistening();
		std::list<std::map<std::string, std::string> >	getlist();
		std::string getAttributes(std::string att);
		std::string getErrorPage(int ERRNumber);
		void	addBodySize(std::string value);
		long long	GetBodySize();
		std::map<std::string, std::string> getLocation(std::string locaPath);
};

std::vector<Conserver> parseConfigFile(char* );