/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meserghi <meserghi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/29 18:35:29 by meserghi          #+#    #+#             */
/*   Updated: 2025/01/16 17:38:46 by meserghi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include <RequestParse.hpp>

RequestParse::RequestParse()
{
	_fd.open("/Users/mal-mora/goinfre/Output.trash", std::ios::binary | std::ios::app);
	if (_fd.fail())
	{
		puts("open failed");
		exit(1);
	}
	_isHeader = true;
	_requestIsDone = false;
	_statusCode = eOK;
	_statusCodeMessage = "200 created";
}

std::string	RequestParse::statusCodeMessage()
{
	return _statusCodeMessage;
}
void	RequestParse::SetStatusCodeMsg(std::string s)
{
	this->_statusCodeMessage = s;
}
void	RequestParse::parseFirstLine(std::string  header)
{
	std::stringstream    ss(header);

	if (!std::isalpha(header[0]))
		throw std::runtime_error("400 Bad Request1");
	ss >> _method >> _url >> _httpVersion;
	if (_method != "GET" && _method != "POST" && _method != "DELETE")
		throw std::runtime_error("400 Bad Request2");
	if (_url.empty() || _httpVersion.empty())
		throw std::runtime_error("400 Bad Request3"); 
	if (_httpVersion != "HTTP/1.1")
		throw std::runtime_error("400 Bad Request4");
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
	_body.setbodyType(_body.getTypeOfBody());
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
	if (_requestIsDone)
		return ;
	// _fd << "\n===========" << _body.bodyType() << "===========\n";
	// _fd << buff; 
	// _fd << "\n======================\n";
	// _fd.flush();
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