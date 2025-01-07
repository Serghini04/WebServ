/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mal-mora <mal-mora@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/29 18:35:29 by meserghi          #+#    #+#             */
/*   Updated: 2025/01/07 10:55:49 by mal-mora         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include <RequestParse.hpp>

// RequestParse::RequestParse(const RequestParse& rqs)
// {
// 	*this = rqs;
// }
// RequestParse& RequestParse::operator=(const RequestParse& other) {
//     if (this != &other) {

//         // Free existing resources
//         // Copy members from 'other'
//     }
//     return *this;
// }

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
	// std::cout <<"ff :" << _url << "\n";
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

bool	isNotSpace(int ch)
{
	return !std::isspace(ch);
}
int		RequestParse::method()
{
	return this->_enumMethod;
}
std::string	RequestParse::trimSpace(std::string line)
{
	std::string::iterator first = std::find_if(line.begin(), line.end(), isNotSpace);
	if (first == line.end())
		return "";
	std::string::iterator last = std::find_if(line.rbegin(), line.rend(), isNotSpace).base();
	return std::string(first, last);
}

void	RequestParse::parseMetaData(std::string header)
{
	
	for (int i = 0; header[i]; i++)
	{
		if (!std::isspace(header[i]) && header[i] != ':')
			continue;
		else if (i != 0 && header[i] == ':')
		{
			_metaData[header.substr(0, i)] = trimSpace(header.substr(i + 1));
			return ;
		}
		else
			break ;
	}
	throw std::runtime_error("400 Bad Request");
}

int RequestParse::parseHeader(std::string &header)
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
				return 0;
			start = i + 1; 
		}
	}
	return 1;
}
RequestParse::RequestParse()
{
	_fd.open("output.txt", std::ios::binary | std::ios::app);
	_requestIsDone = false;
	_statusCode = eOK;
}

void    RequestParse::readBuffer(std::string buff, int &isHeader)
{
	static std::string	header;
	static	BodyType type = eNone;

	if (_requestIsDone)
		return ;
	if (isHeader)
	{
		header.append(buff);
		if (buff.find("\r\n\r\n") == std::string::npos)
			return ;
		isHeader = parseHeader(header);
		_body.setMetaData(_metaData);
		buff = header.substr(header.find("\r\n\r\n") + 4);
		type = _body.getTypeOfBody();
		_body.openFileBasedOnContentType();
		header.clear();
	}
	_fd << "\n======================\n";
	_fd << buff; 
	_fd << "\n======================\n";
	switch (type)
	{
		case eBoundary :
			_body.BoundaryParse(buff);
			break;
		case eChunked :
			_requestIsDone = _body.ChunkedParse(buff);
			break;
		case eChunkedBoundary :
			_body.ChunkedBoundaryParse(buff);
			break;
		case eContentLength :
			_body.ContentLengthParse(buff);
			break;
		case eNone :
			break;
	}
	// isHeaderDone = !_requestIsDone;
}