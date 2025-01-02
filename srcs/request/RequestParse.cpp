/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meserghi <meserghi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/29 18:35:29 by meserghi          #+#    #+#             */
/*   Updated: 2025/01/02 16:02:51 by meserghi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include <RequestParse.hpp>


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
}

std::map<std::string, std::string>	&RequestParse::getMetaData()
{
	return (_metaData);
}

bool	isNotSpace(int ch)
{
	return !std::isspace(ch);
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
				return i + 3;
			start = i + 1; 
		}
	}
    return 0;
}
RequestParse::RequestParse()
{
	_fd.open("output.txt", std::ios::binary | std::ios::app);
	
}

void    RequestParse::readBuffer(std::string buff)
{
	static std::string	header;
	static int isHeaderDone = 0;
	static	BodyType type = eNone;

	_fd << "\n=======================\n";
    _fd << buff ;
	_fd << "\n=======================\n";
	if (!isHeaderDone)
	{	
		header.append(buff);
		if (buff.find("\r\n\r\n") == std::string::npos)
			return ;
		isHeaderDone = parseHeader(header);
		buff = header.substr(isHeaderDone);
		type = _body.getTypeOfBody(this->getMetaData());
	}
	switch (type)
	{
		case eBoundary :
			_body.BoundaryParse(buff);
			break;
		case eChunked :
			_body.ChunkedParse(buff);
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
}
