/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mal-mora <mal-mora@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/30 11:33:58 by meserghi          #+#    #+#             */
/*   Updated: 2025/01/07 10:54:00 by mal-mora         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include <Server.hpp>
# include <RequestParse.hpp>

int main(int ac, char **av)
{
	std::signal(SIGPIPE, SIG_IGN);
    (void)av;
    (void)ac;
    // if(ac != 2)
    //     return 1;
    // std::vector<Conserver>	servers = parseConfigFile(av[1]);
    // std::vector<Server> serverList;
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
    
    Server ser;
    ser.CreateServer();
    // std::cout << servers[0].getAttributes("client_max_body_size")<< std::endl;
	return 0;
}
