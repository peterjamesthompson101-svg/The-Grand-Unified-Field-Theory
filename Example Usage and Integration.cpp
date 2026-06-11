// ====================== EXAMPLE USAGE ======================
// main.cpp - Example of how to use the complete V13 engine

#include "V13_1GHz_EnhancedEngine_Optimized.h"
#include <fstream>
#include <iostream>

int main() {
    std::cout << "=== V13 1GHz Enhanced Engine Test ===\n\n";
    
    // Create engine instance
    V13_1GHz_EnhancedEngine_Optimized engine;
    
    // Test 1: Analyze synthetic data
    std::vector<uint8_t> test_data(4096);
    std::generate(test_data.begin(), test_data.end(), std::rand);
    
    std::cout << "Test 1: Analyzing synthetic data (" << test_data.size() << " bytes)\n";
    ThreatReport report1 = engine.analyzeDeep(test_data);
    std::cout << "  Confidence: " << report1.confidence << "\n";
    std::cout << "  Severity: " << report1.severityToString() << "\n";
    std::cout << "  Processing Time: " << report1.processing_time << "s\n";
    std::cout << "  Signatures: " << report1.signatures.size() << " found\n\n";
    
    // Test 2: Analyze file
    std::cout << "Test 2: Try analyzing a file (enter path or press Enter for synthetic):\n";
    std::string filepath;
    std::getline(std::cin, filepath);
    
    if (!filepath.empty()) {
        std::ifstream file(filepath, std::ios::binary | std::ios::ate);
        if (file) {
            size_t file_size = file.tellg();
            file.seekg(0, std::ios::beg);
            
            std::vector<uint8_t> file_data(file_size);
            file.read(reinterpret_cast<char*>(file_data.data()), file_size);
            
            std::cout << "  File size: " << file_size << " bytes\n";
            
            ThreatReport report2 = engine.analyzeDeep(file_data);
            std::cout << "  Confidence: " << report2.confidence << "\n";
            std::cout << "  Severity: " << report2.severityToString() << "\n";
            std::cout << "  Processing Time: " << report2.processing_time << "s\n";
            
            // Show top 5 signatures
            std::cout << "  Top 5 signatures:\n";
            for (size_t i = 0; i < std::min(size_t(5), report2.signatures.size()); ++i) {
                std::cout << "    - " << report2.signatures[i] << "\n";
            }
        } else {
            std::cout << "  Error: Could not open file.\n";
        }
    }
    
    // Show engine stats
    std::cout << "\nEngine Statistics:\n";
    std::cout << "  Memory Usage: " << engine.getMemoryUsageMB() << " MB\n";
    std::cout << "  Available Buffers: " << engine.getAvailableBuffers() << "\n";
    
    return 0;
}