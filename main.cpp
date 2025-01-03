/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meserghi <meserghi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/30 11:33:58 by meserghi          #+#    #+#             */
/*   Updated: 2025/01/03 09:59:48 by meserghi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include <Server.hpp>
# include <RequestParse.hpp>

int main()
{
	std::signal(SIGPIPE, SIG_IGN);
    Server server;
    server.CreateServer();
	return 0;
}
