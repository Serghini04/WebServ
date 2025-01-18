/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meserghi <meserghi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/30 11:33:58 by meserghi          #+#    #+#             */
/*   Updated: 2025/01/17 15:21:47 by meserghi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include <Server.hpp>
# include <RequestParse.hpp>

int main(int ac, char **av)
{
	try
	{
		if(ac != 2)
			throw std::runtime_error("Invalid Arg.");
		Server ser;
		std::signal(SIGPIPE, SIG_IGN);
		std::vector<Conserver>	servers = parseConfigFile(av[1]);
		ser.CreateServer(servers);
	}
	catch(...)
	{
		std::cerr <<">> FUCk OFF! <<" << '\n';
	}
	return 0;
}
