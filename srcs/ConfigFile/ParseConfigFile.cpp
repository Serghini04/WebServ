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

std::string trimcomment(const std::string& str)
{
	size_t start = str.find_first_not_of(" \t");
	size_t end = str.find_first_of("#");
	return (start == std::string::npos || start >= end) ? "" : str.substr(start, end - 1);
}

std::string trim(const std::string& str)
{
	std::string newstr =  trimcomment(str);
	size_t end = newstr.find_last_not_of(" \t");
	return newstr.substr(0, end+1);
}

bool is_valid_method(const std::string &method)
{
	return method == "GET" || method == "POST" || method == "DELETE";
}

void resetListeningState(bool& sin, std::string& host, std::map<std::string, std::string>& listenings)
{
	sin = false;
	host.clear();
	listenings["Default"].clear();
}

void	is_validvalue(std::string &key, std::string &value, int index_line)
{
	std::istringstream	ss(value);
	std::string			token;

	if (value.empty())
		throw ((std::string)"Invalid structer in the line "+Utility::ToStr(index_line));
	if (key == "client_max_body_size" && value.find_first_not_of("0123456789") != std::string::npos)
		throw ((std::string)"Invalid structer in the line "+Utility::ToStr(index_line));
	if (key == "cgi" && value != "on" && value != "off" )
		throw ((std::string)"Invalid structer in the value of  line "+Utility::ToStr(index_line));
	if (key == "allowed_methods")
	{
		while (ss >> token)
			if (!is_valid_method(token))
				throw std::string("Invalid method at line " + Utility::ToStr(index_line));
	}
	if (key == "return")
	{
		std::string	t[3];
		int			i = -1;
		int			status;

		while (++i < 3 && ss >> t[i] ) 
			;
		status = Utility::stringToInt(t[0]);
		if (t[0].find_first_not_of("0123456789") != std::string::npos || t[1].empty() || !t[2].empty())
			throw ((std::string)("Error: Unexpected Syntaxe, line ")+ Utility::ToStr(index_line));
		if (!(status >= 300 && status <= 399))
			throw ((std::string)("Error: Ivalide status code in line ")+ Utility::ToStr(index_line));
	}
}

void	is_validAttServer(std::string &key,std::string &value, int index)
{
	value.erase(value.end());
	std::vector<std::string> validATT;
	validATT.push_back("server_name");
	validATT.push_back("error_page");
	validATT.push_back("index");
	validATT.push_back("root");
	if (key == "return"){
		is_validvalue(key, value, index);
		return ;
	}
	else{
	for (size_t i = 0; i < validATT.size(); i++)
	{
		if (validATT[i] == key)
		{
			if (key != "error_page" && value.find(' ') != std::string::npos)
				throw ((std::string)("Error: Unexpected Syntaxe, line ")+ Utility::ToStr(index));
			if (key == "error_page")
			{
				key += "_";
				key += Utility::ToStr(index);
			}
			return;
		}
	}
	}
	throw ((std::string)"Error: Unexpected Syntaxe, line "+ Utility::ToStr(index));
}

bool validlocation_key(std::string key)
{
	return (key == "allowed_methods" || key == "upload_store" ||
			key == "root" || key == "index" || key == "return"||
			key == "auto_index" || key == "cgi");
}

void	is_validAttLocation(std::string &key, std::string &value, int index)
{
	if (!validlocation_key(key))
		throw ((std::string)"Error: Unexpected Syntaxe, line "+ Utility::ToStr(index));
	is_validvalue(key,value, index);
}

void	Check_Intree(std::string Name, std::stack<char>& ServStack, int& index)
{
	std::string	str2;
	std::string	str1;
	std::istringstream	ss(Utility::toLowerCase(Name));

	ss >> str1;
	ss >> str2;
	str1 = trim(str1);
	str2 = trim(str2);
	if (!(str1 == "server" && (str2 == "{" || str2.empty())) && str1 != "server{")
		throw (std::string("Error: Missing Server{} block, line ") + Utility::ToStr(index));
	if (str1 == "server{" || str2 == "{")
		ServStack.push('{');
}

void	parseKeyValue(const std::string& line_content, int &index_line, std::string& key, std::string& value)
{
	std::istringstream line_stream(line_content);

	if (!(line_stream >> key))
		throw (std::string("Error: Malformed key in line "));
	std::getline(line_stream, value);
	value = trim(value);
	if (!key.find("location"))
		if (value[value.length() - 1] == '{'){
			value = trim(value.substr(0,value.find_last_of("{")));
			return ;
		}
	if (value.empty() || value.back() != ';')
		throw (std::string("Error: Unexpected Syntaxe, line ") + Utility::ToStr(index_line));
	value = trim(value.erase(value.length() - 1));
	if (value.find(';') != std::string::npos)
		throw (std::string("Error: Unexpected Syntaxe, line ") + Utility::ToStr(index_line));
		return ;
}

void	IsValidHostValue(std::string value, int index)
{
	std::istringstream ss(value);
	std::vector<std::string> tokens;
	std::string token;
	int tokenint;
	int i = -1;

	while (std::getline(ss, token, '.'))
	{
		tokenint = Utility::stringToInt(token);
		if (tokenint > 255 || tokenint < 0 || ++i > 3)
			throw (std::string("Error: Invalid host, line ") + Utility::ToStr(index));
		tokens.push_back(token);
	}
}

void handleHost(std::string& value, Conserver& server, int index_line, std::map<std::string, std::string>& listenings, bool& sin, std::string& host)
{
	if (value.find_first_not_of("0123456789.") != std::string::npos)
		throw (std::string("Error: Unexpected Syntaxe of host in the line ") + Utility::ToStr(index_line));
	IsValidHostValue(value, index_line);
	if (!host.empty() && !listenings[host + "8080"].empty())
		std::cerr << "[Warning]: Duplicate listening << " << host << ":8080 >> ignored!" << std::endl;
	else if (!host.empty())
	{
		server.addlistening(std::make_pair(host, "8080"));
		listenings[host + "8080"] = Utility::ToStr(index_line);
	}
	host = value;
	sin = true;
	listenings["Default"] = host;
}

void handlePort(std::string& value, Conserver& server, int index_line, std::map<std::string, std::string>& listenings, bool& sin, std::string& host)
{
	std::string current_host = sin ? host : "0.0.0.0";
	std::string key = current_host + value;

	if (value.find_first_not_of("0123456789") != std::string::npos)
		throw (std::string("Error: Unexpected Syntaxe of port in the line ") + Utility::ToStr(index_line));
	if (!listenings[key].empty())
	{
		std::cerr << "[Warning]: Duplicate listening << " << current_host << ":" << value << " >> ignored!" << std::endl;
	}
	else
	{
		server.addlistening(std::make_pair(current_host, value));
		listenings[key] = Utility::ToStr(index_line);
	}
	resetListeningState(sin, host, listenings);
}

void saveAttribute(const std::string& line, Conserver& server, int index, std::map<std::string, std::string>& listenings)
{
	static bool sin = false;
	static std::string host;
	std::string key, value;

	if (line.empty() || line == "}")
		return;
	parseKeyValue(line, index, key, value);
	if (value.empty())
		throw ((std::string)"Error: Unexpected Syntaxe, line "+ Utility::ToStr(index));
	if (key == "host") 
		handleHost(value, server, index, listenings, sin, host);
	else if (key == "port")
		handlePort(value, server, index, listenings, sin, host);
	else if (key == "client_max_body_size")
		server.addBodySize(value);
	else
	{
		is_validAttServer(key, value, index);
		server.addAttribute(key, value);
	}
}

std::string	checklocationPath(std::string value)
{
	std::string NewValue;
	bool sig = true;

	if (!value.empty() && value[0] != '/')
		NewValue.push_back('/');
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

bool	isValidLocation(std::string line)
{
	std::stringstream strs(line);
	std::string word[4];
	int i = -1;

	while (strs >> word[++i] && i < 3)
		;
	if (word[0] == "location"&& word[3].empty())
		if ((!word[1].empty() && word[2] == "{") || word[1][word[1].length() - 1] == '{')
			return true;
	return false;
}

std::string	processLocationAttributes(std::ifstream& infile, int& index, std::map<std::string, std::string>& location_map, std::stack<char>& LocationStack)
{
	std::string line, Key, Value;

	while (std::getline(infile, line) && trim(line)[0] != '}')
	{
		index++;
		line = trim(line);
		if (line.empty())
			continue;
		parseKeyValue(line, index, Key, Value);
		is_validAttLocation(Key, Value, index);
			location_map[Key] = Value;
	}
	line = trim(line);
	if (line.find('}') != std::string::npos) 
		LocationStack.pop();
	if (!LocationStack.empty())
		throw std::string("8Error: Unexpected syntax (line ") + Utility::ToStr(index) + ")!";
	return (line[0] == '}' && line.size() > 1)? trim(line.substr(1)) : "";
}

void validateLocationBlock(std::map<std::string, std::string>& location_map, int index)
{
	if (location_map.size() <= 1)
		throw std::string("Error: Invalid location block at line ") + Utility::ToStr(index);

	if (location_map["allowed_methods"].find("POST") != std::string::npos &&
		(location_map["upload_store"].empty() || location_map["upload_store"][0] == '\0'))
		throw std::string("'"+location_map["PATH"] +"': The location requires an 'upload_store' attribute!");
}

void	handleLineAbackup(Conserver& server, std::ifstream& infile, std::string backupline, int& index, std::stack<char>& ServStack, std::map<std::string, std::string>& lis)
{
	if (backupline.empty())
		return;
	if (backupline == "}")
		ServStack.pop();
	if (ServStack.empty())
			return ;
	if (isValidLocation(backupline))
		parseLocation(backupline, server, infile, index, ServStack, lis);
	else
		saveAttribute(backupline, server, index, lis);
}

void parseLocation(const std::string& confline, Conserver& server, std::ifstream& infile, int& index, std::stack<char>& ServStack, std::map<std::string, std::string>& lis)
{
	std::map<std::string, std::string> location_map;
	std::string Key, Value;
	std::string backupline;
	std::stack<char> LocationStack;

	parseKeyValue(confline, index, Key, Value);
	Value = trim(Value);
	if (Value.empty())
		throw std::string("Empty location path in line ") + Utility::ToStr(index) + "!";
	location_map["PATH"] = checklocationPath(Value);
	server.addPath(location_map["PATH"]);
	LocationStack.push('{');
	backupline = processLocationAttributes(infile, index, location_map, LocationStack);
	validateLocationBlock(location_map, index);
	server.addLocation(location_map);
	index++;
	handleLineAbackup(server, infile, backupline, index, ServStack, lis);
}

void validateServerOpening(std::string& line, bool& signe, std::stack<char>& SStack, int index)
{
	line = trim(line);
	if (!signe && SStack.empty() && line != "{" && line[0] != '{')
		throw std::string("'server' has not opened with '{' at line: ") + Utility::ToStr(index - 1);
	if(!signe &&( line == "{" || line[0] == '{'))
	{
		line = line.substr(1);
		SStack.push('{');
		signe = true;
	}

}

void processServerAttributes(const std::string& line, Conserver& server, std::ifstream& inf, int& index, std::stack<char>& SStack, std::map<std::string, std::string>& lis_gs)
{
	if (isValidLocation(line))
		parseLocation(line, server, inf, index, SStack, lis_gs);
	else
		saveAttribute(line, server, index, lis_gs);
}

void finalizeServerBlock(Conserver& server, std::map<std::string, std::string>& lis_gs, int index)
{
	if (server.getAttributes("client_max_body_size").empty())
		server.addBodySize("");

	if (!lis_gs["Default"].empty())
	{
		server.addlistening(std::make_pair(lis_gs["Default"], "8080"));
		lis_gs[lis_gs["Default"] + "8080"] = Utility::ToStr(index);
	}
	if (server.getLocation("/").empty())
		throw std::string("Server without root location!");
}

void processServerBlock(std::ifstream& inf, Conserver& server, int& index, std::stack<char>& SStack, std::map<std::string, std::string>& lis_gs)
{
	std::string line;
	bool signe = false;

	while (std::getline(inf, line) && ++index)
	{
		if (line.empty())
			continue;
		validateServerOpening(line, signe, SStack, index);
		if (line == "}" && SStack.size() > 0)
			SStack.pop();
		processServerAttributes(line, server, inf, index, SStack, lis_gs);
		if (!SStack.size())
			break;
	}
	finalizeServerBlock(server, lis_gs, index);
}

void openConfigFile(char *in_file, std::ifstream &infile)
{
	infile.open(in_file ? in_file : "ConfigFiles/Configfile.conf");
	if (!infile.is_open())
		throw std::string("Error: Failed to open configuration file!");
}

void validateServer(Conserver &server)
{
	if (server.getAttributes("root").empty())
		throw std::string("Error: Server without root");
}

// std::vector<Conserver> filterServersWithListening(std::vector<Conserver> &servers)
// {
// 	std::vector<Conserver> filtered_servers;
// 	for (size_t i = 0; i < servers.size(); i++)
// 		if (!servers[i].getlistening().empty())
// 			filtered_servers.push_back(servers[i]);
// 	return filtered_servers;
// }
std::vector<Conserver> parseConfigFile(char *in_file)
{
	std::ifstream infile;
	std::vector<Conserver> servers;
	std::stack<char> ServStack;
	std::map<std::string, std::string> listenings;
	int index_line = 0;

	try 
	{
		openConfigFile(in_file, infile);
		std::string line;
		while (std::getline(infile, line) && ++index_line)
		{
			Conserver server;
			if ((line = trim(line)).empty())
				continue;
			Check_Intree(line, ServStack, index_line);
			processServerBlock(infile, server, index_line, ServStack, listenings);
			if (!ServStack.empty())
				throw std::string("Error: Unbalanced brackets '{}', line ") + Utility::ToStr(index_line);
			validateServer(server);
			servers.push_back(server);
		}
	}
	catch (const std::string& err)
	{
		std::cerr << err << std::endl;
		exit(EXIT_FAILURE);
	}
	// filterServersWithListening(servers);
	return servers;
}


