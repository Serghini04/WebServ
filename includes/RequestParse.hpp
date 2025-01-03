/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meserghi <meserghi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/29 18:23:34 by meserghi          #+#    #+#             */
/*   Updated: 2025/01/03 15:30:16 by meserghi         ###   ########.fr       */
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
		std::map<std::string, std::string>	&getMetaData();

		RequestParse();
		void    readBuffer(std::string buff);
		int    	parseHeader(std::string &header);
		void	parseFirstLine(std::string  header);
		void	parseMetaData(std::string header);
};
