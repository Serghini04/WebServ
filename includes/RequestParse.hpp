/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meserghi <meserghi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/29 18:23:34 by meserghi          #+#    #+#             */
/*   Updated: 2025/01/04 11:03:17 by meserghi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once
# include <iostream>
# include <sstream>
# include <string>
# include <BodyParse.hpp>
# include <map>

enum	methods
{
	ePOST,
	eGET,
	eDELETE
};

enum status
{
	eOK = 200,
    eCreated = 201,
	eNotFound = 404,
    eNoContent = 204,
    eFORBIDDEN = 403,
    eBadGateway = 502,
	eBadRequest = 400,
	eGetwayTimeout = 504,
	eRequestTimeout = 408,
	eLengthRequired = 411,
    ePartialContent = 206,
	eNotImplemented = 501,
	eMovedPermanently = 301,
	eMethodNotAllowed = 405,
	eRequestURITooLong = 414,
	eRequestEntityTooLarge = 413,
	// if fail system call :
    eInternalServerError = 500,
	eHTTPVersionNotSupported = 505,
    eRequestedRangeNotSatisfiable = 416
};

class   RequestParse
{
	private :
		std::map<std::string, std::string>  _metaData;
		// i need to change this string to enum? 
		methods			_enumMethod;
		status			_requestStatus;
		std::string 	_method;
		std::string		_httpVersion;
		std::string 	_url;
		BodyParse		_body;
		std::ofstream	_fd;
		// add if request done.
		bool			_requestIsDone;
		
		std::string	trimSpace(std::string line);

	public :
		RequestParse();

		// Get :
		std::map<std::string, std::string>	&getMetaData();

		void    readBuffer(std::string buff);
		int    	parseHeader(std::string &header);
		void	parseFirstLine(std::string  header);
		void	parseMetaData(std::string header);
};
