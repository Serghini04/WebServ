/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meserghi <meserghi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/30 11:33:58 by meserghi          #+#    #+#             */
/*   Updated: 2025/02/20 16:36:19 by meserghi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include <Server.hpp>
#include <sys/time.h> 

int main(int ac, char **av)
{
	(void)ac;
	std::signal(SIGPIPE, SIG_IGN);
	std::vector<Conserver>	servers = parseConfigFile(av[1]);
	
	if (servers.size())
	{
		Server ser;
		ser.CreateServer(servers);
	}
	return 0;
}
