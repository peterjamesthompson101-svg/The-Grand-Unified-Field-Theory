#include "threatreport.h"
#include <vector>
#include <string>
#include <map>
#include <chrono>
#include <algorithm>
#include <cmath>

// ThreatIntelligence is now a complete class in threatreport.h
// You can create instances and use it like:

/*
Example usage:

ThreatIntelligence threatIntel;

// Update with threat reports
ThreatReport report;
report.engine = "V3_Quarantine";
report.confidence = 0.8;
report.severity = ThreatSeverity::HIGH;
report.signatures = {"PE_Header", "Shellcode_NOP"};
threatIntel.update(report);

// Query threat intelligence
bool hasHighFreq = threatIntel.hasHighFrequencyAttackPatterns();
double avgLevel = threatIntel.getAverageThreatLevel();
bool isRelevant = threatIntel.isEngineRelevant("V3_Quarantine");
double relevance = threatIntel.getEngineRelevance("V3_Quarantine");

// Get detailed report
std::string landscape = threatIntel.getCurrentThreatLandscape();
*/

// Helper functions for threat analysis
namespace ThreatAnalysis {
    
    double calculateEntropy(const std::vector<uint8_t>& data) {
        if (data.empty()) return 0.0;
        
        std::array<int, 256> freq = {0};
        for (uint8_t byte : data) freq[byte]++;
        
        double entropy = 0.0;
        double size = data.size();
        
        for (int count : freq) {
            if (count > 0) {
                double p = count / size;
                entropy -= p * log2(p);
            }
        }
        
        return entropy;
    }
    
    bool containsSuspiciousPatterns(const std::vector<uint8_t>& data) {
        // Check for common malicious patterns
        if (data.size() < 100) return false;
        
        std::vector<std::vector<uint8_t>> suspicious_patterns = {
            {0x4D, 0x5A}, // PE header
            {0x90, 0x90, 0x90, 0x90}, // NOP sled
            {0xE8, 0x00, 0x00, 0x00, 0x00}, // Call instruction
            {0xFF, 0xFF, 0xFF, 0xFF}, // -1 (often used in exploits)
            {0x00, 0x00, 0x00, 0x00} // Null bytes in code
        };
        
        for (const auto& pattern : suspicious_patterns) {
            if (std::search(data.begin(), data.end(), pattern.begin(), pattern.end()) != data.end()) {
                return true;
            }
        }
        
        // Check entropy (encrypted/packed code)
        double entropy = calculateEntropy(data);
        if (entropy > 7.5) return true;
        
        return false;
    }
    
    ThreatSeverity determineSeverity(double confidence, 
                                    const std::vector<std::string>& signatures,
                                    ThreatIntelligence& threatIntel) {
        if (confidence > 0.9) return ThreatSeverity::CRITICAL;
        if (confidence > 0.7) return ThreatSeverity::HIGH;
        if (confidence > 0.5) return ThreatSeverity::MEDIUM;
        
        // Check if patterns match high-frequency attack patterns
        auto highFreqPatterns = threatIntel.getHighFrequencyAttackPatterns();
        for (const auto& sig : signatures) {
            if (std::find(highFreqPatterns.begin(), highFreqPatterns.end(), sig) != highFreqPatterns.end()) {
                return ThreatSeverity::HIGH;
            }
        }
        
        return ThreatSeverity::LOW;
    }
}