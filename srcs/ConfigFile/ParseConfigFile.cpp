/* ************************************************************************** */
/*	                                                                        */
/*                                                        :::      ::::::::   */
/*   parseConfigFile.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hidriouc <hidriouc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/20 11:45:20 by hidriouc          #+#    #+#             */
/*   Updated: 2024/12/22 15:48:45 by hidriouc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConServer.hpp"
#include <Utility.hpp>
#include <cctype>
#include <cstddef>
#include <cstdlib>
#include <ostream>
#include <string>

std::string trim(const std::string& str) {
	size_t start = str.find_first_not_of(" \t");
	size_t end = str.find_last_not_of(" \t");
	return (start == std::string::npos) ? "" : str.substr(start, end - start + 1);
}
bool is_validvalueServer(std::string &key, std::string &value, int index_line)
{
	std::string tb[3];

	if (value.empty())
		return false;
	if (key == "client_max_body_size")
	{
		for (size_t i = 0; i < value.size(); i++)
			if (!std::isdigit(value[i]))
				return false;
	}
	if (key == "allowed_methods")
	{
		std::istringstream	ss(value);
		for(int i = 0; i < 3; i++)
		{
			ss >> tb[i];
			if (!tb[i].empty() && tb[i] != "GET" && tb[i] != "POST" && tb[i] != "DELETE")
				std::cerr << "Invalid Methode in the line " << index_line<<"!"<<std::endl,exit(EXIT_FAILURE);
		}
	}
	if (key == "return")
	{
		std::istringstream	ss(value);
		for(int i = 0; i < 3; i++)
			ss >> tb[i];
		for(size_t i = 0; i < tb[0].size(); i++)
			if (!std::isdigit(value[i]))
				std::cerr << "Invalid structer in the line " << index_line<<"!"<<std::endl,exit(EXIT_FAILURE);
		if (tb[1].empty()|| !tb[2].empty())
				std::cerr << "Invalid Methode in the line " << index_line<<"!"<<std::endl,exit(EXIT_FAILURE);;
	}
	for (size_t i = 0; i < value.size(); i++);

	return true;
}
bool	is_validAttServer(std::string &key, std::string &value, int inde)
{
	value.erase(value.end());
	std::vector<std::string> validATT;
	validATT.push_back("host"),validATT.push_back("port"),validATT.push_back("server_name");
	validATT.push_back("client_max_body_size"), validATT.push_back("error_page");
	validATT.push_back("index"),validATT.push_back("root");
	if (!is_validvalueServer(key,value, inde)) return false;
	for (size_t i = 0; i < validATT.size(); i++){
	if (validATT[i] == key)
	{
		if (key == "error_page")
		{
			key += "_";
			key += Utility::ToStr(inde);
		}
		return true;
	}
	}
	return false;
}
bool	is_validAttLocation(std::string key, std::string value, int index)
{
	(void)value;
	std::vector<std::string> validATT;
	validATT.push_back("allowed_methods"),validATT.push_back("upload_store");
	validATT.push_back("root"),validATT.push_back("index"), validATT.push_back("return");
	validATT.push_back("auto_index"), validATT.push_back("cgi");
	for (size_t i = 0; i <validATT.size(); i++){
	if (validATT[i] == key)
	{
		if (value.empty())
			return false;
		is_validvalueServer(key,value, index);
			return true;
	}
	}
	return false;
}
bool	Check_Line(std::string Name, std::stack<char>& ServStack)
{
	if (Name == "server{" || Name == "server"){
		if (Name == "server{")
			ServStack.push('{');
		return true;
	}
	size_t space_pos = Name.find(' ');
	if (space_pos != std::string::npos) {
	std::string str1 = Name.substr(0, space_pos);
	size_t next_start = Name.find_first_not_of(' ', space_pos);
	std::string str2 = (next_start != std::string::npos) ? Name.substr(next_start) : "";
	 if (str1 == "server" && ( str2 == "{" || str2.empty())){
		if (str2 == "{")
			ServStack.push('{');
		return true;
	 }
	}
	return false;
}

bool	parseKeyValue(const std::string& line_content, int &index_line, std::string& key, std::string& value) {
	std::istringstream line_stream(line_content);
	if (!(line_stream >> key)) {
		throw (std::string("Error: Malformed key in line "));
	}
	std::getline(line_stream, value);
	value = trim(value);
	if (!key.find("location")){
	if (value[value.length() - 1] == '{'){
		value = value.substr(0,value.find_first_of("{"));
		return true;
	}}
	if (value.empty() || value.back() != ';') {
		std:: cerr << "In Line :"<< index_line<<std::endl;
		std::cerr<<"Error: Missing ';' Or invalid Strecture in the value for key : ";
		return false;
	}
	value.erase(value.length()-1);
	if (value.find(';') != std::string::npos)
	{
		std::cerr << "Invalid strecture in line "<<index_line<<std::endl;
		exit(EXIT_FAILURE);
	}
		return true;
}

void saveAttribute(const std::string& confline, Conserver& server, int index_line, std::map<std::string, std::string>& listenings) {
	std::string trimmed_line = trim(confline);
	static bool sin = false;
	static std::string host;
	std::string key, value;
	

	if (trimmed_line.empty() || trimmed_line == "}") {
	return;
	}
	parseKeyValue(trimmed_line, index_line, key, value);
	if (key == "host" && !value.empty()) {
		if (!host.empty() && !listenings[ host+"8080"].empty()) {
		std::cerr << "[Warning]:Duplicate listening<< " << host << ":8080 >> ignored!" << std::endl;
		} else if (!host.empty()) {
		server.addlistening(std::pair<std::string, std::string>(host, "8080"));
		listenings[host+"8080"] = Utility::ToStr(index_line);
		}
		host = value;
		sin = true;
		listenings["Default"] = host;
		return;
	}
	if (key == "port" && !value.empty()) {
		const std::string& current_host = sin ? host : "0.0.0.0";
			if (!listenings[current_host + value].empty()) {
			std::cerr << "[Warning]: Duplicate listening" 
				<< current_host << ":" << value << " >> ignored!" << std::endl;
			sin = false;
			host.clear();
			listenings["Default"].clear();
			return;
		}
		server.addlistening(std::make_pair(current_host, value));
		listenings[current_host+value] = Utility::ToStr(index_line);
		sin = false;
		listenings["Default"].clear();
		host.clear();
		return;
	}
	if (key == "client_max_body_size" && !value.empty()) {
		server.addBodySize(value);
	}
	if (is_validAttServer(key, value, index_line)) {
		server.addAttribute(key, value);
	} else {
		std::cerr << "Invalid structure in line: " << index_line << "!!" << std::endl;
		exit(EXIT_FAILURE);
	}
}

std::string	checklocationPath(std::string value)
{
	std::string NewValue;
	bool sig = true;

	for(size_t i = 0; i < value.size(); i++)
	{
		if ((value[i] == '/' && sig) || value[i] != '/'){

			NewValue.push_back(value[i]);
		if (value[i] != '/')
			sig = true;
		else
			 sig = false;
		}
	}
	return NewValue;
}

void	parseLocation(const std::string& confline, Conserver& server, std::ifstream& infile, int& index_line) {
	std::map<std::string, std::string> location_map;
	std::string line_content;
	std::stack<char> LocationStack;

	std::string Key, Value;
	parseKeyValue(confline, index_line, Key, Value);
	if (Value.empty())
		throw ((std::string)"Empty location Path in thr lin " + Utility::ToStr(index_line) + "!");
	location_map["PATH"] = checklocationPath(trim(Value));
	server.addPath(location_map["PATH"]);
	LocationStack.push('{');
	while (std::getline(infile, line_content) && (line_content = trim(line_content) )!= "}") {
	index_line++;
	if (line_content.empty() || line_content[0] == '#')
		continue;
	parseKeyValue(line_content, index_line, Key, Value);
	if (Key == "{") {
	std::cerr << "In line : "<< index_line << "\n";
	throw((std::string)("Error: Invalid structure !"));
	}
	if (Key == "cgi")
		;
	if (is_validAttLocation(Key, Value, index_line))
		location_map[Key] = Value;
	else
		throw((std::string)("Error: Invalid structure(line " + Utility::ToStr(index_line) + " )!"));
	}
	if (line_content == "}"){
	LocationStack.pop();
	}
	if (LocationStack.size())
		throw (std::string("Error: Invalid structure (line ")+ Utility::ToStr(index_line)+")!");
	if (location_map.empty() || location_map.size() == 1) {
		throw( std::string("Error: Invalid location block at line ") + Utility::ToStr(index_line));
	}
	if (location_map["allowed_methods"].find("POST")!= std::string::npos &&
	(location_map["upload_store"].empty() || !location_map["upload_store"][0]))
	std::cerr<<"'"<<location_map["PATH"]<<"'", throw ((std::string)": The location requires an 'upload_store' attribute!");
	index_line++;
	server.addLocation(location_map);
}

void	processServerBlock(std::ifstream& infile, Conserver& server, int& index_line, std::stack<char>& ServStack, std::map<std::string, std::string>& listenings) {
	std::string confline;

	while (std::getline(infile, confline) && (confline = trim(confline)) != "}") {
	index_line++;
	if (confline.empty() || confline[0] == '#' || confline == "{"){
		if (confline == "{")
			ServStack.push('{');
		continue;
	}
	if (confline.find("location")!= std::string::npos && confline[confline.length() -1 ] == '{') {
		parseLocation(confline, server, infile, index_line);
	} else {
	saveAttribute(confline, server, index_line, listenings);
	}
	}
	if (confline == "}")
		ServStack.pop();
	index_line++;
	if (server.getAttributes("client_max_body_size").empty())
		server.addBodySize("");
	if (!listenings["Default"].empty()){
		server.addlistening(std::pair<std::string, std::string>(listenings["Default"], "8080"));
		listenings[listenings["Default"]+"8080"] = Utility::ToStr(index_line);
	}
}

std::vector<Conserver>	parseConfigFile(char *in_file){
	std::ifstream			infile;
	std::vector<Conserver>	servers;
	std::stack<char>		ServStack;
	int 					index_line = 0;
	std::string 			confline;
	std::map<std::string, std::string> listenings;
	std::vector<Conserver> Rservers;
	try{
		if (!in_file)
		infile.open("ConfigFiles/Configfile.conf");
		else
		infile.open(in_file);
		if (!infile.is_open())
		throw((std::string)"Error: Failed to open configuration file !" );
		while (std::getline(infile, confline)) {
		Conserver server;
		index_line++;
		confline = trim(confline);
		if (confline.empty() || confline[0] == '#') continue;
		if (Check_Line(confline, ServStack)){
			processServerBlock(infile, server, index_line, ServStack, listenings);
		if(ServStack.size()){
			throw (std::string("Error: Unbalanced brackets '}', line") + Utility::ToStr(index_line));
		}
		if (!server.getAttributes("root").empty()){
			servers.push_back(server);
		}
		else
			throw((std::string )"Error: server without root line !"+ Utility::ToStr(index_line - 1));
		if (confline == "}")
			throw (std::string("Error: Unbalanced '}'" ));
		}
		else{
			throw (std::string("Error: Unbalanced brackets '}', line") + Utility::ToStr(index_line));
		}
		}
		// std::cerr << ">>>>>"<<confline <<std::endl;
	}
	catch(std::string err){
		std::cerr<<err<<std::endl;
		exit(1);
	}
	for (size_t i = 0; i < servers.size(); i++){
	std::vector<std::pair<std::string, std::string> > CurVec = servers[i].getlistening();
	if (!CurVec.empty())
		Rservers.push_back(servers[i]);
	}
	return servers;
}

