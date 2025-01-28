/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meserghi <meserghi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/29 18:35:29 by meserghi          #+#    #+#             */
/*   Updated: 2025/01/28 17:56:41 by meserghi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include <RequestParse.hpp>

RequestParse::RequestParse(Conserver &conserver) : _body(conserver.getBodySize()), _configServer(conserver)
{
	std::cout << "\n============>> Request Start Here <<==============\n" << std::flush;
	_fd.open("/Users/meserghi/goinfre/www/Output.trash", std::ios::binary | std::ios::app);
	if (_fd.fail())
		throw std::runtime_error("500 Internal Server Error");
	_isHeader = true;
	_requestIsDone = false;
	_statusCode = eOK;
	_statusCodeMessage = "200 OK";
	_maxBodySize = _configServer.getBodySize();
}

bool	isDuplicate(char a, char b)
{
	return a == '/' && b == '/';
}

void	RequestParse::checkURL()
{
	const std::string invalidChars = "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F"
									"\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1A\x1B\x1C\x1D\x1E\x1F"
									"\x7F\"<>\\^`{|}";

	if (_url.length() > 4096)
		throw std::runtime_error("414 URI Too Long");
	if (_url[0] != '/')
		throw std::runtime_error("400 Bad Request");
	if (_url.find_first_of(invalidChars) != std::string::npos)
		throw std::runtime_error("400 Bad Request");
	_url.erase(std::unique(_url.begin(), _url.end(), isDuplicate), _url.end());
	// std::cout << "After : {" << _url << "}" << std::endl;
	for (size_t i = 0; i < _url.length(); i++)
	{
		if (_url[i] == '%' && i + 2 < _url.length())
		{
			if (std::isxdigit(_url[i + 1]) && std::isxdigit(_url[i + 2]))
				_url.replace(i, 3, Utility::percentEncoding(_url.substr(i, 3)));
			else
				throw std::runtime_error("400 Bad Request");
			i++;
		}
		else if (_url[i] == '%')
			throw std::runtime_error("400 Bad Request");
	}
	// std::cout << "Before : {" << _url << "}" << std::endl;

	// I need to add handel Query string
	// Fragment Handing : * Validate fragment format * Ensure no sensitive data in fragments

	if (_url.find("/../") != std::string::npos || _url == "/.." || _url.rfind("/..") == _url.length() - 3)
		throw std::runtime_error("400 Bad Request");
}

std::string	RequestParse::statusCodeMessage()
{
	return _statusCodeMessage;
}

void	RequestParse::parseFirstLine(std::string  header)
{
	std::stringstream    ss(header);

	if (!std::isalpha(header[0]))
		throw std::runtime_error("400 Bad Request1");
	ss >> _method >> _url >> _httpVersion;
	if (_method != "GET" && _method != "POST" && _method != "DELETE")
		throw std::runtime_error("400 Bad Request");
	if (_url.empty() || _httpVersion.empty())
		throw std::runtime_error("400 Bad Request"); 
	if (_httpVersion != "HTTP/1.1")
		throw std::runtime_error("400 Bad Request");
	checkURL();
	if (_method == "GET")
		_enumMethod = eGET;
	else if (_method == "POST")
		_enumMethod = ePOST;
	else
		_enumMethod = eDELETE;
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
	throw std::runtime_error("400 Bad Request");
}

bool	RequestParse::parseHeader(std::string &header)
{
	int firstLine = 1;
	size_t start = 0;

	if (header.empty())
		throw std::runtime_error("400 Bad Request");
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
	std::cout << "based on this location =>" << location << "<" << std::endl;
	if (_configServer.getLocation(location)["allowed_methods"].find(Utility::toUpperCase(_method)) == std::string::npos)
		throw std::runtime_error("405 Method Not Allowed");
	if (_enumMethod == ePOST)
		_body.setFileName(_configServer.getLocation(location)["upload_store"] + "/Output");
}

void	RequestParse::deleteURI()
{
    if (!Utility::isReadableFile(_uri))
	{
		std::cerr << ">>" <<_uri << "<<\n"; 
		throw std::runtime_error("403 Forbidden1");
	}
    if (!Utility::isDirectory(_uri))
        std::remove(_uri.c_str());
    else
    {
        DIR* currentDir = opendir(_uri.c_str());
        dirent* dp;
        std::string targetFile;
        if (!currentDir)
            throw std::runtime_error("500 Internal Server Error");
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
	if (_configServer.getLocation(_location)["root"] != "")
		_uri = _configServer.getAttributes("root") + "/" + _configServer.getLocation(_location)["root"];
	else
		_uri = _configServer.getAttributes("root");
	_uri +=  "/" + _url;
	std::cerr << ">>" << _uri << "<<\n";
	if (!Utility::checkIfPathExists(_uri))
		throw std::runtime_error("404 Not Found");
	if (Utility::isDirectory(_uri))
	{
		if (_configServer.getLocation(_location)["index"] == "")
			throw std::runtime_error("403 Forbidden");
		_uri += _configServer.getLocation(_location)["index"];
	}
	// you need to check if cgi not delete;
	deleteURI();
	throw std::runtime_error("204 No Content");
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
	_body.setbodyType(_body.getTypeOfBody(_enumMethod, _maxBodySize));
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
	if (_enumMethod == eGET)
		throw std::runtime_error("200 OK");
	else if (_enumMethod == eDELETE)
		deleteMethod();
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

void    RequestParse::readBuffer(std::string buff)
{
	static std::string	header;
	_fd << "\n===========" << _body.bodyType() << "===========\n";
	_fd << buff; 
	_fd << "\n======================\n";
	_fd.flush();
	try
	{
		if (_requestIsDone)
			return ;
		if (isHeader())
			setIsHeader(parseHeader(header, buff));
		_requestIsDone = _body.parseBody(buff);
	}
	catch (std::exception &e)
	{
		header.clear();
		_statusCode = (status)atoi(e.what());
		if (_statusCode < 200)
		{
			_statusCodeMessage = "500 Internal Server Error";
			_statusCode = eInternalServerError;
		}
		else
			_statusCodeMessage = e.what();
		std::cerr << _statusCodeMessage << std::endl;
		_requestIsDone = 1;
	}
	catch (...)
	{
		header.clear();
		_statusCodeMessage = "500 Internal Server Error";
		_statusCode = eInternalServerError;
		_requestIsDone = 1;
	}
}

RequestParse::~RequestParse()
{
	_fd.close();
	// std::cout << "\n============>> Request Done Here <<==============\n" << std::flush;
}