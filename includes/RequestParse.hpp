/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meserghi <meserghi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/29 18:23:34 by meserghi          #+#    #+#             */
/*   Updated: 2025/01/15 10:59:32 by meserghi         ###   ########.fr       */
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
	// if fail system call or not find file to open
    eInternalServerError = 500,
	eHTTPVersionNotSupported = 505,
    eRequestedRangeNotSatisfiable = 416
};

class   RequestParse
{
	private :
		std::map<std::string, std::string>  _metaData;
		methods			_enumMethod;
		status			_statusCode;
		std::string 	_method;
		std::string		_httpVersion;
		std::string 	_url;
		BodyParse		_body;
		bool			_requestIsDone;
		std::ofstream	_fd;
		bool			_isHeader;
		std::string		_statusCodeMessage;
	public :
		RequestParse();

		// Get :
		std::map<std::string, std::string>	&getMetaData();
		status		statusCode();
		bool		requestIsDone();
		bool		isHeader();
		std::string	URL();
		std::string	statusCodeMessage();
		methods		method();
		void	setUrl(std::string s);
		bool	isConnectionClosed();
		// set :
		void	SetisHeader(bool isHeader);
		void	SetStatusCode(status s);
		void	SetStatusCodeMsg(std::string s);
		void	SetRequestIsDone(bool s);

		bool	readHeader(std::string &buff);
		void	readBuffer(std::string buff);
		bool	parseHeader(std::string &header);
		void	parseFirstLine(std::string  header);
		void	parseMetaData(std::string header);
};