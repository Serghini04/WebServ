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
	std::vector<std::string> gg = servers[0].getphats();
	for (std::vector<std::string>::iterator it= gg.begin(); it != gg.end(); it++)
	{
		std::cout << *it << std::endl; 
	}
	if (servers.size()){
	Server ser;
	ser.CreateServer(servers);}
	return 0;
}
