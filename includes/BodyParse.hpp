/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BodyParse.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meserghi <meserghi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/31 13:36:14 by meserghi          #+#    #+#             */
/*   Updated: 2025/01/28 11:15:50 by meserghi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once
# include <map>
# include <iostream>
# include <fstream>
# include <unistd.h>
# include <sstream>
# include <Utility.hpp>

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

class BodyParse
{
	private:
		BodyType							_type;
		long long							_bodySize;
		long long							_maxBodySize;
		std::ofstream						_fileOutput;
		std::string							_boundary;
		std::string							_boundaryEnd;
		static size_t 						_indexFile;
		std::map<std::string, std::string>	_metaData;
		bool 								_clearData;
	public:
		BodyParse(long long maxBodySize);

		// Set :
		void	setBoundary(std::string boundary);
		void	setBoundaryEnd(std::string boundary);
		void	setbodyType(BodyType type);	
		void	setClearData(bool s);

		// Get :
		BodyType	bodyType();
		long long	sizeRead();
		BodyType	getTypeOfBody(methods method, long long maxBodySize);
		
		// Methods :
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