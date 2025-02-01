/* ************************************************************************** */
/*	                                                                        */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hidriouc <hidriouc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/30 11:33:58 by meserghi          #+#    #+#             */
/*   Updated: 2025/01/21 10:18:43 by hidriouc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes/Server.hpp"
# include <Server.hpp>
# include <RequestParse.hpp>
#include <cstdlib>
#include <vector>

int main(int ac, char **av)
{
	(void)ac;
	std::signal(SIGPIPE, SIG_IGN);
	std::vector<Conserver>	servers = parseConfigFile(av[1]);
	std::cout << servers.size() << std::endl;
	for (size_t i = 0; i < servers.size(); i++)
	{
		std::vector<std::pair<std::string, std::string> > listening = servers[i].getlistening();
		std::cout <<i + 1<<"----------------------------------------------------------"<<std::endl;
		for (size_t j = 0; j < listening.size(); j++){
			std::cout << listening[j].first<<":"<<listening[j].second <<std::endl;
		}

	}

	if (servers.size()){
	Server ser;
	ser.CreateServer(servers);}
	return 0;
}
