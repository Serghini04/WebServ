#!/usr/bin/php
<?php
// Define the HTML content
$query_string = getenv("QUERY_STRING") ?: "No query provided";
$html = "<html>
<head><title>CGI Test</title></head>
<body>
<h1>Hello from CGI!</h1>
<p>Hello Mr $query_string </p>
</body>
</html>";

// Calculate content length
$content_length = strlen($html);

// Output raw HTTP response
echo "HTTP/1.1 200 OK\r\n";
echo "Content-Length: $content_length\r\n";
echo "Content-Type: text/html\r\n\r\n";
echo $html;
?>