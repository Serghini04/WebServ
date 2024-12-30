/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meserghi <meserghi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/29 18:35:29 by meserghi          #+#    #+#             */
/*   Updated: 2024/12/30 20:22:19 by meserghi         ###   ########.fr       */
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
	if (_url.empty())
		throw std::runtime_error("400 Bad Request");
	if (_httpVersion != "HTTP/1.1")
		throw std::runtime_error("400 Bad Request");
}

void	RequestParse::parseMetaData(std::string header)
{
	
	for (int i = 0; header[i]; i++)
	{
		if (!std::isspace(header[i]) && header[i] != ':')
			continue;
		else if (i != 0 && header[i] == ':')
		{
			_metaData[header.substr(0, i)] = header.substr(i + 1);
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


void    RequestParse::readBuffer(std::string buff)
{
	static std::string	header;
	static int isHeaderDone = 0;
    
	if (!isHeaderDone)
	{	
		header.append(buff);
		if (buff.find("\r\n\r\n") == std::string::npos)
			return ;
		isHeaderDone = parseHeader(header);
	}
}
