/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BodyParse.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meserghi <meserghi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/31 13:38:49 by meserghi          #+#    #+#             */
/*   Updated: 2025/01/02 16:07:39 by meserghi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include <BodyParse.hpp>

// for (std::map<std::string, std::string>::iterator it = data.begin(); it != data.end(); it++)
// {
//     std::cout << "\n#######################\n";
//     std::cout << it->first << "\n";
//     std::cout << it->second << "\n";
//     std::cout << "\n#######################\n";
// }

BodyParse::BodyParse()
{
    _type = eNone;
	_len = 0;
}

    // chunked && boundray ==>
    // "Content-Type" = "multipart/form-data" && "Transfer-Encoding" == "chunked"
    // chunked ==>
    //  "Transfer-Encoding" == "chunked"
    // boundray ==>
    // "Content-Type" == "multipart/form-data" && "Transfer-Encoding" != "chunked"
    //  Content-Length ==>
    // Content-Length || !Content-Length

BodyType	BodyParse::getTypeOfBody(std::map<std::string, std::string> &data)
{
    if (data["Transfer-Encoding"] == "chunked" && data["Content-Type"].find("multipart/form-data") != std::string::npos)
		return eChunkedBoundary;
	else if (data["Transfer-Encoding"] == "chunked")
		return eChunked;
	else if (data["Content-Type"].find("multipart/form-data") != std::string::npos)
		return eBoundary;
	return eContentLength;
}

void	BodyParse::ChunkedBoundaryParse(std::string &buff)
{
	(void)buff;
}

void	BodyParse::BoundaryParse(std::string &buff)
{
	(void)buff;

}

void	BodyParse::ChunkedParse(std::string &buff)
{
	(void)buff;	
}

void	BodyParse::ContentLengthParse(std::string &buff)
{
	(void)buff;
	puts("ffff");
}
