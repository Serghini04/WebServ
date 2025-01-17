/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hidriouc <hidriouc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/20 16:02:15 by hidriouc          #+#    #+#             */
/*   Updated: 2025/01/17 12:27:06 by hidriouc         ###   ########.fr       */
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
#include <vector>
#include <sstream>
#include <ostream>
class Conserver {
	private:
	    std::map<std::string, std::string> serv_attributes;
	    std::list<std::map<std::string, std::string> > location_list;
		std::vector<std::string> hosts;
		std::vector<std::string> ports;
	public:
		void addAttribute(const std::string& key, const std::string& value);
		void addLocation(const std::map<std::string, std::string> loc_attribute);
		void addhost(std::string host);
		void addport(std::string port);
		// std::string gethost(int n);
		// std::string getport(int n);
		std::string getAttributes(std::string att);
		std::string getErrorPage(int ERRNumber);
		std::map<std::string, std::string> getLocation(std::string locaPath);
};

std::vector<Conserver> parseConfigFile(char* );