/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BodyParse.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meserghi <meserghi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/31 13:36:14 by meserghi          #+#    #+#             */
/*   Updated: 2025/01/14 18:27:30 by meserghi         ###   ########.fr       */
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
		size_t								_bodySize;
		std::ofstream						_fileOutput;
		std::string							_boundary;
		std::string							_boundaryEnd;
		size_t 								_indexFile;
		std::map<std::string, std::string>	_metaData;
	public:
		BodyParse();

		// Set
		void	setBoundary(std::string boundary);
		void	setBoundaryEnd(std::string boundary);
		void	setbodyType(BodyType type);	

		// Get
		BodyType	bodyType();
		size_t		sizeRead();

		BodyType	getTypeOfBody();
		void		openFileBasedOnContentType();
		bool		BoundaryParse(std::string &buff);
		bool		ChunkedParse(std::string &buff);
		bool		ChunkedBoundaryParse(std::string &buff);
		bool		ContentLengthParse(std::string &buff);
		void		openFileOfBoundary(std::string buff);
		void		setMetaData(std::map<std::string, std::string> &data);
};
