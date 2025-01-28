/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meserghi <meserghi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/29 18:23:34 by meserghi          #+#    #+#             */
/*   Updated: 2025/01/28 14:27:05 by meserghi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once
# include <iostream>
# include <sstream>
# include <string>
# include <BodyParse.hpp>
# include <map>

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
	eContentTooLarge = 413,
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
		std::string		_uri;
		BodyParse		_body;
		bool			_requestIsDone;
		std::ofstream	_fd;
		bool			_isHeader;
		Conserver		&_configServer;
		long long		_maxBodySize;
		std::string		_statusCodeMessage;
		std::string		_location;

	public :
		RequestParse(Conserver &conserver);
	
		// Get :
		std::string	location();
		std::map<std::string, std::string>	&getMetaData();
		status		statusCode();
		std::string	statusCodeMessage();
		bool		requestIsDone();
		bool		isHeader();
		std::string	URL();
		methods		method();

		// set :
		void		setUrl(std::string s);
		void		setIsHeader(bool isHeader);
		void		SetStatusCode(status s);
		void		SetStatusCodeMsg(std::string message);
		void		SetRequestIsDone(bool s);

		// Methods :
		void		checkURL();
		void 		deleteMethod();
		std::string matchingURL();
		void		checkAllowedMethod();
		bool		parseHeader(std::string &header, std::string &buff);
		void		readBuffer(std::string buff);
		bool		parseHeader(std::string &header);
		void		parseFirstLine(std::string  header);
		void		parseMetaData(std::string header);
		void		deleteURI();
		
		~RequestParse();
};