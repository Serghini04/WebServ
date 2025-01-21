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

int main(int ac, char **av)
{
	std::signal(SIGPIPE, SIG_IGN);
	if(ac != 2)
		return 1;
	std::vector<Conserver>	servers = parseConfigFile(av[1]);
	std::cerr<<">> " <<servers.size()<<std::endl;
	if (servers.size()){
	Server ser;
	ser.CreateServer(servers);}
	return 0;
}
