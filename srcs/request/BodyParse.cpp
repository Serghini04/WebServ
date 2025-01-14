/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BodyParse.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meserghi <meserghi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/31 13:38:49 by meserghi          #+#    #+#             */
/*   Updated: 2025/01/14 10:31:22 by meserghi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include <BodyParse.hpp>

BodyParse::BodyParse()
{
    _type = eNone;
	_sizeRead = 0;
	_boundary = "";
	_indexFile = 1;
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
    if (_metaData["Transfer-Encoding"] == "=chunked" && _metaData["Content-Type"].find("multipart/form-data; boundary=") != std::string::npos)
		return eChunkedBoundary;
	else if (_metaData["Transfer-Encoding"] == "chunked")
		return eChunked;
	else if (_metaData["Content-Type"].find("multipart/form-data; boundary=") != std::string::npos)
		return eBoundary;
	// case : POST
	// 411 Length Required
	// 400 Bad Request if Content type is not define for case POST
	return eContentLength;
}

void	BodyParse::setbodyType(BodyType type)
{
	_type = type;
}

BodyType	BodyParse::bodyType()
{
	return (_type);
}

size_t		BodyParse::sizeRead()
{
	return (_sizeRead);
}

void BodyParse::openFileBasedOnContentType()
{
	std::map<std::string, std::string> Extensions;

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
	oss << "/Users/meserghi/goinfre/D/Output" << _indexFile;
	std::string namefile = oss.str();
    
	if (_metaData["Content-Type"] == "")
		namefile += ".txt";
	else
		namefile += Extensions[_metaData["Content-Type"]];
	_fileOutput.open(namefile.c_str(), std::ios::binary);
	if (_fileOutput.fail())
		throw std::runtime_error("open failed !");
	_indexFile++;
}

void	BodyParse::ChunkedBoundaryParse(std::string &buff)
{
	(void)buff;
}

void	BodyParse::setBoundary(std::string boundary)
{
	_boundary = boundary;
}

void	BodyParse::setBoundaryEnd(std::string boundary)
{
	_boundaryEnd = boundary;
}

void	BodyParse::openFileOfBoundary(std::string buff)
{
	std::string	target, filename;
	size_t	start, end;
	std::ostringstream oss;

	if (_fileOutput.is_open())
		_fileOutput.close();
	if (buff.find("filename=\"") != std::string::npos)
	{
		target = "filename=\"";
		start = buff.find(target) + target.length();
		end = buff.find('\"', start);
		if (end == std::string::npos)
			throw std::runtime_error("Invalid Exec to open file");
		filename = buff.substr(start, end - start);
		start = filename.rfind(".");
		if (start != std::string::npos)
			oss << "/Users/meserghi/goinfre/D/Output" << _indexFile << filename.substr(start);
		else
			oss << "/Users/meserghi/goinfre/D/Output" << _indexFile << ".txt";
	}
	else if (buff.find("Content-Type: ") != std::string::npos)
	{
		target = "Content-Type: ";
		start = buff.find(target) + target.length();
		end = buff.find("\r\n", start);
		if (end == std::string::npos)
			throw std::runtime_error("Invalid Exec to open file");
		filename =  Utility::trimSpace(buff.substr(start, end - start));
		_metaData["Content-Type"] = filename;
		openFileBasedOnContentType();
		return ;
	}
	else
		oss << "/Users/meserghi/goinfre/D/Output" << _indexFile << ".txt";
	std::cout <<"Create File :>>" << oss.str() << "<<\n";
	_fileOutput.open(oss.str(), std::ios::binary);
	if (_fileOutput.fail())
		throw std::runtime_error("open failed !");
	_indexFile++;
}

// My version :
bool BodyParse::BoundaryParse(std::string& buff)
{
	static std::string data;
	size_t processed = 0;

	if (!data.empty())
	{
		buff = data + buff;
		data.clear();
	}

	while (processed < buff.size())
	{
		size_t boundaryPos = buff.find(_boundary, processed);
		if (boundaryPos != std::string::npos)
		{
			// IF "DATA BOUNDARY..."
			if (boundaryPos > processed)
				_fileOutput.write(buff.data() + processed, boundaryPos - processed - 2);
			size_t headerEndPos = buff.find("\r\n\r\n", boundaryPos);
			if (headerEndPos == std::string::npos)
			{
				data = buff.substr(boundaryPos);
				return false;
			}
			openFileOfBoundary(buff.substr(boundaryPos, headerEndPos - boundaryPos + 4));
			processed = headerEndPos + 4;
			continue ;
		}
		size_t boundaryEndPos = buff.find(_boundaryEnd, processed);
		if (boundaryEndPos != std::string::npos)
		{
			if (boundaryEndPos >= 2)
				boundaryEndPos -= 2;
			_fileOutput.write(buff.data() + processed, boundaryEndPos - processed);
			_fileOutput.flush();
			buff.erase(0, boundaryEndPos + _boundaryEnd.size() + 2);
			return true;
		}
		if (processed < buff.size() && _boundaryEnd.find(buff.back()) == std::string::npos)
		{
			_fileOutput.write(buff.data() + processed, buff.size() - processed);
			buff.clear();
			return false;
		}
		data = buff.substr(processed);
		buff.clear();
	}
    return false;
}

bool BodyParse::ChunkedParse(std::string &buff)
{
	static std::string data;
	static size_t length = 0;
	static bool readingChunk = false;

	if (!data.empty())
	{
		buff = data + buff;
		data.clear();
	}
    while (!buff.empty())
	{
		if (!readingChunk)
		{
			size_t pos = buff.find("\r\n");
			if (pos == std::string::npos)
				return data = buff, false;
			std::string lengthStr = buff.substr(0, pos);
			buff.erase(0, pos + 2);
			if (lengthStr.empty())
				continue;
			char *trash = NULL;
			length = std::strtol(lengthStr.c_str(), &trash, 16);
			if (*trash || length == SIZE_MAX)
			{
				std::cerr << "Invalid chunk size format!" << std::endl;
				return false;
			}
			if (length == 0)
				return _fileOutput.flush(), true;
			readingChunk = true;
		}
		size_t bytesToRead = std::min(length, buff.size());
		_fileOutput.write(buff.c_str(), bytesToRead);
		length -= bytesToRead;
		buff.erase(0, bytesToRead);
		if (length == 0)
		{
			if (buff.size() >= 2)
				buff.erase(0, 2);
			readingChunk = false;
		}
	}
	return false;
}

void	BodyParse::ContentLengthParse(std::string &buff)
{
	(void)buff;
}
