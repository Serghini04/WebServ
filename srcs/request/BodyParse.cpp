/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BodyParse.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meserghi <meserghi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/31 13:38:49 by meserghi          #+#    #+#             */
/*   Updated: 2025/01/10 21:21:50 by meserghi         ###   ########.fr       */
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
		end = buff.find("\"\r\n", start);
		if (end == std::string::npos)
			throw std::runtime_error("Invalid Exec to open file");
		filename = buff.substr(start, end - start);
		std::cout << ">>" << filename << "<<\n"; 
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
	std::cout <<">>" << oss.str() << "<<\n";
	_fileOutput.open(oss.str(), std::ios::binary);
	if (_fileOutput.fail())
		throw std::runtime_error("open failed !");
	_indexFile++;
}

// bool	BodyParse::BoundaryParse(std::string &buff)
// {
// 	static bool			isBoundary = true;
// 	static std::string	data;
// 	size_t				pos;

// 	if (!data.empty())
// 	{
// 		buff = data + buff;
// 		data.clear();
// 	}
// 	while (!buff.empty())
// 	{
// 		pos = buff.find(_boundary + "--");
// 		if (pos != std::string::npos)
// 		{
// 			// _fileOutput << "\n############  end  ###########\n";
// 			_fileOutput << buff.substr(0, pos - 2);
// 			// _fileOutput << "\n#############################\n";
// 			_fileOutput.flush();
// 			buff.clear();
// 			puts("Request Is Done");
// 			return true;
// 		}
// 		if (isBoundary)
// 		{
// 			// _fileOutput << "\n###########  1  ############\n";
// 			size_t	pos = buff.find("\r\n\r\n");
// 			// _fileOutput << "\n#############################\n";
// 			if (pos == std::string::npos)
// 				return data = buff, false;
// 			std::string boundary = buff.substr(0, pos + 4);
// 			openFileOfBoundary(boundary);
// 			buff = buff.substr(pos + 4);
// 			isBoundary = false;
// 		}
// 		pos = buff.find("\r\n");
// 		if (pos == std::string::npos)
// 		{
// 			// _fileOutput << "\n###########  2  ############\n";
// 			_fileOutput << buff;
// 			// _fileOutput << "\n#############################\n";
// 			_fileOutput.flush();
// 			buff.clear();
// 		}
// 		else
// 		{
// 			if (pos + _boundary.length() < buff.size())
// 			{
// 				// _fileOutput << "\n###########  3  ############\n";
// 				_fileOutput << buff.substr(0, pos);
// 				// _fileOutput << "\n#############################\n";
// 				_fileOutput.flush();
// 				data = buff.substr(pos + 2);
// 				buff.clear();		
// 			}
// 			else if (buff.find(_boundary) != std::string::npos)
// 			{
// 				isBoundary = true;
// 				// _fileOutput << "\n###########  4  ############\n";
// 				_fileOutput << buff.substr(0, pos);
// 				// _fileOutput << "\n#############################\n";
// 				_fileOutput.flush();
// 				buff = buff.substr(pos + 2);
// 			}
// 			else
// 			{
// 				// _fileOutput << "\n###########  5  ############\n";
// 				_fileOutput << buff;
// 				// _fileOutput << "\n#############################\n";
// 				_fileOutput.flush();
// 				buff.clear();
// 			}
// 		}
// 	}
// 	return false;
// }

bool	BodyParse::isPartOfBoundary(std::string buff)
{
	// for (size_t i = 0; i < buff.size(); i++)
	// {
	// 	if (_boundary.find(buff[i]) == std::string::npos)
	// 		return false;
	// }
	if (_boundaryEnd.find(buff) != std::string::npos)
		return true;
	return false;	
}

bool	BodyParse::BoundaryParse(std::string &buff)
{
	static std::string	data;
	size_t				pos;

	// std::cout << ">>" << _boundaryEnd << "<<\n"; 
	// std::cout << ">>" << _boundary << "<<\n";
	if (!data.empty())
	{
		buff = data + buff;
		data.clear();
	}
	while (!buff.empty())
	{
		pos = buff.find(_boundaryEnd);
		if (pos != std::string::npos)
		{
			puts("End");
			buff.erase(pos, _boundaryEnd.size());
			_fileOutput << buff;
			_fileOutput.flush();
			return true;
		}
		pos = buff.find(_boundary);
		if (pos != std::string::npos)
		{
			puts("First");
			size_t next_post = buff.find("\r\n\r\n");
			if (next_post == std::string::npos)
				return data = buff, false;
			openFileOfBoundary(buff.substr(pos, next_post));
			buff.erase(pos,  next_post + 4);
		}
		if (isPartOfBoundary(buff.substr(buff.length() > 6 ? buff.length() - 6 : buff.length())))
			return data = buff, false;
		_fileOutput << buff;
		buff.clear();
	}
	return false;
}


// version 1 :
// bool BodyParse::BoundaryParse(std::string &buff) {
//     static std::string data;
//     static bool isFirstBoundary = true;
//     static bool inHeader = false;

//     // Append any remaining data from previous calls
//     if (!data.empty()) {
//         buff = data + buff;
//         data.clear();
//     }

//     while (!buff.empty()) {
//         // Check for final boundary
//         size_t finalBoundaryPos = buff.find(_boundary + "--");
//         if (finalBoundaryPos != std::string::npos) {
//             // Write remaining content before final boundary
//             if (finalBoundaryPos > 0) {
//                 _fileOutput << buff.substr(0, finalBoundaryPos);
//                 _fileOutput.flush();
//             }
//             buff.clear();
//             return true;  // Parsing complete
//         }

//         // Handle first boundary
//         if (isFirstBoundary) {
//             size_t firstBoundaryPos = buff.find(_boundary);
//             if (firstBoundaryPos == std::string::npos) {
//                 data = buff;
//                 buff.clear();
//                 return false;
//             }
//             buff = buff.substr(firstBoundaryPos + _boundary.length());
//             isFirstBoundary = false;
//             inHeader = true;
//             continue;
//         }

//         // Handle part headers
//         if (inHeader) {
//             size_t headerEndPos = buff.find("\r\n\r\n");
//             if (headerEndPos == std::string::npos) {
//                 data = buff;
//                 buff.clear();
//                 return false;
//             }
//             std::string partHeader = buff.substr(0, headerEndPos);
//             openFileOfBoundary(partHeader);  // Process part headers
//             buff = buff.substr(headerEndPos + 4);
//             inHeader = false;
//             continue;
//         }

//         // Look for next boundary
//         size_t nextBoundaryPos = buff.find(_boundary);
        
//         // No boundary found
//         if (nextBoundaryPos == std::string::npos) {
//             // Keep last potential boundary for next iteration
//             size_t lastCRLF = buff.rfind("\r\n");
//             if (lastCRLF != std::string::npos && buff.length() - lastCRLF < _boundary.length() + 4) {
//                 data = buff.substr(lastCRLF);
//                 _fileOutput << buff.substr(0, lastCRLF);
//             } else {
//                 _fileOutput << buff;
//                 data.clear();
//             }
//             _fileOutput.flush();
//             buff.clear();
//             return false;
//         }

//         // Write content up to boundary
//         if (nextBoundaryPos > 0) {
//             _fileOutput << buff.substr(0, nextBoundaryPos - 2);  // -2 to remove \r\n before boundary
//             _fileOutput.flush();
//         }

//         // Move past boundary
//         buff = buff.substr(nextBoundaryPos + _boundary.length());
//         inHeader = true;
//     }

//     return false;
// }

// bool BodyParse::BoundaryParse(std::string &buff) {
//     static std::string data;
//     static bool isFirstBoundary = true;
//     static bool inHeader = false;

//     if (!data.empty()) {
//         buff = data + buff;
//         data.clear();
//     }

//     while (!buff.empty())
// 	{
//         if (isFirstBoundary)
// 		{
//             size_t boundaryPos = buff.find(_boundary);
//             if (boundaryPos == std::string::npos) {
//                 data = buff;
//                 return false;
//             }
//             buff = buff.substr(boundaryPos + _boundary.length());
//             isFirstBoundary = false;
//             inHeader = true;
//             continue;
//         }

//         if (inHeader) {
//             size_t headerEnd = buff.find("\r\n\r\n");
//             if (headerEnd == std::string::npos) {
//                 data = buff;
//                 return false;
//             }
// 			std::string ss = buff.substr(0, headerEnd);
//             openFileOfBoundary(ss);
//             buff = buff.substr(headerEnd + 4);
//             inHeader = false;
//             continue;
//         }

//         size_t boundaryPos = buff.find("\r\n" + _boundary);
//         if (boundaryPos == std::string::npos) {
//             // Check for final boundary
//             size_t finalPos = buff.find("\r\n" + _boundary + "--");
//             if (finalPos != std::string::npos) {
//                 _fileOutput.write(buff.data(), finalPos);
//                 _fileOutput.flush();
// 				puts("gg");
//                 return true;
//             }

//             // Keep last boundary-length bytes in case of partial boundary
//             if (buff.length() > _boundary.length() + 2) {
//                 size_t keepLen = _boundary.length() + 2;
//                 _fileOutput.write(buff.data(), buff.length() - keepLen);
//                 data = buff.substr(buff.length() - keepLen);
//             } else {
//                 data = buff;
//             }
//             _fileOutput.flush();
//             buff.clear();
//             return false;
//         }

//         // Write content up to boundary
//         _fileOutput.write(buff.data(), boundaryPos);
//         _fileOutput.flush();

//         // Move past boundary
//         buff = buff.substr(boundaryPos + 2 + _boundary.length());
//         inHeader = true;
//     }

//     return false;
// }

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
