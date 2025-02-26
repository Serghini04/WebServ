#include <Response.hpp>

std::string replacePlaceholders(const std::string input, const std::string &placeholder, const std::string &replacement)
{

    std::string result = input;
    size_t pos = 0;
    while ((pos = result.find(placeholder, pos)) != std::string::npos)
    {
        result.replace(pos, placeholder.length(), replacement);
        pos += replacement.length();
    }
    return result;
}

std::string getFileHtml()
{

	const std::string errHtml = "<!DOCTYPE html>\n"
									"<html>\n"
									"<head>\n"
									"    <title>$MSG</title>\n"
									"    <style>\n"
									"        body {\n"
									"            margin: 0;\n"
									"            height: 100vh;\n"
									"            display: flex;\n"
									"            justify-content: center;\n"
									"            align-items: center;\n"
									"            font-family: Arial, sans-serif;\n"
									"            background-color: #f5f5f5;\n"
									"        }\n"
									"        .error-box {\n"
									"            background: white;\n"
									"            padding: 40px;\n"
									"            border-radius: 8px;\n"
									"            box-shadow: 0 2px 15px rgba(0,0,0,0.1);\n"
									"            text-align: center;\n"
									"            max-width: 80%;\n"
									"        }\n"
									"        h1 {\n"
									"            color: #e74c3c;\n"
									"            font-size: 48px;\n"
									"            margin: 0 0 20px 0;\n"
									"        }\n"
									"        p {\n"
									"            color: #666;\n"
									"            font-size: 18px;\n"
									"            line-height: 1.6;\n"
									"            margin: 0;\n"
									"        }\n"
									"    </style>\n"
									"</head>\n"
									"<body>\n"
									"    <div class=\"error-box\">\n"
									"        <h1>$MSG</h1>\n"
									"        <p>Sorry, something went wrong.<br>Please try again later.</p>\n"
									"    </div>\n"
									"</body>\n"
									"</html>";
    return errHtml;
}