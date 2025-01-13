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
    (void)ac;
    (void)av;
	std::signal(SIGPIPE, SIG_IGN);
    std::vector<Conserver>	servers = parseConfigFile(av[1]);
    std::vector<Server> serverList;
    for(size_t i = 0; i < servers.size(); i++)
    {
        Server server(servers[i]);
        serverList.push_back(server);

    }
    // serverList[0].CreateServer();
    
    // std::cout << servers[0].getAttributes("client_max_body_size")<< std::endl;
	return 0;
}
