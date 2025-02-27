/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hidriouc <hidriouc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/29 18:23:34 by meserghi          #+#    #+#             */
/*   Updated: 2025/02/27 16:24:32 by hidriouc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once
# include <iostream>
# include <sstream>
# include <string>
# include <BodyParse.hpp>
# include <map>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

# define CGI_TIMEOUT 10
# define SIZE_BUFFER 50

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
		std::string 	_method;
		std::string		_httpVersion;
		std::string 	_url;
		methods			_enumMethod;
		std::ofstream	_fileDebug;
		status			_statusCode;
		std::string		_uri;
		BodyParse		_body;
		bool			_requestIsDone;
		bool			_isHeader;
		Conserver		&_configServer;
		long long		_maxBodySize;
		std::string		_statusCodeMessage;
		std::string		_location;
		std::string		header;
		std::string		_queryString;
		std::string		_fragment;
		std::string		_outfile;
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
		bool		isCGI();
		methods		method();

		// set :
		void		setIsCGI(bool s);
		void		setUrl(std::string s);
		void		setIsHeader(bool isHeader);
		void		SetStatusCode(status s);
		void		SetStatusCodeMsg(std::string message);
		void		SetRequestIsDone(bool s);
		bool		isConnectionClosed();

		// Methods :
		void		checkCGI();
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

		// Execution of CGI by hidriouc
		int		runcgiscripte();
		bool	is_InvalideURL();
		bool	CheckStdERR(const char* fileERR);
		void	_dupfd(int infd, int outfd, int ERRfile);
		void	clear(int bodyfd, int outfd, int fileERR);
		void	_validateContentLength(const std::string& contentLength, size_t bodysize);
		void	_validateContentType(const std::string& contentType);
		void	_parseHeaderLine(const std::string& line, std::string lines[]);
		int		_parseHeaders(size_t bodysize, const std::string& headers);
		int		_forkAndExecute(int infd, int outfd, char* env[], int ERRfile);
		int		_waitForCGIProcess(int pid);
		int		parseCGIOutput();

		std::string 	getCGIfile();
		std::vector<std::string>	_buildEnvVars();
		void						_openFileSafely(std::ifstream& file, const std::string& filename);
		std::string					_extractHeaderValue(const std::string& line);

		~RequestParse();
};