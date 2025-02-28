/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BodyParse.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meserghi <meserghi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/31 13:38:49 by meserghi          #+#    #+#             */
/*   Updated: 2025/02/20 17:12:38 by meserghi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include <BodyParse.hpp>

size_t	BodyParse::_indexFile = 1;

void	BodyParse::setFileName(std::string fileName)
{
	_fileName = fileName;
}

void	BodyParse::setIsCGI(bool s)
{
	_isCGI = s;
}

void	BodyParse::setBodyFileName(std::string name)
{
	_bodyFileName = name;
}

bool	BodyParse::isCGI()
{
	return _isCGI;
}

std::string	BodyParse::BodyFileName()
{
	return _bodyFileName;
}

BodyParse::BodyParse(long long maxBodySize)
{
	_maxBodySize = maxBodySize;
	_bodyFileName = "/tmp/Output.trash";
	_isCGI = false;
    _type = eNone;
	_bodySize = 0;
	_boundary = "";
	_boundaryEnd = "";
	_clearData = true;
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
			throw std::string("501 Not Implemented");
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
			throw std::string("400 Bad Request");
		if (maxBodySize >= 0 && _bodySize > maxBodySize)
			throw std::string("413 Content Too Large");
	}
    if (_metaData["transfer-encoding"] == "chunked" && _metaData["content-type"].find("multipart/form-data; boundary=") != std::string::npos)
		return eChunkedBoundary;
	else if (_metaData["transfer-encoding"] == "chunked")
	{
		if (method == ePOST && _metaData["content-type"] == "")
			throw std::string("400 Bad Request");
		return eChunked;
	}
	else if (_metaData["content-type"].find("multipart/form-data; boundary=") != std::string::npos)
		return eBoundary;
	else if (_metaData["content-length"] != "")
	{
		if (method == ePOST && _metaData["content-type"] == "")
			throw std::string("400 Bad Request");
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
	oss << _fileName << _indexFile;
	std::string namefile = oss.str();
	namefile += Utility::getExtensions(_metaData["content-type"], "");
	_fileOutput.open(namefile.c_str(), std::ios::binary);
	if (_fileOutput.fail())
		throw std::string("500 Internal Server Error");
	if (_isCGI)
		_bodyFileName = namefile;
	else
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
			checkContentTooLarge(safeLength);
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
	return true;
}

void	BodyParse::setClearData(bool s)
{
	_clearData = s;
}

bool BodyParse::ChunkedBoundaryParse(std::string& buff)
{
	static std::string data;
	static size_t length = 0;
	static bool readingChunk = false;
	static std::string accumulatedData;
	static std::string carryOver;

	if (_isCGI)
		handleCGI(buff);
	if (_clearData)
		clearBuffers(data, accumulatedData, carryOver, readingChunk), _clearData = false;
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
					return BoundaryParse(tempBuff);
			    }
			    throw std::string("201 Created");
			}
			readingChunk = true;
		}
		if (writeChunkToFile(buff, length, carryOver, accumulatedData))
			throw std::string("201 Created");
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
			throw std::string("404 Bad Request");
		filename = buff.substr(start, end - start);
		start = filename.rfind(".");
		if (start != std::string::npos)
			oss << _fileName << _indexFile << filename.substr(start);
		else
			oss << _fileName << _indexFile << ".txt";
	}
	else if (buff.find("Content-Type: ") != std::string::npos)
	{
		target = "Content-Type: ";
		start = buff.find(target) + target.length();
		end = buff.find("\r\n", start);
		if (end == std::string::npos)
			throw std::string("404 Bad Request");
		filename =  Utility::trimSpace(buff.substr(start, end - start));
		_metaData["content-type"] = filename;
		openFileBasedOnContentType();
		return ;
	}
	else
		oss << _fileName << _indexFile << ".txt";
	// std::cout <<"Create File :>>" << oss.str() << "<<\n";
	_fileOutput.open(oss.str(), std::ios::binary);
	if (_fileOutput.fail())
		throw std::string("500 Internal Server Error");
	_indexFile++;
}

void	BodyParse::handleCGI(std::string &buff)
{
	if (!_fileTrash.is_open())
	{
		_fileTrash.open(_bodyFileName, std::ios::binary);
		if (_fileTrash.fail())
			throw std::string("500 Internal Server Error");
	}
	checkContentTooLarge(buff.size());
	_fileTrash << buff;
	_fileTrash.flush();
}

bool BodyParse::BoundaryParse(std::string& buff)
{
	static std::string data;
	static bool	clearData = true;
	size_t processed = 0;

	if (_isCGI)
		handleCGI(buff);
	if (clearData)
		data.clear(), clearData = false;
	if (!data.empty())
		buff = data + buff, data.clear();
	while (processed < buff.size())
	{
		size_t boundaryPos = buff.find(_boundary, processed);
		if (boundaryPos != std::string::npos)
		{
			// IF "DATA...BOUNDARY..."
			if (boundaryPos > processed)
			{
				checkContentTooLarge(boundaryPos - processed - 2);
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
			checkContentTooLarge(boundaryEndPos - processed);
			_fileOutput.write(buff.data() + processed, boundaryEndPos - processed);
			buff.erase(0, boundaryEndPos + _boundaryEnd.size() + 2);
			return _fileOutput.close(), true;
		}
		if (processed < buff.size() && _boundaryEnd.find(buff.back()) == std::string::npos)
		{
			checkContentTooLarge(buff.size() - processed),
			_fileOutput.write(buff.data() + processed, buff.size() - processed);
			return buff.clear(), false;
		}
		data = buff.substr(processed);
		buff.clear();
	}
    return false;
}

void	BodyParse::checkContentTooLarge(size_t length)
{
	if (_metaData["content-length"] != "")
	{
		_bodySize -= length;
		if (_maxBodySize >= 0 && _bodySize < 0)
			throw std::string("413 Content Too Large");
	}
	else
	{
		_bodySize += length;
		std::cout << "Body size = " << _bodySize << std::endl;
		std::cout << "Max body size = " << _maxBodySize << std::endl;
		if (_maxBodySize >= 0 && _bodySize > _maxBodySize)
			throw std::string("413 Content Too Large");
	}	
}

void	BodyParse::clearChunkedAttributes(std::string &data, size_t &length, bool &readingChunk, std::string &buff)
{
	if (_clearData)
	{
		data.clear();
		length = 0;
		readingChunk = false;
		_clearData = false;
	}
	if (!data.empty())
	{
		buff = data + buff;
		data.clear();
	}
}

bool BodyParse::ChunkedParse(std::string &buff)
{
	static std::string data;
	static size_t length = 0;
	static bool readingChunk = false;

	clearChunkedAttributes(data, length, readingChunk, buff);
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
				throw std::string("404 Bad Request");
			checkContentTooLarge(length);
			if (length == 0)
				_fileOutput.close(), throw std::string("201 Created");
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
		_fileOutput.close();
		throw std::string("201 Created");
	}
	return false;
}

BodyParse::~BodyParse()
{
	_fileTrash.flush();
	_fileOutput.close();
	_fileTrash.close();
}