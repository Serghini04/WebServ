/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BodyParse.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meserghi <meserghi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/31 13:36:14 by meserghi          #+#    #+#             */
/*   Updated: 2024/12/31 17:20:09 by meserghi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once
# include <map>
# include <iostream>

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
	public:
		BodyParse();
		BodyType	getTypeOfBody(std::map<std::string, std::string> &data);
};

