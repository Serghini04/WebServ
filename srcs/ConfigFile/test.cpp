
void openConfigFile(char *in_file, std::ifstream &infile)
{
	infile.open(in_file ? in_file : "ConfigFiles/Configfile.conf");
	if (!infile.is_open())
		throw std::string("Error: Failed to open configuration file!");
}

void validateServer(const Conserver &server)
{
	if (server.getAttributes("root").empty())
		throw std::string("Error: Server without root");
}

std::vector<Conserver> filterServersWithListening(const std::vector<Conserver> &servers)
{
	std::vector<Conserver> filtered_servers;
	for (size_t i = 0; i < servers.size(); i++)
		if (!servers[i].getlistening().empty())
			filtered_servers.push_back(servers[i]);
	return filtered_servers;
}
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
			if ((confline = trim(confline)).empty())
				continue;
			Check_Intree(confline, ServStack, index_line);
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
	return filterServersWithListening(servers);
}

