/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utility.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meserghi <meserghi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/08 18:36:13 by meserghi          #+#    #+#             */
/*   Updated: 2025/03/04 21:21:41 by meserghi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include <vector>
# include <map>
# include <sys/stat.h>
# include <sys/stat.h>
# include <dirent.h>
#include <ctime>
#include <sys/time.h> 

class Utility
{
	private:
		static bool isNotSpace(int ch)
		{
			return !std::isspace(ch);
		}

	public:
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
		static std::string GetCurrentT()
{
				struct timeval tv;
    gettimeofday(&tv, NULL);

    std::time_t t = tv.tv_sec;
    std::tm *now = std::localtime(&t);
    
    char buffer[80];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", now);

    std::ostringstream oss;
    oss << buffer << "." << (tv.tv_usec / 1000); // Convert microseconds to milliseconds

    return oss.str();
}

		static void Debug(std::string str)
		{
			std::cout << "----------Debug------------" << "\n";
			std::cout << str << "\n";
			std::cout << "----------Debug------------" << "\n";
		}

		static std::string toUpperCase(const std::string& input) {
			std::string result = input;
			for (size_t i = 0; i < result.size(); ++i)
				if (std::isalpha(result[i]))
					result[i] = std::toupper(result[i]);
			return result;
		}

		static bool	compareByLength(const std::string &a, const std::string &b)
		{
			return a.size() > b.size();
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

		static std::string percentEncoding(std::string key)
		{
			std::map<std::string, std::string> encoding;

			encoding["%3A"] = ":";
			encoding["%2F"] = "/";
			encoding["%3F"] = "?";
			encoding["%23"] = "#";
			encoding["%5B"] = "[";
			encoding["%5D"] = "]";
			encoding["%40"] = "@";
			encoding["%21"] = "!";
			encoding["%24"] = "$";
			encoding["%26"] = "&";
			encoding["%27"] = "'";
			encoding["%28"] = "(";
			encoding["%29"] = ")";
			encoding["%2A"] = "*";
			encoding["%2B"] = "+";
			encoding["%2C"] = ",";
			encoding["%3B"] = ";";
			encoding["%3D"] = "=";
			encoding["%25"] = "%";
			encoding["%20"] = " ";
			encoding["+"]   = " ";
			if (encoding[key] == "")
				throw std::string("400 Bad Request");
			return encoding[key];
		}

		static std::string trimSpace(std::string line)
		{
			std::string::iterator first = std::find_if(line.begin(), line.end(), isNotSpace);
			if (first == line.end())
				return "";
			std::string::iterator last = std::find_if(line.rbegin(), line.rend(), isNotSpace).base();
			return std::string(first, last);
		}

		static std::string trimJstSpace(std::string line)
		{
			size_t first = line.find_first_not_of(" \r");
			if (first == std::string::npos)
				return "";
			size_t last = line.find_last_not_of(" \r");
			return line.substr(first, last);
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

		static std::string toLowerCase(const std::string& input) {
			std::string result = input;
			for (std::size_t i = 0; i < result.size(); ++i) {
				result[i] = std::tolower(static_cast<unsigned char>(result[i]));
			}
			return result;
		}

		static std::string ToStr(long long nb)
		{
			std::stringstream ss;
			ss << nb;
			return ss.str();
		}

		static int stringToInt(const std::string& str)
		{
			std::istringstream ss(str);
			int num = 0;
			ss >> num;
			return num;
		}

		static bool	checkIfPathExists(const std::string& path)
		{
			struct stat statBuff;
			if (stat(path.c_str(), &statBuff) < 0)
			{
				if (errno == EACCES)
					throw std::string("404 Not Found");
				return false;
			}
			return true;
		}

		static bool	isDirectory(const std::string& path)
		{
			struct stat statBuff;
			if (!stat(path.c_str(), &statBuff) && S_ISDIR(statBuff.st_mode))
				return true;
			return false;
		}

		static bool	isReadableFile(const std::string& path)
		{
			struct stat statBuff;
			if (!stat(path.c_str(), &statBuff) && (statBuff.st_mode & S_IRUSR))
				return true;
			return false;
		}

		static void	deleteFolderContent(const std::string& path)
		{
			DIR* currentDir = opendir(path.c_str());
			dirent* dp;
			std::string targetFile;
			if (!isReadableFile(path))
			{
				std::cerr << path ;
				throw std::string("403 Forbidden");
			}
			if (!currentDir)
				return;
			while ((dp = readdir(currentDir)))
			{
				targetFile = path + "/" + dp->d_name;
				if (stringEndsWith(targetFile, ".") || stringEndsWith(targetFile, ".."))
					continue ;
				if (!isDirectory(targetFile))
					std::remove(targetFile.c_str());
				else
					deleteFolderContent(targetFile);
			}
			closedir(currentDir);
		}

		static bool stringEndsWith(const std::string& str, const std::string& suffix)
		{
			if (suffix.size() > str.size())
				return false;
			return !str.compare(str.size() - suffix.size(), suffix.size(), suffix);
		}
};
