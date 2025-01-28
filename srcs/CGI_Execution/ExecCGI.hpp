/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execCGI.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hidriouc <hidriouc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/26 11:34:08 by hidriouc          #+#    #+#             */
/*   Updated: 2024/12/26 12:13:05 by hidriouc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConServer.hpp"
#include <unistd.h>
#include <sys/wait.h>


#define BUFFER_SIZE 4096

class	ExecCGI
{
	private:
		int			Inpipe[2];
		int			Outpipe[2];
		pid_t		pid;
		std::string	Response;
	public:
	ExecCGI (const std::string &, const std::string &, const std::string&);
	std::string	GetRespose();
};