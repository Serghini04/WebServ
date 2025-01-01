/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BodyParse.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meserghi <meserghi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/31 13:38:49 by meserghi          #+#    #+#             */
/*   Updated: 2024/12/31 18:31:12 by meserghi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include <BodyParse.hpp>

BodyParse::BodyParse()
{
    _type = eNone;
}

BodyType	BodyParse::getTypeOfBody(std::map<std::string, std::string> &data)
{
    (void)data;
    // puts("+++++++++++++++++++++++++++\n");
    // std::cout << data["Transfer-Encoding"] << "\n";
    // puts("+++++++++++++++++++++++++++\n");
    if (data["Transfer-Encoding"] == "chunked" && data[""] == "")
        return eChunkedBoundary;
    return eNone;
}