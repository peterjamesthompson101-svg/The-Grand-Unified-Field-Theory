#include <vector>
#include <algorithm>
#include <cmath>
#include <cstdint>

double performGHzAnalysis(const std::vector<uint8_t>& data) {
    if (data.size() < 512) return 0.1;
    
    // Look for patterns indicating GHz frequency signals
    // Common GHz signal patterns in digital systems often show:
    // - Rapid transitions (0/1 alternation)
    // - Specific periodic sequences
    // - High frequency clock patterns
    
    double ghz_score = 0.0;
    
    // Check 1: High frequency transitions (many 0/1 changes)
    int transitions = 0;
    for (size_t i = 1; i < data.size(); ++i) {
        // Count bit transitions between consecutive bytes
        uint8_t xor_result = data[i] ^ data[i-1];
        while (xor_result) {
            transitions += (xor_result & 1);
            xor_result >>= 1;
        }
    }
    
    // Normalize transition count (empirical threshold)
    double transition_ratio = static_cast<double>(transitions) / (data.size() * 8);
    if (transition_ratio > 0.4) {  // High transition rate suggests high frequency
        ghz_score += 0.3;
    }
    
    // Check 2: Look for common GHz clock patterns
    // Example: 01010101 (0x55) or 10101010 (0xAA) repeating
    std::vector<uint8_t> clock_patterns = {0x55, 0xAA, 0x33, 0xCC};  // Common clock signals
    int pattern_matches = 0;
    
    for (const auto& pattern : clock_patterns) {
        int count = 0;
        for (size_t i = 0; i < data.size(); ++i) {
            if (data[i] == pattern) {
                count++;
            }
        }
        double pattern_ratio = static_cast<double>(count) / data.size();
        if (pattern_ratio > 0.2) {  // Significant presence of clock pattern
            pattern_matches++;
        }
    }
    
    if (pattern_matches >= 2) {
        ghz_score += 0.3;
    }
    
    // Check 3: Periodicity detection using autocorrelation
    const int max_lag = 16;  // Check for short-term periodicity
    int periodic_score = 0;
    
    for (int lag = 1; lag <= max_lag; ++lag) {
        int matches = 0;
        int comparisons = 0;
        
        for (size_t i = 0; i + lag < data.size() && i < 100; ++i) {  // Sample first 100 bytes
            if (data[i] == data[i + lag]) {
                matches++;
            }
            comparisons++;
        }
        
        if (comparisons > 0) {
            double correlation = static_cast<double>(matches) / comparisons;
            if (correlation > 0.7) {  // Strong correlation at this lag
                periodic_score++;
            }
        }
    }
    
    if (periodic_score > 0) {
        ghz_score += 0.2;
    }
    
    // Check 4: High frequency preamble patterns (common in RF/GHz communications)
    std::vector<uint8_t> preamble_pattern = {0xAA, 0xAA, 0xAA, 0xAA};  // Common preamble
    int preamble_matches = 0;
    
    for (size_t i = 0; i <= data.size() - preamble_pattern.size(); ++i) {
        bool match = true;
        for (size_t j = 0; j < preamble_pattern.size(); ++j) {
            if (data[i + j] != preamble_pattern[j]) {
                match = false;
                break;
            }
        }
        if (match) {
            preamble_matches++;
            i += preamble_pattern.size() - 1;  // Skip ahead
        }
    }
    
    if (preamble_matches > 0) {
        ghz_score += 0.2;
    }
    
    // Ensure score is between 0.0 and 1.0
    return std::max(0.0, std::min(1.0, ghz_score));
}