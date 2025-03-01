/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hidriouc <hidriouc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/20 16:02:15 by hidriouc          #+#    #+#             */
/*   Updated: 2025/02/27 18:51:16 by hidriouc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#ifndef CONSERVER_HPP
#define CONSERVER_HPP

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
		std::vector<std::pair<std::string, std::string> > listening;
		std::list<std::map<std::string, std::string> > location_list;
		std::vector <std::string> AllPaths;
		long long client_max_body_size;
	public:
		void	addAttribute(const std::string& key, const std::string& value);
		void	addPath(std::string path);
		void	addBodySize(std::string value);
		void	addLocation(const std::map<std::string, std::string> loc_attribute);
		void	addlistening(std::pair<std::string, std::string> listen);
		~Conserver ();
		long long	getBodySize();
		std::pair<std::string, std::string>	getreturnof(std::string path);
		std::string	getAttributes(std::string att);
		std::string	getErrorPage(int ERRNumber);
		std::vector<std::string>	getphats();
		std::map<std::string, std::string>	getLocation(std::string locaPath);
		std::vector<std::pair<std::string, std::string> >	getlistening();
		std::list<std::map<std::string, std::string> >		getlist();
};

std::vector<Conserver> parseConfigFile(char* );
void parseLocation(const std::string& confline, Conserver& server, std::ifstream& infile, int& index, std::stack<char>& ServStack);

#endif