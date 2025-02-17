/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hidriouc <hidriouc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/30 11:33:58 by meserghi          #+#    #+#             */
/*   Updated: 2025/02/16 16:06:36 by hidriouc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include <Server.hpp>
#include <sys/time.h> 

int main(int ac, char **av)
{
	(void)ac;
	std::signal(SIGPIPE, SIG_IGN);
	std::vector<Conserver>	servers = parseConfigFile(av[1]);

	// std::vector<std::pair<std::string, std::string> >  lis = servers[0].getlistening();
	// for(size_t i = 0; i < lis.size(); i++){
	// 	std::cout << lis[i].first <<":"<<lis[i].second<<std::endl;
	// }
	// std::cerr <<">>" <<servers[0].getLocation("goinfre")["auto_index"] <<std::endl;
	std::cout << servers.size() << "\n";
	if (servers.size())
	{
		Server ser;
		ser.CreateServer(servers);
	}
	return 0;
}
