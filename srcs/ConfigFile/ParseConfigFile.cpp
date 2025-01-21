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
#include <cstdlib>
#include <ostream>
#include <string>

std::string trim(const std::string& str) {
	size_t start = str.find_first_not_of(" \t");
	size_t end = str.find_last_not_of(" \t");
	return (start == std::string::npos) ? "" : str.substr(start, end - start + 1);
}

bool	is_validAttServer(std::string &key, std::string &value, int inde)
{
	value.erase(value.end()-1);
	std::vector<std::string> validATT;
	validATT.push_back("host"),validATT.push_back("port"),validATT.push_back("server_name");
	validATT.push_back("client_max_body_size"), validATT.push_back("error_page");
	validATT.push_back("index"),validATT.push_back("root");
	for (size_t i = 0; i < validATT.size(); i++){
	if (validATT[i] == key)
	{
		if (key == "error_page")
		{
			key += "_";
			key += std::to_string(inde);
		}
		return true;
	}
	}
	return false;
}
bool	is_validAttLocation(std::string key, std::string value)
{
	(void)value;
	std::vector<std::string> validATT;
	validATT.push_back("allowed_methods"),validATT.push_back("upload_store");
	validATT.push_back("root"),validATT.push_back("index"), validATT.push_back("return");
	validATT.push_back("auto_index"), validATT.push_back("cgi");
	for (size_t i = 0; i <validATT.size(); i++){
	if (validATT[i] == key)
	{
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
	value.back() = '\0';
		return true;
}

void saveAttribute(const std::string& confline, Conserver& server, int index_line) {
	std::string trimmed_line = trim(confline);
	static bool sin = false;
	static std::string	host;
	if (trimmed_line.empty() || trimmed_line == "}")
		return;
	std::string key, value;
	parseKeyValue(trimmed_line, index_line, key, value);
	if (key == "host"){
		if (host.empty())
			host = value;
		else{
		server.addlistening(std::pair<std::string, std::string>(host, "8080"));
		host = value;
		}
		sin = true;
		return;
	}
	else if (key == "port"){
		if (sin)
			server.addlistening(std::pair<std::string, std::string>(host, value));
		else
			server.addlistening(std::pair<std::string, std::string>("0.0.0.0", value));
		sin = false;
		host = "";
		return ;
	}
	if (is_validAttServer(key, value, index_line))
		server.addAttribute(key, value);
	else
		throw("Unowned element in line :" + std::to_string(index_line) + "!");
}

void	parseLocation(const std::string& confline, Conserver& server, std::ifstream& infile, int& index_line) {
	std::map<std::string, std::string> location_map;
	std::string line_content;
	std::stack<char> LocationStack;

	std::string Key, Value;
	parseKeyValue(confline, index_line, Key, Value);
	location_map["PATH"] = trim(Value);
	LocationStack.push('{');
	while (std::getline(infile, line_content) && (line_content = trim(line_content) )!= "}") {
	index_line++;
	if (line_content.empty() || line_content[0] == '#')
		continue;
	if (line_content == "{") {
	std::cerr << "In line : "<< index_line << "\n";
	throw((std::string)("Error: Invalid structure !"));
	}
	parseKeyValue(line_content, index_line, Key, Value);
	if (is_validAttLocation(Key, Value))
		location_map[Key] = Value;
	else
		throw((std::string)("Error: Invalid structure(line " + std::to_string(index_line) + " )!"));
	}
	if (line_content == "}"){
	LocationStack.pop();
	}
	if (LocationStack.size())
		throw (std::string("Error: Invalid structure (line ")+ std::to_string(index_line)+")!");
	if (location_map.empty() || location_map.size() == 1) {
		throw( std::string("Error: Invalid location block at line ") + std::to_string(index_line));
	}
	if (!location_map["allowed_methods"].find("POST") && location_map["upload_store"].empty())
	{
		std::cerr<<"hhhhhhhh";
		exit((EXIT_FAILURE));
	}
	index_line++;
	server.addLocation(location_map);
}

void	processServerBlock(std::ifstream& infile, Conserver& server, int& index_line, std::stack<char>& ServStack) {
	std::string confline;

	while (std::getline(infile, confline) && (confline = trim(confline)) != "}") {
	index_line++;
	if (confline.empty() || confline[0] == '#' || confline == "{"){
		if (confline == "{")
			ServStack.push('{');
		continue;
	}
	if (confline.find("location") == 0 && confline[confline.length() -1 ] == '{') {
	parseLocation(confline, server, infile, index_line);
	} else {
	saveAttribute(confline, server, index_line);
	}
	}
	if (confline == "}")
		ServStack.pop();
	index_line++;
}

std::vector<Conserver>	parseConfigFile(char *in_file){
	std::ifstream infile(in_file);
	std::vector<Conserver> servers;
	std::stack<char> ServStack;
	int index_line = 0;
	std::string confline;

	if (!infile.is_open())
		throw((std::string)"Error: Failed to open configuration file '" );
	try{
		while (std::getline(infile, confline)) {
		Conserver server;
		index_line++;
		confline = trim(confline);
		if (confline.empty() || confline[0] == '#') continue;
		if (Check_Line(confline, ServStack)){
			processServerBlock(infile, server, index_line, ServStack);
		if(ServStack.size()){
			throw (std::string("Error: Unbalanced brackets '}', line") + std::to_string(index_line));
		}
		if (!server.getAttributes("root").empty())
			servers.push_back(server);
		else
			throw((std::string )"Error: server without root line");
		if (trim(confline) == "}")
			throw (std::string("Error: Unbalanced '}'" ));
		}
	}}
	catch(std::string err){
		std::cerr<<err<<std::endl;
		exit(1);
	}
		return servers;
}

