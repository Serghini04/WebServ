/* ************************************************************************** */
/*			                                                */
/*                                                        :::      ::::::::   */
/*   ExecCGI.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hidriouc <hidriouc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/26 11:35:11 by hidriouc          #+#    #+#             */
/*   Updated: 2024/12/26 11:47:26 by hidriouc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ExecCGI.hpp"
#include <cstddef>
#include <cstdlib>

ExecCGI::ExecCGI (const std::string &cgiPath, const std::string &filePath, const std::string &body)
{
 if (pipe(Inpipe) == -1 || pipe(Outpipe) == -1) {
	perror("pipe failed");
	}

	if ((pid = fork()) == -1){
	perror("fork failed");
	}
	if (pid == 0) {
	close(Inpipe[1]);
	close(Outpipe[0]);

	dup2(Inpipe[0], STDIN_FILENO);
	dup2(Outpipe[1], STDOUT_FILENO);

	close(Inpipe[0]);
	close(Outpipe[1]);

	char envRequestMethod[] = "REQUEST_METHOD=POST";
	char envContentLength[32];
	snprintf(envContentLength, sizeof(envContentLength), "CONTENT_LENGTH=%lu", body.size());
	char envPathInfo[256];
	snprintf(envPathInfo, sizeof(envPathInfo), "PATH_INFO=%s", filePath.c_str());
	char envScriptName[256];
	snprintf(envScriptName, sizeof(envScriptName), "SCRIPT_NAME=%s", filePath.c_str());

	char *envp[] = {
	envRequestMethod,
	envContentLength,
	envPathInfo,
	envScriptName,
	NULL
	};
	char *args[] = {
	const_cast<char *>(cgiPath.c_str()),
	const_cast<char *>(filePath.c_str()),
	NULL
	};
	execve(cgiPath.c_str(), args, envp);
	perror("execve failed");
	exit(1);
	} else if (pid > 0) {
	close(Inpipe[0]);
	close(Outpipe[1]);

	write(Inpipe[1], body.c_str(), body.size());
	close(Inpipe[1]);

	waitpid(pid, NULL, 0);
	char buffer[4096];
	std::string response;
	ssize_t bytesRead;
	while ((bytesRead = read(Outpipe[0], buffer, sizeof(buffer))) > 0) {
	response.append(buffer, bytesRead);
	}
	close(Outpipe[0]);

	Response=response;
	} else {
		perror("fork failed");
	}
}

std::string	ExecCGI::GetRespose()
{
	return Response;
}
