#include <iostream>
#include <fstream>
#include <string>

std::string readFileBytes(std::ifstream &file, std::size_t bytesToRead = 1028)
{
    std::string buffer(bytesToRead, '\0'); // Create a string with the desired size
    file.read(&buffer[0], bytesToRead);    // Read up to `bytesToRead` bytes into the buffer

    // Resize the buffer if fewer bytes were read (e.g., end of file reached)
    buffer.resize(file.gcount());

    return buffer;
}

int main()
{
    const std::string filePath = "outpucct.txt"; // Replace with your file path

    // Open the file in binary mode
    std::ifstream inputFile(filePath, std::ios::binary);
    if (!inputFile)
    {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return 1;
    }

    std::ofstream file("testing");
    if (!file)
    {
        std::cerr << "Failed to open output file: testing" << std::endl;
        return 1;
    }

    while (true)
    {
        try
        {
            std::string fileContent = readFileBytes(inputFile); // Read the next chunk
            if (fileContent.empty()) // Check if we reached the end of the file
                break;

            file << "File content (up to 1028 bytes):\n"
                 << fileContent << std::endl;
        }
        catch (const std::ios_base::failure &e)
        {
            std::cerr << "Error: " << e.what() << std::endl;
            break;
        }
    }

    return 0;
}
