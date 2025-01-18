/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meserghi <meserghi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/29 18:35:29 by meserghi          #+#    #+#             */
/*   Updated: 2025/01/18 14:29:50 by meserghi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include <RequestParse.hpp>

RequestParse::RequestParse(Conserver &conserver) : _configServer(conserver)
{
	_fd.open("/Users/meserghi/goinfre/D/Output.trash", std::ios::binary | std::ios::app);
	if (_fd.fail())
	{
		puts("open failed");
		exit(1);
	}
	_isHeader = true;
	_requestIsDone = false;
	_statusCode = eOK;
	_statusCodeMessage = "20 OK";
	_maxBodySize = atoll(_configServer.getAttributes("client_max_body_size").c_str());
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
}

void	RequestParse::parseFirstLine(std::string  header)
{
	std::stringstream    ss(header);

	if (!std::isalpha(header[0]))
		throw std::runtime_error("400 Bad Request");
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

std::string	RequestParse::statusCodeMessage()
{
	return _statusCodeMessage;
}

void	RequestParse::SetstatusCodeMessage(std::string message)
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

void	RequestParse::SetisHeader(bool isHeader)
{
	_isHeader = isHeader;
}

bool	RequestParse::isHeader()
{
	return _isHeader;
}

void	RequestParse::parseMetaData(std::string header)
{
	
	for (int i = 0; header[i]; i++)
	{
		if (!std::isspace(header[i]) && header[i] != ':')
			continue;
		else if (i != 0 && header[i] == ':')
		{
			_metaData[header.substr(0, i)] = Utility::trimSpace(header.substr(i + 1));
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

bool RequestParse::readHeader(std::string &buff)
{
	static std::string	header;
	bool	isHeader = true;

	header.append(buff);
	size_t pos = header.find("\r\n\r\n");
	if (pos == std::string::npos)
		return isHeader;
	isHeader = parseHeader(header);
	_body.setMetaData(_metaData);
	buff = header.substr(pos + 4);
	_body.setbodyType(_body.getTypeOfBody(_enumMethod, _maxBodySize));
	if (_body.bodyType() == eBoundary || _body.bodyType() == eChunkedBoundary)
	{
		std::string	boundary = _metaData["Content-Type"].substr(_metaData["Content-Type"].find("boundary=") + 9);
		_body.setBoundary("--" + boundary + "\r\n");
		_body.setBoundaryEnd("--" + boundary + "--\r\n");
	}
	if (_body.bodyType() == eChunked || _body.bodyType() == eContentLength)
		_body.openFileBasedOnContentType();
	header.clear();
	if (_enumMethod == eGET)
		_requestIsDone = true;
	return isHeader;
}

void    RequestParse::readBuffer(std::string buff)
{
	_fd << "\n===========" << _body.bodyType() << "===========\n";
	_fd << buff; 
	_fd << "\n======================\n";
	_fd.flush();
	if (_requestIsDone)
		return ;
	try
	{
		if (isHeader())
			SetisHeader(readHeader(buff));
		switch (_body.bodyType())
		{
			case eBoundary :
				_requestIsDone = _body.BoundaryParse(buff);
				break;
			case eChunked :
				_requestIsDone = _body.ChunkedParse(buff);
				break;
			case eChunkedBoundary :
				_requestIsDone = _body.ChunkedBoundaryParse(buff);
				break;
			case eContentLength :
				_requestIsDone = _body.ContentLengthParse(buff);
				break;
			case eNone :
				break;
		}
	}
	catch (std::string &e)
	{
		_statusCode = (status)atoi(e.c_str());
		_requestIsDone = 1;
	}
	catch (...)
	{
		_statusCode = eBadRequest;
		_requestIsDone = 1;
	}
}