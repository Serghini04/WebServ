/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BodyParse.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meserghi <meserghi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/31 13:38:49 by meserghi          #+#    #+#             */
/*   Updated: 2025/01/18 13:04:08 by meserghi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include <BodyParse.hpp>

size_t	BodyParse::_indexFile = 1;

BodyParse::BodyParse()
{
    _type = eNone;
	_bodySize = 0;
	_boundary = "";
}

void		BodyParse::setMetaData(std::map<std::string, std::string> &data)
{
	_metaData = data;
}

BodyType	BodyParse::getTypeOfBody(methods method, long long maxBodySize)
{
	(void)maxBodySize;
	if (_metaData["Content-Length"] != "")
	{
		_bodySize = atoll(_metaData["Content-Length"].c_str());
		// if (_bodySize > maxBodySize)
		// 	throw std::runtime_error("413 Content Too Large");
		// puts("ff");
	}
    if (_metaData["Transfer-Encoding"] == "chunked" && _metaData["Content-Type"].find("multipart/form-data; boundary=") != std::string::npos)
		return eChunkedBoundary;
	else if (_metaData["Transfer-Encoding"] == "chunked")
	{
		if (method == ePOST && _metaData["Content-Type"] == "")
			throw std::runtime_error("400 Bad Request");
		return eChunked;
	}
	else if (_metaData["Content-Type"].find("multipart/form-data; boundary=") != std::string::npos)
		return eBoundary;
	else if (_metaData["Content-Length"] != "")
	{
		if (method == ePOST && _metaData["Content-Type"] == "")
			throw std::runtime_error("400 Bad Request");
		_bodySize = atoll(_metaData["Content-Length"].c_str());
		return eContentLength;
	}
	if (method == ePOST)
		throw std::length_error("411 Length Required");
	return eNone;
}

void	BodyParse::setbodyType(BodyType type)
{
	_type = type;
}

BodyType	BodyParse::bodyType()
{
	return (_type);
}

long long		BodyParse::sizeRead()
{
	return (_bodySize);
}

void BodyParse::openFileBasedOnContentType()
{
	std::ostringstream oss;
	if (_fileOutput.is_open())
		_fileOutput.close();
	oss << "/Users/meserghi/goinfre/D/Output" << _indexFile;
	std::string namefile = oss.str();
	namefile += Utility::getExtensions(_metaData["Content-Type"], "");
	_fileOutput.open(namefile.c_str(), std::ios::binary);
	if (_fileOutput.fail())
		throw std::runtime_error("open failed !");
	std::cout <<"Create File :>>" << namefile << "<<\n";
	_indexFile++;
}

bool	BodyParse::writeChunkToFile(std::string &buff, size_t &length, std::string &carryOver, std::string &accumulatedData)
{
	size_t bytesToRead = std::min(length, buff.size());
	std::string chunkData = buff.substr(0, bytesToRead);
	std::string searchData = carryOver + chunkData;
	carryOver.clear();
	size_t boundaryPos = searchData.find(_boundary);
	size_t boundaryEndPos = searchData.find(_boundaryEnd);
	
	if (boundaryPos != std::string::npos || boundaryEndPos != std::string::npos)
	{
	    accumulatedData += searchData;
	    std::string tempBuff = accumulatedData;
	    accumulatedData.clear();
	    if (BoundaryParse(tempBuff))
	        return true;
	}
	else
	{
	    size_t potentialSplitSize = std::max(_boundary.length(), _boundaryEnd.length()) - 1;
	    if (searchData.length() > potentialSplitSize)
		{
	        size_t safeLength = searchData.length() - potentialSplitSize;
	        _fileOutput.write(searchData.c_str(), safeLength);
	        carryOver = searchData.substr(safeLength);
	    }
		else
	        carryOver = searchData;
	}
    length -= bytesToRead;
    buff.erase(0, bytesToRead);
	return false;
}

bool	BodyParse::clearBuffers(std::string &data, std::string &accumulatedData, std::string &carryOver)
{
	data.clear();
	accumulatedData.clear();
	carryOver.clear();
	_fileOutput.flush();
	return true;
}

bool BodyParse::ChunkedBoundaryParse(std::string& buff)
{
	static std::string data;
	static size_t length = 0;
	static bool readingChunk = false;
	static std::string accumulatedData;
	static std::string carryOver;

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
			char* trash = NULL;
			length = std::strtol(lengthStr.c_str(), &trash, 16);
			if (*trash || length == SIZE_MAX)
			{
				std::cerr << "Invalid chunk size format!" << std::endl;
				return false;
			}
			if (length == 0)
			{
			    if (!accumulatedData.empty())
				{
					std::string tempBuff = accumulatedData;
					accumulatedData.clear();
					return BoundaryParse(tempBuff);
			    }
			    return length = 0, readingChunk = false, clearBuffers(data, accumulatedData, carryOver);
			}
			readingChunk = true;
		}
		if (writeChunkToFile(buff, length, carryOver, accumulatedData))
			return length = 0, readingChunk = false, clearBuffers(data, accumulatedData, carryOver);
		if (length == 0)
		{
			if (buff.size() >= 2)
				buff.erase(0, 2);
			readingChunk = false;
		}
	}
	return false;
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
				return data = buff.substr(boundaryPos), false;
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
			return buff.clear(), false;
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

bool BodyParse::ContentLengthParse(std::string &buff)
{
	if (_bodySize <= 0)
		return true;
	size_t bytesToProcess = buff.size();
	_fileOutput.write(buff.data(), bytesToProcess);
	_bodySize -= bytesToProcess;
	if (_bodySize == 0)
	{
		_fileOutput.flush();
		return true;
	}
	return false;
}
