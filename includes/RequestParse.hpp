/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meserghi <meserghi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/29 18:23:34 by meserghi          #+#    #+#             */
/*   Updated: 2025/01/02 14:54:51 by meserghi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once
# include <iostream>
# include <sstream>
# include <string>
# include <BodyParse.hpp>
# include <map>

class   RequestParse
{
	private :
		std::map<std::string, std::string>  _metaData;
		std::string _method;
		std::string	_httpVersion;
		std::string _url;
		BodyParse	_body;
		std::ofstream	_fd;
		std::ofstream	_fdOut;
		std::string	trimSpace(std::string line);

	public :
		std::map<std::string, std::string>	&getMetaData();

		RequestParse();
		void    readBuffer(std::string buff);
		int    	parseHeader(std::string &header);
		void	parseFirstLine(std::string  header);
		void	parseMetaData(std::string header);
};
