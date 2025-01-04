/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mal-mora <mal-mora@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/29 18:23:34 by meserghi          #+#    #+#             */
/*   Updated: 2025/01/04 15:32:50 by mal-mora         ###   ########.fr       */
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

enum	status
{
	eOK,
	eBadRequest
};

class   RequestParse
{
	private :
		std::map<std::string, std::string>  _metaData;
		// i need to change this string to enum? 
		std::string _method;
		std::string	_httpVersion;
		std::string _url;
		BodyParse	_body;
		std::ofstream	_fdOut;
		std::ofstream	_fd;
		// add if request done.
		
		std::string	trimSpace(std::string line);

	public :
		methods		_enumMethod;
		status		_state;
		bool		_requestIsDone;
		std::map<std::string, std::string>	&getMetaData();
		bool getRequestIsDone();
		RequestParse();
		void    readBuffer(std::string buff);
		int    	parseHeader(std::string &header);
		void	parseFirstLine(std::string  header);
		void	parseMetaData(std::string header);
};
