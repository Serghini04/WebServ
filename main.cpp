/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meserghi <meserghi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/30 11:33:58 by meserghi          #+#    #+#             */
/*   Updated: 2025/02/01 15:47:03 by meserghi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include <Server.hpp>

int main(int ac, char **av)
{
	(void)ac;
	std::signal(SIGPIPE, SIG_IGN);
	std::vector<Conserver>	servers = parseConfigFile(av[1]);
	// std::vector<std::pair<std::string, std::string> >  lis = servers[0].getlistening();
	// for(size_t i = 0; i < lis.size(); i++){
	// 	std::cout << lis[i].first <<":"<<lis[i].second<<std::endl;
	// }
	std::cerr <<">>" <<servers[0].getLocation("goinfre")["auto_index"] <<std::endl;
	if (servers.size()){
	Server ser;
	ser.CreateServer(servers);}
	return 0;
}
