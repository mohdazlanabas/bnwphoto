#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <ctime>

// ✅ Check if file exists
bool fileExists(const std::string& filename) {
    std::ifstream file(filename);
    return file.good();
}

// ✅ Generate output filename
std::string generateOutputFileName(const std::string& inputFile) {
    size_t dotPos = inputFile.find_last_of(".");
    if (dotPos == std::string::npos) {
        return inputFile + "_bw"; // No extension? Just append
    }
    return inputFile.substr(0, dotPos) + "_bw" + inputFile.substr(dotPos);
}

// ✅ Get file size in KB
double getFileSizeKB(const std::string& filename) {
    struct stat stat_buf;
    if (stat(filename.c_str(), &stat_buf) == 0) {
        return stat_buf.st_size / 1024.0; // in KB
    }
    return -1;
}

// ✅ Get file modification time
std::string getFileTime(const std::string& filename) {
    struct stat attr;
    if (stat(filename.c_str(), &attr) == 0) {
        return std::ctime(&attr.st_mtime); // modification time
    }
    return "Unavailable";
}

// ✅ Manual EXIF GPS reader (basic placeholder)
std::string getGPSData(const std::string& filename) {
    // For simplicity, placeholder logic
    return "Location: Unavailable";
}

// ✅ Add text overlay + subtle "<azlanio>" watermark
void addTextOverlay(cv::Mat& image, const std::vector<std::string>& lines) {
    int fontFace = cv::FONT_HERSHEY_SIMPLEX;
    double fontScale = 0.6; // smaller text
    int thickness = 1;
    int lineHeight = 20;

    // Draw overlay text lines
    for (size_t i = 0; i < lines.size(); ++i) {
        cv::putText(image, lines[i], cv::Point(10, 30 + i * lineHeight),
                    fontFace, fontScale, cv::Scalar(255, 255, 255), thickness, cv::LINE_AA);
    }

    // ✅ Add subtle "<azlanio>" watermark
    std::string watermark = "<azlanio>";
    double watermarkFontScale = 0.5; // smaller font
    int watermarkThickness = 1;      // thin
    cv::Scalar watermarkColor(200, 200, 200); // light gray

    cv::Size textSize = cv::getTextSize(watermark, fontFace, watermarkFontScale, watermarkThickness, nullptr);
    cv::Point watermarkPos(image.cols - textSize.width - 10, image.rows - 10);

    cv::putText(image, watermark, watermarkPos, fontFace, watermarkFontScale, watermarkColor, watermarkThickness, cv::LINE_AA);
}

int main() {
    std::string inputFile;

    // Ask user for input filename
    std::cout << "Enter input image filename (e.g., photo.jpg): ";
    std::getline(std::cin, inputFile);

    if (!fileExists(inputFile)) {
        std::cerr << "Error: File does not exist - " << inputFile << std::endl;
        return 1;
    }

    // Load original image
    cv::Mat colorImage = cv::imread(inputFile);
    if (colorImage.empty()) {
        std::cerr << "Error: Could not load image " << inputFile << std::endl;
        return 1;
    }

    // Get original properties
    int width = colorImage.cols;
    int height = colorImage.rows;
    double sizeKB = getFileSizeKB(inputFile);
    std::string modTime = getFileTime(inputFile);
    std::string gpsData = getGPSData(inputFile);

    // Convert to black & white
    cv::Mat bwImage;
    cv::cvtColor(colorImage, bwImage, cv::COLOR_BGR2GRAY);

    // Auto-generate output filename
    std::string outputFile = generateOutputFileName(inputFile);

    // Prepare overlay text
    std::vector<std::string> overlayLines = {
        "Resolution: " + std::to_string(width) + "x" + std::to_string(height),
        "Size: " + std::to_string(static_cast<int>(sizeKB)) + " KB",
        "Modified: " + modTime.substr(0, modTime.size() - 1),
        gpsData
    };

    // Convert grayscale to BGR for colored text
    cv::cvtColor(bwImage, bwImage, cv::COLOR_GRAY2BGR);

    // Add overlay
    addTextOverlay(bwImage, overlayLines);

    // Save the B&W image
    if (cv::imwrite(outputFile, bwImage)) {
        std::cout << "✅ Black & White image with overlay saved as: " << outputFile << std::endl;
    } else {
        std::cerr << "Error: Could not save image " << outputFile << std::endl;
        return 1;
    }

    return 0;
}
