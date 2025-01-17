/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hidriouc <hidriouc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/30 11:33:58 by meserghi          #+#    #+#             */
/*   Updated: 2025/01/17 11:38:15 by hidriouc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes/Server.hpp"
# include <Server.hpp>
# include <RequestParse.hpp>
#include <cstdlib>

int main(int ac, char **av)
{
	std::signal(SIGPIPE, SIG_IGN);
    (void)av;
    (void)ac;
    if(ac != 2)
        return 1;
	try {
	std::vector<Conserver>	servers = parseConfigFile(av[1]);
	} catch (std::string Err) {
		std::cerr<<Err<<std::endl;
		exit(EXIT_FAILURE);
	}
	
    // for(size_t i = 0; i < servers.size(); i++)
    // {
    //     Server server(servers[i]);
    //     serverList.push_back(server);
    // }
    // for(size_t i = 0; i < serverList.size(); i++)
    // {
    //     int pid = fork();
    //     if(pid == 0)
    //     {
    //         serverList[i].CreateServer();
    //         exit(0);
    //     }
    //     else if(pid < 0)
    //         return 1;
    // }
    // while (true) {
    //     int status;
    //     pid_t pid = wait(&status);
    //     if (pid < 0) 
    //         break;
    // }
    
    // Server ser;
    // ser.CreateServer();
    // std::cout << servers[0].getAttributes("client_max_body_size")<< std::endl;
	return 0;
}
