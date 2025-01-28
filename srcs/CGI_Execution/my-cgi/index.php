<?php
// Check if the request method is POST
if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    // Retrieve the submitted data
    $data = $_POST['data'] ?? 'No data provided';

    // Output the response in plain text
    header('Content-Type: text/plain');
    echo "Submission Received:\n\n";
    echo "You submitted:\n";
    echo $data;
} else {
    // Respond with an error for non-POST requests
    header('Content-Type: text/plain', true, 405); // 405 Method Not Allowed
    echo "Error: Only POST requests are allowed.";
}
?>
