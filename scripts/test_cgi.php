<?php
// Generate a simple HTML page
header("Content-Type: text/html; charset=UTF-8");

echo "<!DOCTYPE html>";
echo "<html lang='en'>";
echo "<head>";
echo "<meta charset='UTF-8'>";
echo "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
echo "<title>CGI PHP Test</title>";
echo "</head>";
echo "<body>";
echo "<h1>Welcome to the CGI PHP Test</h1>";
echo "<p>This is a dynamically generated HTML page.</p>";

echo "<h2>Request Data</h2>";
echo "<pre>";

// Display CGI environment variables
echo "--- CGI Environment Variables ---\n";
foreach ($_SERVER as $key => $value) {
    echo htmlspecialchars("$key: $value\n");
}

echo "\n--- GET Parameters ---\n";
foreach ($_GET as $key => $value) {
    echo htmlspecialchars("$key: $value\n");
}

echo "\n--- POST Data ---\n";
$postData = file_get_contents("php://input");
echo htmlspecialchars($postData . "\n");

echo "</pre>";
echo "</body>";
echo "</html>";
?>
