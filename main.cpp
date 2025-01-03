/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meserghi <meserghi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/30 11:33:58 by meserghi          #+#    #+#             */
/*   Updated: 2024/12/30 18:34:26 by meserghi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Server.hpp>
#include <RequestParse.hpp>
#include <csignal>
int main()
{
	// try
	// {
	// 	RequestParse    request;
	// 	std::string test = "GET / HTTP/1.1         \r\ngreerterter:    sss\r\n\r\n";
	// 	request.readBuffer(test);
	// }
	// catch (const std::exception &e)
	// {
	// 	std::cerr << e.what() << "\n";
	// }
	
	std::signal(SIGPIPE, SIG_IGN);
    Server server;
    server.CreateServer();
	return 0;
}