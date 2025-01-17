/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utility.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meserghi <meserghi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/08 18:36:13 by meserghi          #+#    #+#             */
/*   Updated: 2025/01/16 17:39:40 by meserghi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <vector>

class Utility
{
private:
	static bool isNotSpace(int ch)
	{
		return !std::isspace(ch);
	}

public:
	Utility()
	{
	}
	static void ShowErrorExit(std::string const &errorMsg)
	{
		std::cout << errorMsg << std::endl;
		exit(1);
	}
	static std::string GetCurrentTime()
	{
		std::time_t t = std::time(0);
		std::tm *now = std::localtime(&t);
		char buffer[80];

		std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", now);
		return std::string(buffer);
	}
	static void Debug(std::string str)
	{
		std::cout << "----------Debug------------" << "\n";
		std::cout << str << "\n";
		std::cout << "----------Debug------------" << "\n";
		// exit(0);
	}
	static std::string getExtensions(std::string contentType, std::string extension)
	{
		std::vector<std::pair<std::string, std::string> > extensions;

		extensions.push_back(std::make_pair("image/jpeg", ".jpg"));
		extensions.push_back(std::make_pair("image/png", ".png"));
		extensions.push_back(std::make_pair("image/gif", ".gif"));
		extensions.push_back(std::make_pair("image/bmp", ".bmp"));
		extensions.push_back(std::make_pair("image/webp", ".webp"));
		extensions.push_back(std::make_pair("image/tiff", ".tiff"));
		extensions.push_back(std::make_pair("image/x-icon", ".ico"));

		extensions.push_back(std::make_pair("text/plain", ".txt"));
		extensions.push_back(std::make_pair("text/html", ".html"));
		extensions.push_back(std::make_pair("text/css", ".css"));
		extensions.push_back(std::make_pair("text/javascript", ".js"));
		extensions.push_back(std::make_pair("text/markdown", ".md"));
		extensions.push_back(std::make_pair("text/xml", ".xml"));

		extensions.push_back(std::make_pair("application/json", ".json"));
		extensions.push_back(std::make_pair("application/pdf", ".pdf"));
		extensions.push_back(std::make_pair("application/xml", ".xml"));
		extensions.push_back(std::make_pair("application/zip", ".zip"));
		extensions.push_back(std::make_pair("application/x-7z-compressed", ".7z"));
		extensions.push_back(std::make_pair("application/x-tar", ".tar"));
		extensions.push_back(std::make_pair("application/gzip", ".gz"));
		extensions.push_back(std::make_pair("application/x-rar-compressed", ".rar"));

		extensions.push_back(std::make_pair("audio/mpeg", ".mp3"));
		extensions.push_back(std::make_pair("audio/wav", ".wav"));
		extensions.push_back(std::make_pair("audio/ogg", ".ogg"));
		extensions.push_back(std::make_pair("audio/aac", ".aac"));

		extensions.push_back(std::make_pair("video/mp4", ".mp4"));
		extensions.push_back(std::make_pair("video/mpeg", ".mpeg"));
		extensions.push_back(std::make_pair("video/webm", ".webm"));
		extensions.push_back(std::make_pair("video/quicktime", ".mov"));
		extensions.push_back(std::make_pair("video/x-msvideo", ".avi"));
		extensions.push_back(std::make_pair("video/x-matroska", ".mkv"));

		extensions.push_back(std::make_pair("application/octet-stream", ".bin"));
		extensions.push_back(std::make_pair("application/msword", ".doc"));
		extensions.push_back(std::make_pair("application/vnd.openxmlformats-officedocument.wordprocessingml.document", ".docx"));
		extensions.push_back(std::make_pair("application/vnd.ms-excel", ".xls"));
		extensions.push_back(std::make_pair("application/vnd.openxmlformats-officedocument.spreadsheetml.sheet", ".xlsx"));
		extensions.push_back(std::make_pair("application/vnd.ms-powerpoint", ".ppt"));
		extensions.push_back(std::make_pair("application/vnd.openxmlformats-officedocument.presentationml.presentation", ".pptx"));

		if (contentType != "")
		{
			for (std::size_t i = 0; i < extensions.size(); ++i)
			{
				if (extensions[i].first == contentType)
					return extensions[i].second;
			}
			return ".txt";
		}
		else if (extension != "")
		{
			for (std::size_t i = 0; i < extensions.size(); ++i)
			{
				if (extensions[i].second == extension)
					return extensions[i].first;
			}
			return "text/plain";
		}
		return ".txt";
	}
	static std::string trimSpace(std::string line)
	{
		std::string::iterator first = std::find_if(line.begin(), line.end(), isNotSpace);
		if (first == line.end())
			return "";
		std::string::iterator last = std::find_if(line.rbegin(), line.rend(), isNotSpace).base();
		return std::string(first, last);
	}
	static int StrToInt(std::string str)
	{
		int num;

		std::stringstream ss(str);
		ss >> num;
		return num;
	}
	static std::string fileToString(std::ifstream &file)
	{
		if (!file.is_open())
		{
			std::cerr << "Error opening file: " << std::endl;
			return "";
		}
		std::stringstream buffer;
		buffer << file.rdbuf();
		return buffer.str();
	}
};