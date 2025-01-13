/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BodyParse.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meserghi <meserghi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/31 13:36:14 by meserghi          #+#    #+#             */
/*   Updated: 2025/01/04 15:59:19 by meserghi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once
# include <map>
# include <iostream>
# include <fstream>
# include <unistd.h>
# include <sstream>

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
		size_t								_sizeRead;
		std::ofstream						_fileOutput;
		std::map<std::string, std::string>	_metaData;
	public:
		BodyParse();
		BodyType	getTypeOfBody();
		void		openFileBasedOnContentType();
		bool		BoundaryParse(std::string &buff);
		bool		ChunkedParse(std::string &buff);
		bool		ChunkedBoundaryParse(std::string &buff);
		bool		ContentLengthParse(std::string &buff);
		void		setMetaData(std::map<std::string, std::string> &data);
};
