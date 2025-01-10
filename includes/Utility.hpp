/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utility.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meserghi <meserghi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/08 18:36:13 by meserghi          #+#    #+#             */
/*   Updated: 2025/01/09 13:54:20 by meserghi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

class Utility
{
    private :
		static bool	isNotSpace(int ch)
		{
			return !std::isspace(ch);
		}
    public :
        Utility()
        {
        }
        static void ShowErrorExit(std::string const &errorMsg)
        {
            std::cout << errorMsg << std::endl;
            exit(1);
        }
        static std::string GetCurrentTime()
        {
            std::time_t t = std::time(0); 
            std::tm* now = std::localtime(&t);
            char buffer[80];

            std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", now);
            return std::string(buffer);
        }

		static std::string	trimSpace(std::string line)
		{
			std::string::iterator first = std::find_if(line.begin(), line.end(), isNotSpace);
			if (first == line.end())
				return "";
			std::string::iterator last = std::find_if(line.rbegin(), line.rend(), isNotSpace).base();
			return std::string(first, last);
		}
};
