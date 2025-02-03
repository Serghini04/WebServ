/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BodyParse.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meserghi <meserghi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/31 13:36:14 by meserghi          #+#    #+#             */
/*   Updated: 2025/02/01 16:34:01 by meserghi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once
# include <map>
# include <iostream>
# include <fstream>
# include <unistd.h>
# include <sstream>
# include <Utility.hpp>
# include <ConServer.hpp>

class RequestParse;

enum	methods
{
	ePOST,
	eGET,
	eDELETE
};

enum	BodyType
{
	eChunked,
	eBoundary,
	eChunkedBoundary,
	eContentLength,
	eNone
};

// _bodyFile.open(_body., std::ios::binary | std::ios::app);
// if (_bodyFile.fail())
// 	throw std::string("500 Internal Server Error");

class BodyParse
{
	private:
		BodyType							_type;
		long long							_bodySize;
		long long							_maxBodySize;
		std::ofstream						_fileOutput;
		std::string							_fileName;
		std::string							_boundary;
		std::string							_boundaryEnd;
		static size_t 						_indexFile;
		bool 								_clearData;
		bool								_isCGI;
		std::ofstream						_fileTrash;
		std::string							_bodyFileName;				
		std::map<std::string, std::string>	_metaData;
		
	public:
		BodyParse(long long maxBodySize);

		// Set :
		void	setBodyFileName(std::string name);
		void	setIsCGI(bool s);
		void	setBoundary(std::string boundary);
		void	setBoundaryEnd(std::string boundary);
		void	setbodyType(BodyType type);	
		void	setClearData(bool s);
		void	setFileName(std::string fileName);

		// Get :
		bool		isCGI();
		BodyType	bodyType();
		long long	sizeRead();
		std::string	BodyFileName();
		BodyType	getTypeOfBody(methods method, long long maxBodySize);
		
		// Methods :
		void		handleCGI(std::string &buff);
		void		clearChunkedAttributes(std::string &data, size_t &length, bool &readingChunk, std::string &buff);
		bool		parseBody(std::string &buff);
		bool		clearBuffers(std::string &data, std::string &accumulatedData, std::string &carryOver, bool &readingChunk);
		void		checkContentTooLarge(size_t length);
		void		openFileBasedOnContentType();
		bool		BoundaryParse(std::string &buff);
		bool		ChunkedParse(std::string &buff);
		bool		ChunkedBoundaryParse(std::string &buff);
		bool		ContentLengthParse(std::string &buff);
		void		openFileOfBoundary(std::string buff);
		bool		writeChunkToFile(std::string &buff, size_t &length, std::string &carryOver, std::string &accumulatedData);
		void		setMetaData(std::map<std::string, std::string> &data);

		~BodyParse();
};