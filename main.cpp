/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mal-mora <mal-mora@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/30 11:33:58 by meserghi          #+#    #+#             */
/*   Updated: 2025/01/03 17:34:13 by mal-mora         ###   ########.fr       */
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
