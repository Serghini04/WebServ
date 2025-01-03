/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BodyParse.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meserghi <meserghi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/31 13:36:14 by meserghi          #+#    #+#             */
/*   Updated: 2025/01/03 11:02:47 by meserghi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once
# include <map>
# include <iostream>
# include <fstream>
# include <unistd.h>

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
		BodyType	_type;
		size_t		_len;
		std::map<std::string, std::string> _metaData;
	public:
		BodyParse();
		BodyType	getTypeOfBody();
		void		openFileBasedOnContentType();
		void		BoundaryParse(std::string &buff);
		void		ChunkedParse(std::string &buff);
		void		ChunkedBoundaryParse(std::string &buff);
		void		ContentLengthParse(std::string &buff);
		void		setMetaData(std::map<std::string, std::string> &data);
};
