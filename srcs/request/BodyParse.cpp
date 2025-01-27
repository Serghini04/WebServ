/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BodyParse.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meserghi <meserghi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/31 13:38:49 by meserghi          #+#    #+#             */
/*   Updated: 2025/01/27 15:42:19 by meserghi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include <BodyParse.hpp>

size_t	BodyParse::_indexFile = 1;

BodyParse::BodyParse(long long maxBodySize)
{
	_maxBodySize = maxBodySize;
    _type = eNone;
	_bodySize = 0;
	_boundary = "";
}

void		BodyParse::setMetaData(std::map<std::string, std::string> &data)
{
	_metaData = data;
}

bool	BodyParse::parseBody(std::string &buff)
{
	switch (bodyType())
	{
		case eChunked :
			return ChunkedParse(buff);
		case eBoundary :
			return BoundaryParse(buff);
		case eChunkedBoundary :
			return ChunkedBoundaryParse(buff);
		case eContentLength :
			return ContentLengthParse(buff);
		case eNone :
			throw std::runtime_error("501 Not Implemented");
	}
	return false;
}

BodyType	BodyParse::getTypeOfBody(methods method, long long maxBodySize)
{
	char	*trash = NULL;

	if (_metaData["content-length"] != "")
	{
		_bodySize = strtoll(_metaData["content-length"].c_str(), &trash, 10);
		if (trash == _metaData["content-length"].c_str() || *trash != '\0' || errno == ERANGE || _bodySize < 0)
			throw std::runtime_error("400 Bad Request 1");
		std::cout << "_bodySize =>>" << _bodySize << "<<" <<std::endl;
		std::cout << "_Max =>>" << maxBodySize << "<<" <<std::endl;
		if (maxBodySize >= 0 && _bodySize > maxBodySize)
			throw std::runtime_error("413 Content Too Large");
	}
    if (_metaData["transfer-encoding"] == "chunked" && _metaData["content-type"].find("multipart/form-data; boundary=") != std::string::npos)
		return eChunkedBoundary;
	else if (_metaData["transfer-encoding"] == "chunked")
	{
		if (method == ePOST && _metaData["content-type"] == "")
			throw std::runtime_error("400 Bad Request");
		return eChunked;
	}
	else if (_metaData["content-type"].find("multipart/form-data; boundary=") != std::string::npos)
		return eBoundary;
	else if (_metaData["content-length"] != "")
	{
		if (method == ePOST && _metaData["content-type"] == "")
			throw std::runtime_error("400 Bad Request");
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
	oss << "/Users/meserghi/goinfre/www/Output" << _indexFile;
	std::string namefile = oss.str();
	namefile += Utility::getExtensions(_metaData["content-type"], "");
	_fileOutput.open(namefile.c_str(), std::ios::binary);
	if (_fileOutput.fail())
		throw std::runtime_error("open failed !");
	std::cout <<"Create File :>>" << namefile << "<<\n";
	_indexFile++;
}

bool	BodyParse::writeChunkToFile(std::string &buff, size_t &length, std::string &carryOver, std::string &accumulatedData, bool &clearData)
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
			checkContentTooLarge(safeLength, clearData);
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

bool	BodyParse::clearBuffers(std::string &data, std::string &accumulatedData, std::string &carryOver, bool &readingChunk)
{
	data.clear();
	accumulatedData.clear();
	carryOver.clear();
	_fileOutput.flush();
	readingChunk = false;
	// throw std::runtime_error("201 Created");
	return true;
}

bool BodyParse::ChunkedBoundaryParse(std::string& buff)
{
	static std::string data;
	static size_t length = 0;
	static bool readingChunk = false;
	static std::string accumulatedData;
	static std::string carryOver;
	static bool clearData = true;

	if (clearData)
		clearBuffers(data, accumulatedData, carryOver, readingChunk), clearData = false;
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
			if (*trash)
			{
				std::cout <<  lengthStr << "\n";
				std::cerr << "Invalid chunk size format!" << std::endl;
				return false;
			}
			if (length == 0)
			{
				if (!accumulatedData.empty())
				{
					std::string tempBuff = accumulatedData;
					accumulatedData.clear();
					if (BoundaryParse(tempBuff))
						return clearBuffers(data, accumulatedData, carryOver, readingChunk);
					return false;
			    }
			    return length = 0, clearBuffers(data, accumulatedData, carryOver, readingChunk);
			}
			readingChunk = true;
		}
		if (writeChunkToFile(buff, length, carryOver, accumulatedData, clearData))
			return length = 0, clearBuffers(data, accumulatedData, carryOver, readingChunk);
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
			oss << "/Users/meserghi/goinfre/www/Output" << _indexFile << filename.substr(start);
		else
			oss << "/Users/meserghi/goinfre/www/Output" << _indexFile << ".txt";
	}
	else if (buff.find("Content-Type: ") != std::string::npos)
	{
		target = "Content-Type: ";
		start = buff.find(target) + target.length();
		end = buff.find("\r\n", start);
		if (end == std::string::npos)
			throw std::runtime_error("Invalid Exec to open file");
		filename =  Utility::trimSpace(buff.substr(start, end - start));
		_metaData["content-type"] = filename;
		openFileBasedOnContentType();
		return ;
	}
	else
		oss << "/Users/meserghi/goinfre/www/Output" << _indexFile << ".txt";
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
	static bool	clearData = true;
	size_t processed = 0;

	if (clearData)
	{
		data.clear();
		clearData = false;	
	}
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
			// IF "DATA...BOUNDARY..."
			if (boundaryPos > processed)
			{
				checkContentTooLarge(boundaryPos - processed - 2, clearData);
				_fileOutput.write(buff.data() + processed, boundaryPos - processed - 2);
			}
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
			checkContentTooLarge(boundaryEndPos - processed, clearData);
			_fileOutput.write(buff.data() + processed, boundaryEndPos - processed);
			_fileOutput.close();
			buff.erase(0, boundaryEndPos + _boundaryEnd.size() + 2);
		}
		if (processed < buff.size() && _boundaryEnd.find(buff.back()) == std::string::npos)
		{
			checkContentTooLarge(buff.size() - processed, clearData),
			_fileOutput.write(buff.data() + processed, buff.size() - processed);
			return buff.clear(), false;
		}
		data = buff.substr(processed);
		buff.clear();
	}
    return false;
}

void	BodyParse::checkContentTooLarge(size_t length, bool &clearData)
{
	if (_metaData["content-length"] != "")
	{
		_bodySize -= length;
		if (_maxBodySize >= 0 && _bodySize < 0)
		{
			clearData = true;
			throw std::runtime_error("413 Content Too Large1");
		}
	}
	else
	{
		_bodySize += length;
		std::cout << "Body size = " << _bodySize << std::endl;
		std::cout << "Max body size = " << _maxBodySize << std::endl;
		if (_maxBodySize >= 0 && _bodySize > _maxBodySize)
		{
			clearData = true;
			throw std::runtime_error("413 Content Too Large2");
		}
	}	
}

bool BodyParse::ChunkedParse(std::string &buff)
{
	static std::string data;
	static size_t length = 0;
	static bool readingChunk = false;
	static bool	clearData = true;

	if (clearData)
	{
		data.clear();
		length = 0;
		readingChunk = false;
		clearData = false;
	}
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
			if (*trash)
			{
				data.clear();
				throw std::runtime_error("404 Bad Request");
			}
			checkContentTooLarge(length, clearData);
			if (length == 0)
			{
				_fileOutput.close();
				throw std::runtime_error("201 Created");
			}
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
	if (_bodySize <= 0)
	{
		_fileOutput.flush();
		return true;
	}
	return false;
}

BodyParse::~BodyParse()
{
	_fileOutput.close();
}
