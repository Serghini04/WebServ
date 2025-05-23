/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meserghi <meserghi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/29 18:35:29 by meserghi          #+#    #+#             */
/*   Updated: 2025/03/04 21:33:24 by meserghi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include <RequestParse.hpp>
# include <Server.hpp>


RequestParse::RequestParse(int clientSocket, Server &server): server(server)
{
	_clientSocket = clientSocket;
	_fileDebug.open(_body.BodyFileName());
	if (!_fileDebug.is_open())
		throw std::string("500 Internal Server Error");
	_isHeader = true;
	_body.setIsCGI(false);
	_requestIsDone = false;
	_statusCode = eOK;
	_statusCodeMessage = "200 OK";
}

bool	isDuplicate(char a, char b)
{
	return a == '/' && b == '/';
}

bool	RequestParse::isCGI()
{
	return _body.isCGI();
}

void	RequestParse::setIsCGI(bool s)
{
	_body.setIsCGI(s);
}

void	RequestParse::checkURL()
{
	const std::string invalidChars = "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F"
									"\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1A\x1B\x1C\x1D\x1E\x1F"
									"\x7F\"<>\\^`{|}";

	if (_url.length() > 4096)
		throw std::string("414 URI Too Long");
	if (_url[0] != '/')
		throw std::string("400 Bad Request");
	if (_url.find_first_of(invalidChars) != std::string::npos)
		throw std::string("400 Bad Request");
	_url.erase(std::unique(_url.begin(), _url.end(), isDuplicate), _url.end());
	for (size_t i = 0; i < _url.length(); i++)
	{
		if (_url[i] == '%' && i + 2 < _url.length())
		{
			if (std::isxdigit(_url[i + 1]) && std::isxdigit(_url[i + 2]))
				_url.replace(i, 3, Utility::percentEncoding(_url.substr(i, 3)));
			else
				throw std::string("400 Bad Request");
			i++;
		}
		else if (_url[i] == '%')
			throw std::string("400 Bad Request");
	}
	size_t queryPos = _url.find('?');
	if (queryPos != std::string::npos)
	{
		_queryString = _url.substr(queryPos + 1);
		_url.erase(queryPos);
	}
	size_t fragmentPos = _url.find('#');
	if (fragmentPos != std::string::npos)
	{
		_fragment = _url.substr(fragmentPos + 1);
		_url.erase(fragmentPos);
	}
	if (_url.find("/../") != std::string::npos || _url == "/.." || _url.rfind("/..") == _url.length() - 3)
		throw std::string("400 Bad Request");
}

std::string	RequestParse::statusCodeMessage()
{
	return _statusCodeMessage;
}

void	RequestParse::parseFirstLine(std::string  header)
{
	std::stringstream    ss(header);

	if (!std::isalpha(header[0]))
		throw std::string("400 Bad Request");
	ss >> _method >> _url >> _httpVersion;
	if (_method != "GET" && _method != "POST" && _method != "DELETE")
		throw std::string("400 Bad Request");
	if (_url.empty() || _httpVersion.empty())
		throw std::string("400 Bad Request"); 
	if (_httpVersion != "HTTP/1.1")
		throw std::string("400 Bad Request");
	checkURL();
	if (_method == "GET")
		_enumMethod = eGET;
	else if (_method == "POST")
		_enumMethod = ePOST;
	else
		_enumMethod = eDELETE;
	std::cout << "URl =>>" << _url << "<<" << std::endl;
}

std::map<std::string, std::string>	&RequestParse::getMetaData()
{
	return (_metaData);
}

void	RequestParse::SetStatusCodeMsg(std::string message)
{
	_statusCodeMessage = message;
}

bool RequestParse::requestIsDone()
{
	return this->_requestIsDone;
}

status	RequestParse::statusCode()
{
	return this->_statusCode;
}

void	RequestParse::SetStatusCode(status s)
{
	_statusCode = s;
}

void	RequestParse::SetRequestIsDone(bool s)
{
	_requestIsDone = s;
}

void	RequestParse::setIsHeader(bool isHeader)
{
	_isHeader = isHeader;
}

bool	RequestParse::isHeader()
{
	return _isHeader;
}
bool		RequestParse::isConnectionClosed()
{
	return (getMetaData()["Connection"].find("close"));
}
void	RequestParse::parseMetaData(std::string header)
{
	
	for (int i = 0; header[i]; i++)
	{
		if (!std::isspace(header[i]) && header[i] != ':')
			continue;
		else if (i != 0 && header[i] == ':')
		{
			_metaData[Utility::toLowerCase(header.substr(0, i))] = Utility::trimSpace(header.substr(i + 1));
			return ;
		}
		else
			break ;
	}
	throw std::string("400 Bad Request");
}

void RequestParse::getConfigFile()
{
    for (size_t i = 0; i < server.serversConfigs[server.serversClients[_clientSocket]].size(); i++)
    {
        std::string address = _metaData.find("host")->second;
        size_t pos = address.find(':');
        std::string hostname;
        if (pos != std::string::npos)
            hostname = address.substr(0, pos);
        if (server.serversConfigs[server.serversClients[_clientSocket]][i]->getAttributes("server_name") == hostname)
		{
            _configServer = *server.serversConfigs[server.serversClients[_clientSocket]][i];
			return ;
		}
    }
    _configServer = *server.serversConfigs[server.serversClients[_clientSocket]][0];
}
bool	RequestParse::parseHeader(std::string &header)
{
	int firstLine = 1;
	size_t start = 0;

	if (header.empty())
		throw std::string("400 Bad Request");
	for (size_t i = 1; i < header.size(); i++)
	{
		if (header[i - 1] == '\r' && header[i] == '\n')
		{
			if (firstLine)
			{
				parseFirstLine(header.substr(0, i - 1));
				firstLine = 0;
			}
			else
			    parseMetaData(header.substr(start, i - start - 1));
			if (i + 2 < header.size() && header[i + 1] == '\r' && header[i + 2] == '\n')
				break ;
			start = i + 1; 
		}
	}
	_body.setMetaData(_metaData);
	getConfigFile();
	_body.setMaxBodySize(_configServer.getBodySize());
	return false;
}

methods	RequestParse::method()
{
	return _enumMethod;
}

std::string	RequestParse::URL()
{
	return (_url);
}

std::string	RequestParse::matchingURL()
{
	std::vector<std::string> locations = _configServer.getphats();

	std::sort(locations.begin(), locations.end(), Utility::compareByLength);
	for (size_t	i = 0; i < locations.size(); i++)
	{
		if (locations[i] == _url)
			return locations[i];
		else if (locations[i] < _url && locations[i] == _url.substr(0, locations[i].length()))
			return locations[i];
	}
	return "/";
}

void	RequestParse::checkAllowedMethod()
{
	std::string	location = matchingURL();

	_location = location;
	if (_configServer.getAttributes("return") != "" || _configServer.getLocation(_location)["return"] != "")
		throw std::string("200 OK");

	if (_configServer.getLocation(_location)["allowed_methods"].find(Utility::toUpperCase(_method)) == std::string::npos)
	{
		_body.setIsCGI(false);
		throw std::string("405 Method Not Allowed");
	}
	if (_enumMethod == ePOST)
		_body.setFileName(_configServer.getLocation(location)["upload_store"] + "/Output");
}

void	RequestParse::deleteURI()
{
	if (!Utility::isReadableFile(_uri))
		throw std::string("403 Forbidden");
	if (!Utility::isDirectory(_uri))
		std::remove(_uri.c_str());
	else
	{
		DIR* currentDir = opendir(_uri.c_str());
		dirent* dp;
		std::string targetFile;
		if (!currentDir)
		    throw std::string("500 Internal Server Error");
		while ((dp = readdir(currentDir)))
		{
			if (dp->d_name == std::string(".") || dp->d_name == std::string(".."))
				continue;
			targetFile = _uri + "/" + dp->d_name;
			if (!Utility::isDirectory(targetFile))
				std::remove(targetFile.c_str());
			else
				Utility::deleteFolderContent(targetFile);
		}
		closedir(currentDir);
	}
}

void RequestParse::deleteMethod()
{
	_body.setIsCGI(false);
	if (_configServer.getLocation(_location)["root"] != "")
		_uri = _configServer.getLocation(_location)["root"];
	else
		_uri = _configServer.getAttributes("root");
	_uri +=  "/" + _url;
	if (!Utility::checkIfPathExists(_uri))
		throw std::string("404 Not Found");
	if (Utility::isDirectory(_uri))
	{
		if (_configServer.getLocation(_location)["index"] != "")
			_uri += "/" + _configServer.getLocation(_location)["index"];
	}
	deleteURI();
	throw std::string("204 No Content");
}

bool RequestParse::parseHeader(std::string &header, std::string &buff)
{
	bool	isHeader = true;

	header.append(buff);
	size_t pos = header.find("\r\n\r\n");
	if (pos == std::string::npos)
		return isHeader;
	isHeader = parseHeader(header);
	buff = header.substr(pos + 4);
	_body.setbodyType(_body.getTypeOfBody(_enumMethod));
	checkAllowedMethod();
	if (_body.bodyType() == eBoundary || _body.bodyType() == eChunkedBoundary)
	{
		std::string	boundary = _metaData["content-type"].substr(_metaData["content-type"].find("boundary=") + 9);
		_body.setBoundary("--" + boundary + "\r\n");
		_body.setBoundaryEnd("--" + boundary + "--\r\n");
	}
	if (_enumMethod == ePOST && (_body.bodyType() == eChunked || _body.bodyType() == eContentLength))
		_body.openFileBasedOnContentType();
	header.clear();
	if (_enumMethod != eDELETE)
		checkCGI();
	if (_enumMethod == eGET || _enumMethod == eDELETE)
	{
		if (_enumMethod == eDELETE)
			deleteMethod();
		throw std::string("200 OK");
	}
	_body.setClearData(true);
	return isHeader;
}

void	RequestParse::setUrl(std::string s)
{
	this->_url = s;
}

std::string	RequestParse::location()
{
	return _location;
}

void	RequestParse::checkCGI()
{
	// check url if have cgi :
	size_t pos = _url.rfind('.');
	if (pos != std::string::npos)
	{
		std::string ext = _url.substr(pos);
		if (_configServer.getLocation(_location)["cgi"+ ext] != "")
		{
			_body.setIsCGI(true);
		}
	}
	// check index :
	if (Utility::isDirectory(_url) && _configServer.getLocation(_location)["index"] != "")
	{
		size_t pos = _configServer.getLocation(_location)["index"].rfind('.');
		if (pos != std::string::npos)
		{
			std::string ext =  _configServer.getLocation(_location)["index"].substr(pos);
			if (_configServer.getLocation(_location)["cgi"+ ext] != "")
			{
				_body.setIsCGI(true);
			}
		}
	}
}

void    RequestParse::readBuffer(std::string buff)
{
	try
	{
		if (_requestIsDone)
			return ;
		if (isHeader())
			setIsHeader(parseHeader(_header, buff));
		_requestIsDone = _body.parseBody(buff);
	}
	catch (std::string &e)
	{
		std::cout << ">>" << e << std::endl;
		_header.clear();
		_statusCode = (status)atoi(e.c_str());
		_statusCodeMessage = e;
		_requestIsDone = 1;
	}
	catch (...)
	{
		_header.clear();
		_statusCodeMessage = "500 Internal Server Error";
		_statusCode = eInternalServerError;
		_requestIsDone = 1;
	}
}
std::string 	RequestParse::getCGIfile()
{
	return _outfile;
}
// hidriouc part :
std::vector<std::string> RequestParse::_buildEnvVars() 
{
	std::vector<std::string> env_vars;

	env_vars.push_back("REQUEST_METHOD=" + _method);
	env_vars.push_back("QUERY_STRING=" + _queryString);
	env_vars.push_back("CONTENT_LENGTH=" + _metaData["content-length"]);
	env_vars.push_back("CONTENT_TYPE=" + _metaData["content-type"]);
	env_vars.push_back("SCRIPT_FILENAME=" + _configServer.getAttributes("root") + _url);
	env_vars.push_back("SCRIPT_NAME=" + _url);
	env_vars.push_back("SERVER_PROTOCOL=http/1.1");
	env_vars.push_back("REMOTE_ADDR=" + _metaData["host"]);
	env_vars.push_back("REMOTE_PORT=");
	env_vars.push_back("HTTP_USER_AGENT=PostmanRuntime/7.29.0");
	env_vars.push_back("HTTP_COOKIE=" + _metaData["cookie"]);
	env_vars.push_back("SERVER_NAME=" + _configServer.getAttributes("server_name"));
	env_vars.push_back("SERVER_PORT=" + _configServer.getlistening()[0].second);
	env_vars.push_back("QUERY_STRING=" + _queryString);

	return env_vars;
}

void RequestParse::_openFileSafely(std::ifstream& file, const std::string& filename) 
{
	file.open(filename.c_str());
	if (!file.is_open())
		throw (std::string)("Error opening file: " + filename);
}

std::string RequestParse::_extractHeaderValue(const std::string& line)
{
	size_t pos = line.find(':');
	if (pos == std::string::npos)
		return "";
	std::string value = Utility::trimJstSpace(line.substr(pos + 1));
	if (!value.empty() && value[value.length() - 1] == '\r')
		value.erase(value.length() - 1);
	return value;
}

void	RequestParse::_validateContentLength(const std::string& contentLength, size_t bodysize)
{
	if (contentLength.empty() || contentLength.find_first_not_of("0123456789") != std::string::npos ||
		(int)bodysize != Utility::StrToInt(contentLength))
		throw (std::string)("ERROR: Unexpected Content-Length!");
}

void RequestParse::_validateContentType(const std::string& contentType)
{
	if (contentType.empty() || !(contentType == "Content-Type: text/html" || contentType == "Content-Type: text/json"))
		throw (std::string)("ERROR: Unexpected Content-Type!");
}

void RequestParse::_parseHeaderLine(const std::string& line, std::string lines[])
{
	if (line.find("HTTP/") != std::string::npos && lines[0].empty()){
		if (!lines[0].empty())
			throw((std::string)"Deplucate Start header !");
		lines[0] = line;
	}
	else if (line.find("Content-Length:") != std::string::npos){
		if (!lines[1].empty())
			throw((std::string)"Content-Length !");
		lines[1] = _extractHeaderValue(line);
	}
	 if (line.find("Content-Type:") != std::string::npos){
		if (!lines[2].empty())
			throw((std::string)"Content-Type!");
		lines[2] = _extractHeaderValue(line);
	}
}

int	RequestParse::_parseHeaders(size_t bodysize, const std::string& headers)
{
	std::stringstream ss(headers);
	std::string line;

	(void)bodysize;
	while (std::getline(ss, line) && line.find ("Content-Typ") == std::string::npos)
		;
	_validateContentType(line);
	return 200;
}
void	RequestParse::_dupfd(int infd, int outfd, int ERRfile)
{
	(void) ERRfile;
	if (infd > 0)
		if (dup2(infd, STDIN_FILENO) == -1){
		perror ("dup2 failed !!");
		exit(EXIT_FAILURE);
		}
	if (dup2(outfd, STDOUT_FILENO) == -1){
		perror ("dup2 failed !!");
		exit(EXIT_FAILURE);
	}
	if (dup2(ERRfile, STDERR_FILENO) == -1){
		perror ("dup2 failed !!");
		exit(EXIT_FAILURE);
	}
}

int RequestParse::_forkAndExecute(int infd, int outfd, char* env[], int ERRfile)
{
	int pid = fork();
	if (pid == 0)
	{
		std::string exte = "cgi" + _url.substr(_url.rfind('.'));
		_dupfd(infd, outfd, ERRfile);
		std::string scriptPath = _url;
		std::string AbsPath = _configServer.getLocation(_location)[exte].substr(_configServer.getLocation(_location)[exte].find(' ')+ 1) ;
		char* args[] = {(char*)AbsPath.c_str(), (char*)scriptPath.c_str(), NULL};
		if (chdir(scriptPath.substr(0, scriptPath.rfind("/")).c_str()) == -1)
			;
		execve(AbsPath.c_str(), args, env);
		exit(1);
	}
	return pid;
}

int RequestParse::_waitForCGIProcess(int pid)
{
	int status, elapsed_time = 0;

	while (elapsed_time < CGI_TIMEOUT) 
	{
		usleep(100000);
		elapsed_time++;
		if (waitpid(pid, &status, WNOHANG) > 0){
			if (WEXITSTATUS(status) == 1)
				throw ((std::string) "exicve() failed !");
			return 200;	
		}
	}
	kill(pid, SIGKILL);
	waitpid(pid, &status, 0);
	return 504;
}
int RequestParse::parseCGIOutput()
{
	std::ifstream file;
	_openFileSafely(file, _outfile.c_str());
	std::string lines, buffer;
	char buf[SIZE_BUFFER + 1] = {0};
	while (file.read(buf, SIZE_BUFFER) || file.gcount() > 0)
	{
		buf[file.gcount()] = '\0';
		lines.append(buf, file.gcount());
	}
	if (lines.empty())
		return 500;
	size_t headerEnd = lines.find("\r\n\r\n");
	if (headerEnd == std::string::npos) 
		throw ((std::string)"No valid header separator found.");
	return _parseHeaders(lines.substr(headerEnd + 4).size(), lines.substr(0, headerEnd));
	return 200;
}

bool	RequestParse::is_InvalideURL()
{
	std::string scriptPath = "";
	if (_configServer.getLocation(_location)["root"] != "")
	{
		scriptPath = _configServer.getLocation(_location)["root"] + _url;
		if (Utility::isDirectory(scriptPath) && _configServer.getLocation(_location)["index"] != "")
			scriptPath += _configServer.getLocation(_location)["index"];
	}
	else
	{
		scriptPath = _configServer.getAttributes("root") + _url;
		if (Utility::isDirectory(scriptPath) && _configServer.getLocation(_location)["index"] != "")
			scriptPath +=  _configServer.getLocation(_location)["index"];
	}
	_url = scriptPath;
	if (access(scriptPath.c_str(), F_OK) != 0)
	{
		_statusCode = (status)(404);
		_statusCodeMessage = "404 Not Found";
		return 1;
	}
	if (access(scriptPath.c_str(), X_OK) != 0)
	{
		_statusCode = (status)(403);
		_statusCodeMessage = "403 Forbidden";
		return 1;
	}
	return 0;
}

void	RequestParse::clear(int bodyfd, int outfd, int fileERR)
{
	close (bodyfd);
	close (outfd);
	close (fileERR);
	unlink("/tmp/Errout.text");
	unlink(_body.BodyFileName().c_str());
}
bool RequestParse::CheckStdERR(const char* fileERRpath)
{
	char buffer[1024];
	
	memset(buffer, 0, sizeof(buffer));
	int fileERR = open(fileERRpath, O_RDONLY);
	if (fileERR == -1 || read(fileERR, buffer, sizeof(buffer) - 1) > 0)
		return close(fileERR ),true;
	return close(fileERR), 0;

}
int	RequestParse::runcgiscripte()
{
	std::vector<std::string> env_strings = _buildEnvVars();
	char*	env[env_strings.size() + 1];
	size_t	i = 0;
	int		bodyfd = -1;
	int		outfd;
	int		fileERR;

	if (is_InvalideURL())
		return 1;
	try 
	{
		for (; i < env_strings.size(); ++i)
			env[i] = (char*)env_strings[i].c_str();
		env[i] = NULL;
		if (_method == "POST"){
			bodyfd = open(_body.BodyFileName().c_str(), O_RDONLY);
			if (bodyfd == -1)
				throw (std::string)("Failed to open body file");
		}
		_outfile = ("/tmp/outCGI" + Utility::GetCurrentT() + ".out");
		outfd = open(_outfile.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0644);
		if (outfd == -1) {
			if (_method == "POST") close(bodyfd);
				unlink("/tmp/outCGI.text");
			throw (std::string)("Failed to open output file");
		}
		fileERR = open("/tmp/Errout.text", O_RDWR | O_CREAT, 0644);
		if (fileERR == -1) {
			if (_method == "POST") close(bodyfd);
				unlink("/tmp/outCGI.text");
			throw (std::string)("Failed to open output file");
		}
		int	pid = _forkAndExecute(bodyfd, outfd, env, fileERR);
		if (pid < 0) 
			throw (std::string)("Fork failed");
		_statusCode = _waitForCGIProcess(pid) == 200 ? (status)parseCGIOutput() : (status)504;
		if (_statusCode == 504)
			_statusCodeMessage = "504 Gateway Timeout";
		if (CheckStdERR("/tmp/Errout.text"))
			throw (std::string("500 Internal Server Error"));
	}catch(std::string err){
		std::cout << ">>>" << err << "\n";
		_statusCode = (status)(500);
		_statusCodeMessage = "500 Internal Server Error";
	}
	clear(bodyfd ,outfd, fileERR );
	return 402;
}

RequestParse::~RequestParse()
{
	_header.clear();
}