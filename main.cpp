/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hidriouc <hidriouc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/30 11:33:58 by meserghi          #+#    #+#             */
/*   Updated: 2025/03/05 11:35:25 by hidriouc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include <Server.hpp>
#include <sys/time.h> 

int main(int ac, char **av)
{
	(void)ac;
	std::signal(SIGPIPE, SIG_IGN);
	std::vector<Conserver>	servers = parseConfigFile(av[1]);
	
	// std::vector<std::pair<std::string, std::string> > lss = servers[0].getlistening();
	// for (size_t i = 0; i < lss.size() ; i++)
	// {
	// 	std::cout <<"|"<<lss[i].first <<":"<<lss[i].second<<"|"<<std::endl;
	// }
	if (servers.size())
	{
		Server ser;
		ser.CreateServer(servers);
	}
	return 0;
}
