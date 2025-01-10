/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BodyParse.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mal-mora <mal-mora@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/31 13:38:49 by meserghi          #+#    #+#             */
/*   Updated: 2025/01/09 19:11:45 by mal-mora         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include <BodyParse.hpp>

BodyParse::BodyParse()
{
    _type = eNone;
	_sizeRead = 0;
}

// chunked && boundray ==>
// "Content-Type" = "multipart/form-data" && "Transfer-Encoding" == "chunked"
// chunked ==>
//  "Transfer-Encoding" == "chunked"
// boundray ==>
// "Content-Type" == "multipart/form-data" && "Transfer-Encoding" != "chunked"
//  Content-Length ==>
// Content-Length || !Content-Length

void		BodyParse::setMetaData(std::map<std::string, std::string> &data)
{
	_metaData = data;
}

BodyType	BodyParse::getTypeOfBody()
{
    if (_metaData["Transfer-Encoding"] == "=chunked" && _metaData["Content-Type"].find("multipart/form-data") != std::string::npos)
		return eChunkedBoundary;
	else if (_metaData["Transfer-Encoding"] == "chunked")
		return eChunked;
	else if (_metaData["Content-Type"].find("multipart/form-data") != std::string::npos)
		return eBoundary;
	// case : POST
	// 411 Length Required
	// 400 Bad Request if Content type is not define for case POST
	return eContentLength;
}

void BodyParse::openFileBasedOnContentType()
{
	std::map<std::string, std::string> Extensions;
	static int index = 1;

	Extensions["image/jpeg"] = ".jpg";
	Extensions["image/png"] = ".png";
	Extensions["image/gif"] = ".gif";
	Extensions["image/bmp"] = ".bmp";
	Extensions["image/webp"] = ".webp";
	Extensions["text/plain"] = ".txt";
	Extensions["text/html"] = ".html";
	Extensions["text/css"] = ".css";
	Extensions["text/javascript"] = ".js";
	Extensions["application/json"] = ".json";
	Extensions["application/pdf"] = ".pdf";
	Extensions["application/xml"] = ".xml";
	Extensions["application/zip"] = ".zip";
	Extensions["audio/mpeg"] = ".mp3";
	Extensions["audio/wav"] = ".wav";
	Extensions["video/mp4"] = ".mp4";
	Extensions["video/mpeg"] = ".mpeg";
	Extensions["video/webm"] = ".webm";
	Extensions["application/octet-stream"] = ".bin";
    
	std::ostringstream oss;
	oss << "RequestPOSTS/Output" << index;
	std::string namefile = oss.str();
    
	if (_metaData["Content-Type"] == "")
		namefile += ".txt";
	else
		namefile += Extensions[_metaData["Content-Type"]];
	_fileOutput.open(namefile.c_str(), std::ios::binary);
	if (_fileOutput.fail())
		throw std::runtime_error("open failed !");
	index++;
}

void	BodyParse::ChunkedBoundaryParse(std::string &buff)
{
	(void)buff;
}

void	BodyParse::BoundaryParse(std::string &buff)
{
	(void)buff;

}

bool BodyParse::ChunkedParse(std::string &buff)
{
	static std::string data;
	static size_t length = 0;
	char *trash = NULL;    

	if (!data.empty())
	{
		buff = data.append(buff);
		data.clear();
	}
	while (!buff.empty())
	{
		if (length == 0)
		{
			size_t pos = buff.find("\r\n");
			if (pos == std::string::npos)
				return data = buff, false;
			std::string lengthStr = buff.substr(0, pos);
			length = std::strtol(lengthStr.c_str(), &trash, 16);
			if (*trash)
			{
				std::cerr << "Invalid chunk size format!" << std::endl;
				return false;
			}
			buff = buff.substr(pos + 2);
			if (length == 0)
			{
				puts("Request Done");
				_fileOutput.flush();
				return true;
			}
		}
		if (buff.size() >= length + 2)
		{
			_fileOutput.write(buff.c_str(), length);
			_fileOutput.flush();
			buff = buff.substr(length + 2);
			length = 0;
		}
		else
		{
			_fileOutput.write(buff.c_str(), buff.size());
			_fileOutput.flush();
			length -= buff.size();
			buff.clear();
		}
	}
	return false;
}




void	BodyParse::ContentLengthParse(std::string &buff)
{
	(void)buff;
}
