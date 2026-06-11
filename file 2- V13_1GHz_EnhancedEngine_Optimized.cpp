// ====================== V13_1GHZ_ENHANCED_ENGINE_OPTIMIZED ======================
// Complete implementation file

#include "V13_1GHz_EnhancedEngine_Optimized.h"
#include <algorithm>
#include <numeric>
#include <execution>
#include <bitset>
#include <sstream>
#include <iomanip>
#include <fstream>

// Constructor
V13_1GHz_EnhancedEngine_Optimized::V13_1GHz_EnhancedEngine_Optimized() 
    : memory_pool(16, 4),  // 16 blocks of 4KB
      config() {
    
    // Initialize neural network
    ghz_nn = std::make_unique<OptimizedGHzNeuralNetwork>();
    
    std::cout << "[V13] 1GHz Enhanced Engine Optimized Initialized\n";
    std::cout << "  Memory Pool: " << memory_pool.getTotalCount() 
              << " blocks, " << memory_pool.getMemoryUsageMB() << " MB\n";
    std::cout << "  GHz Neural Network: " 
              << (ghz_nn ? "Initialized" : "Disabled") << "\n";
}

// Destructor
V13_1GHz_EnhancedEngine_Optimized::~V13_1GHz_EnhancedEngine_Optimized() {
    std::cout << "[V13] 1GHz Enhanced Engine Optimized Shutting Down\n";
}

// ====================== MAIN DEEP ANALYSIS FUNCTION ======================

ThreatReport V13_1GHz_EnhancedEngine_Optimized::analyzeDeep(const std::vector<uint8_t>& data) {
    ThreatReport report;
    report.engine = "V13_1GHz_Deep";
    auto start_time = std::chrono::high_resolution_clock::now();
    
    // Check if data is too small for deep analysis
    if (data.size() < 256) {
        report.confidence = 0.1;
        report.severity = ThreatSeverity::LOW;
        report.signatures.push_back("DATA_TOO_SMALL_FOR_DEEP_ANALYSIS");
        report.processing_time = std::chrono::duration<double>(
            std::chrono::high_resolution_clock::now() - start_time).count();
        return report;
    }
    
    try {
        // Step 1: Determine optimal sampling rate for GHz analysis
        double sampling_rate = determineOptimalSamplingRate(data);
        report.signatures.push_back("SAMPLING_RATE_" + std::to_string(static_cast<int>(sampling_rate * 1000)) + "MHz");
        
        // Step 2: Acquire pattern buffer from memory pool for efficient processing
        PatternBuffer pattern_buffer = memory_pool.acquirePatternBuffer();
        
        // Step 3: Extract GHz features for analysis (COMPLETED)
        auto ghz_features = extractGHzFeatures(data);
        
        // Step 4: Perform neural network inference
        double neural_score = 0.0;
        if (ghz_nn) {
            neural_score = ghz_nn->predictFromGHzFeatures(ghz_features, true); // Use cache
            if (neural_score > 0.7) {
                report.signatures.push_back("GHZ_NEURAL_NETWORK_HIGH_THREAT");
            } else if (neural_score > 0.4) {
                report.signatures.push_back("GHZ_NEURAL_NETWORK_SUSPICIOUS");
            }
        }
        
        // Step 5: Perform GHz microarchitecture analysis
        double microarch_score = performGHzMicroarchitectureAnalysis(data, sampling_rate);
        
        // Step 6: Cache timing attack detection
        double cache_score = analyzeCacheTimingPatterns(data);
        
        // Step 7: Branch prediction vulnerability analysis
        double branch_score = analyzeBranchPrediction(data);
        
        // Step 8: Rowhammer attack detection
        double rowhammer_score = detectRowhammerPatterns(data);
        
        // Step 9: Quantum harmonic analysis
        double quantum_score = performQuantumHarmonicAnalysis(data, sampling_rate);
        
        // Step 10: Apply GHz processing to pattern buffer
        pattern_buffer.applyGHzFilter(sampling_rate);
        
        // Step 11: Combine all analysis results
        report.confidence = calculateDeepAnalysisConfidence(
            neural_score, microarch_score, cache_score, 
            branch_score, rowhammer_score, quantum_score
        );
        
        // Step 12: Determine severity
        report.severity = determineThreatSeverity(report.confidence);
        
        // Step 13: Add GHz-specific signatures
        if (microarch_score > 0.7) {
            report.signatures.push_back("GHZ_MICROARCHITECTURE_THREAT");
        }
        if (cache_score > 0.6) {
            report.signatures.push_back("CACHE_TIMING_ATTACK_DETECTED");
        }
        if (branch_score > 0.5) {
            report.signatures.push_back("BRANCH_PREDICTION_ATTACK");
        }
        if (rowhammer_score > 0.4) {
            report.signatures.push_back("ROWHAMMER_PATTERN_DETECTED");
        }
        if (quantum_score > 0.6) {
            report.signatures.push_back("QUANTUM_HARMONIC_THREAT");
        }
        
        report.signatures.push_back("V13_DEEP_ANALYSIS_COMPLETE");
        report.signatures.push_back("OPTIMIZED_ENGINE_V8");
        
        // Step 14: Release pattern buffer back to pool
        memory_pool.releasePatternBuffer(pattern_buffer);
        
    } catch (const std::exception& e) {
        std::cerr << "[V13] Deep analysis error: " << e.what() << std::endl;
        report.confidence = 0.3;
        report.severity = ThreatSeverity::MEDIUM;
        report.signatures.push_back("DEEP_ANALYSIS_ERROR");
        report.signatures.push_back("FALLBACK_ANALYSIS_USED");
    }
    
    // Calculate processing time
    auto end_time = std::chrono::high_resolution_clock::now();
    report.processing_time = std::chrono::duration<double>(end_time - start_time).count();
    
    return report;
}

// ====================== COMPLETED MISSING FUNCTIONS ======================

double V13_1GHz_EnhancedEngine_Optimized::calculateEntropy(const std::vector<uint8_t>& data) {
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

double V13_1GHz_EnhancedEngine_Optimized::estimateCompressionRatio(const std::vector<uint8_t>& data) {
    if (data.size() < 100) return 0.5;
    
    // Simple compression ratio estimation
    std::array<int, 256> freq = {0};
    for (uint8_t byte : data) freq[byte]++;
    
    int unique_bytes = 0;
    for (int count : freq) {
        if (count > 0) unique_bytes++;
    }
    
    return static_cast<double>(unique_bytes) / 256.0;
}

std::vector<std::vector<double>> V13_1GHz_EnhancedEngine_Optimized::extractGHzFeatures(const std::vector<uint8_t>& data) {
    std::vector<std::vector<double>> features;
    
    if (data.empty()) return features;
    
    // Extract features in chunks
    size_t chunk_size = 1024;
    for (size_t i = 0; i < data.size(); i += chunk_size) {
        size_t end = std::min(i + chunk_size, data.size());
        std::vector<uint8_t> chunk(data.begin() + i, data.begin() + end);
        
        std::vector<double> chunk_features;
        
        // Basic statistical features
        chunk_features.push_back(calculateEntropy(chunk));
        chunk_features.push_back(calculateMean(chunk));
        
        double mean = chunk_features.back();
        chunk_features.push_back(calculateVariance(chunk, mean));
        
        // GHz-specific features (using provided implementations)
        chunk_features.push_back(detectGHzPatterns(chunk));
        chunk_features.push_back(analyzeGHzFrequency(chunk));
        
        features.push_back(chunk_features);
    }
    
    // If no chunks were created (data smaller than chunk_size), process entire data
    if (features.empty() && !data.empty()) {
        std::vector<double> chunk_features;
        chunk_features.push_back(calculateEntropy(data));
        chunk_features.push_back(calculateMean(data));
        chunk_features.push_back(calculateVariance(data, chunk_features.back()));
        chunk_features.push_back(detectGHzPatterns(data));
        chunk_features.push_back(analyzeGHzFrequency(data));
        features.push_back(chunk_features);
    }
    
    return features;
}

double V13_1GHz_EnhancedEngine_Optimized::detectGHzPatterns(const std::vector<uint8_t>& chunk) {
    if (chunk.size() < 8) return 0.0;
    
    int ghz_pattern_matches = 0;
    
    // Fixed patterns to look for (from provided file)
    std::vector<std::vector<uint8_t>> ghz_patterns = {
        {0x47, 0x48, 0x5A},  // "GHZ" uppercase
        {0x47, 0x48, 0x7A},  // "GHz" lowercase z
        {0x67, 0x68, 0x7A},  // "ghz" all lowercase
        {0x31, 0x47, 0x48, 0x7A},  // "1GHz" prefix
        {0x32, 0x47, 0x48, 0x7A},  // "2GHz" prefix
        {0x33, 0x47, 0x48, 0x7A},  // "3GHz" prefix
        {0x31, 0x30, 0x30, 0x30, 0x4D, 0x48, 0x7A},  // "1000MHz" variant
        {0x31, 0x30, 0x30, 0x30, 0x47, 0x48, 0x7A}   // "1000GHz" variant
    };
    
    // Check each pattern
    for (const auto& pattern : ghz_patterns) {
        if (chunk.size() < pattern.size()) continue;
        
        for (size_t i = 0; i < chunk.size() - pattern.size(); ++i) {
            bool match = true;
            for (size_t j = 0; j < pattern.size(); ++j) {
                if (chunk[i + j] != pattern[j]) {
                    match = false;
                    break;
                }
            }
            if (match) {
                ghz_pattern_matches++;
                i += pattern.size() - 1;  // Skip ahead to avoid overlapping matches
            }
        }
    }
    
    // Also look for numeric GHz patterns (e.g., frequency references)
    int numeric_ghz_matches = 0;
    for (size_t i = 0; i < chunk.size() - 3; ++i) {
        // Look for patterns like "2.4G", "5.0G", etc.
        if (std::isdigit(chunk[i]) && chunk[i+1] == 0x2E &&  // digit followed by '.'
            std::isdigit(chunk[i+2]) && chunk[i+3] == 0x47) { // digit followed by 'G'
            numeric_ghz_matches++;
        }
    }
    
    // Add frequency-based analysis for GHz patterns
    double freq_analysis_score = 0.0;
    if (chunk.size() >= 32) {
        // Check for high-frequency byte patterns that might indicate GHz references
        int high_freq_sequences = 0;
        for (size_t i = 0; i < chunk.size() - 4; i += 4) {
            // Look for byte sequences that might represent GHz values
            uint32_t val = (chunk[i] << 24) | (chunk[i+1] << 16) | 
                          (chunk[i+2] << 8) | chunk[i+3];
            
            // Common GHz frequency values in MHz converted to hex patterns
            if (val >= 0x3B9ACA00 && val <= 0x77359400) { // 1GHz-2GHz range in Hz
                high_freq_sequences++;
            }
        }
        freq_analysis_score = std::min(1.0, high_freq_sequences * 0.2);
    }
    
    int total_matches = ghz_pattern_matches + numeric_ghz_matches;
    
    // Return score based on matches, with diminishing returns
    double base_score = std::min(1.0, total_matches * 0.25);
    
    // Combine with frequency analysis
    return std::min(1.0, base_score * 0.7 + freq_analysis_score * 0.3);
}

double V13_1GHz_EnhancedEngine_Optimized::analyzeGHzFrequency(const std::vector<uint8_t>& chunk) {
    if (chunk.size() < 32) return 0.0;
    
    // Analyze frequency content using FFT-like approach
    double freq_score = 0.0;
    
    // Simple frequency analysis: count rapid transitions
    int rapid_changes = 0;
    for (size_t i = 0; i < chunk.size() - 1; ++i) {
        int diff = std::abs(static_cast<int>(chunk[i]) - static_cast<int>(chunk[i + 1]));
        if (diff > 128) { // Rapid change indicates high frequency
            rapid_changes++;
        }
    }
    
    double change_ratio = static_cast<double>(rapid_changes) / chunk.size();
    freq_score = std::min(1.0, change_ratio * 2.0);
    
    return freq_score;
}

double V13_1GHz_EnhancedEngine_Optimized::calculateMean(const std::vector<uint8_t>& data) {
    if (data.empty()) return 0.0;
    
    double sum = 0.0;
    for (uint8_t byte : data) sum += byte;
    
    return sum / data.size();
}

double V13_1GHz_EnhancedEngine_Optimized::calculateVariance(const std::vector<uint8_t>& data, double mean) {
    if (data.size() < 2) return 0.0;
    
    if (mean < 0) {
        mean = calculateMean(data);
    }
    
    double sum_sq = 0.0;
    for (uint8_t byte : data) {
        double diff = byte - mean;
        sum_sq += diff * diff;
    }
    
    return sum_sq / data.size();
}

double V13_1GHz_EnhancedEngine_Optimized::calculateQuantumCoherence(const std::vector<uint8_t>& data) {
    if (data.size() < 256) return 0.5;
    
    // Calculate byte correlations as a measure of quantum coherence
    double correlation_sum = 0.0;
    int correlation_pairs = 0;
    
    for (size_t i = 0; i + 1 < data.size(); i += 2) {
        double a = data[i];
        double b = data[i + 1];
        
        // Simple correlation measure
        double correlation = 1.0 - std::abs(a - b) / 255.0;
        correlation_sum += correlation;
        correlation_pairs++;
    }
    
    return correlation_pairs > 0 ? correlation_sum / correlation_pairs : 0.5;
}

// ====================== EXISTING HELPER METHODS ======================
// (From analyzeDeep.cpp - included for completeness)

double V13_1GHz_EnhancedEngine_Optimized::determineOptimalSamplingRate(const std::vector<uint8_t>& data) {
    if (data.size() < 1024) {
        return 1.0; // Base 1 GHz
    }
    
    // Calculate entropy to determine sampling complexity
    double entropy = calculateEntropy(data);
    
    // Estimate compression ratio
    double compression_ratio = estimateCompressionRatio(data);
    
    // Adjust sampling rate based on data characteristics
    if (entropy > 7.0 && compression_ratio > 0.7) {
        return 2.7; // High frequency for encrypted/compressed data
    } else if (entropy > 6.0) {
        return 2.1; // Medium-high frequency
    } else if (data.size() > 10 * 1024 * 1024) { // >10MB
        return 1.8; // Higher for large files
    }
    
    return 1.0; // Default 1 GHz
}

double V13_1GHz_EnhancedEngine_Optimized::performGHzMicroarchitectureAnalysis(
    const std::vector<uint8_t>& data, double sampling_rate) {
    
    if (data.size() < 512) return 0.1;
    
    // Analyze for GHz-specific microarchitecture patterns
    double score = 0.0;
    
    // Check for cache access patterns
    std::vector<uint8_t> cache_patterns[] = {
        {0x0F, 0xAE}, // LFENCE/MFENCE
        {0x90, 0x90, 0x90}, // NOP sled
        {0xEB, 0xFE} // Infinite loop
    };
    
    for (const auto& pattern : cache_patterns) {
        if (std::search(data.begin(), data.end(), pattern.begin(), pattern.end()) != data.end()) {
            score += 0.2;
        }
    }
    
    // Analyze timing patterns
    double timing_variance = analyzeTimingVariance(data);
    score += timing_variance * 0.3;
    
    // Analyze memory access patterns
    double memory_pattern_score = analyzeMemoryAccessPatterns(data);
    score += memory_pattern_score * 0.3;
    
    // Adjust based on sampling rate
    score *= sampling_rate;
    
    return std::min(1.0, score);
}

double V13_1GHz_EnhancedEngine_Optimized::analyzeCacheTimingPatterns(const std::vector<uint8_t>& data) {
    if (data.size() < 256) return 0.1;
    
    // Check for cache timing attack patterns
    double score = 0.0;
    
    // RDTSC instruction
    if (std::search(data.begin(), data.end(), std::vector<uint8_t>{0x0F, 0x31}.begin(), 
                    std::vector<uint8_t>{0x0F, 0x31}.end()) != data.end()) {
        score += 0.4;
    }
    
    // Timing side-channel indicators
    double entropy_variation = calculateEntropyVariation(data, 64);
    score += entropy_variation * 0.3;
    
    // Check for flush patterns
    if (containsFlushPatterns(data)) {
        score += 0.3;
    }
    
    return std::min(1.0, score);
}

double V13_1GHz_EnhancedEngine_Optimized::analyzeBranchPrediction(const std::vector<uint8_t>& data) {
    if (data.size() < 1024) return 0.1;
    
    std::string data_str(data.begin(), data.end());
    double score = 0.0;
    
    // Count branch-related patterns
    std::vector<std::string> branch_indicators = {
        "jmp", "call", "ret", "je", "jne", "jz", "jnz"
    };
    
    int branch_count = 0;
    for (const auto& indicator : branch_indicators) {
        size_t pos = 0;
        while ((pos = data_str.find(indicator, pos)) != std::string::npos) {
            branch_count++;
            pos += indicator.length();
        }
    }
    
    // Calculate branch density
    double branch_density = static_cast<double>(branch_count) / (data.size() / 1000.0);
    if (branch_density > 5.0) {
        score += std::min(0.5, branch_density / 10.0);
    }
    
    // Check for indirect branches
    if (containsIndirectBranches(data)) {
        score += 0.3;
    }
    
    return std::min(1.0, score);
}

double V13_1GHz_EnhancedEngine_Optimized::detectRowhammerPatterns(const std::vector<uint8_t>& data) {
    if (data.size() < 2048) return 0.1;
    
    // Look for repetitive memory access patterns
    double score = 0.0;
    const size_t pattern_size = 64; // Cache line size
    
    // Count repeating 64-byte patterns
    std::map<std::vector<uint8_t>, int> pattern_counts;
    
    for (size_t i = 0; i + pattern_size <= data.size(); i += pattern_size) {
        std::vector<uint8_t> pattern(data.begin() + i, 
                                     data.begin() + std::min(i + pattern_size, data.size()));
        
        pattern_counts[pattern]++;
        
        if (pattern_counts[pattern] > 10) { // Highly repetitive
            score = std::min(1.0, score + 0.5);
        }
    }
    
    // Check for bit flip patterns
    if (containsBitFlipPatterns(data)) {
        score = std::min(1.0, score + 0.3);
    }
    
    return score;
}

double V13_1GHz_EnhancedEngine_Optimized::performQuantumHarmonicAnalysis(
    const std::vector<uint8_t>& data, double sampling_rate) {
    
    if (data.size() < 512) return 0.1;
    
    // Generate harmonic signature
    auto signature = generateHarmonicSignature(data);
    
    // Analyze frequency bands
    std::vector<double> frequency_bands = {
        27000.0, 54000.0, 108000.0, 216000.0,
        432000.0, 864000.0, 1728000.0, 3456000.0
    };
    
    double total_deviation = 0.0;
    size_t valid_bands = 0;
    
    for (size_t i = 0; i < signature.size() && i < frequency_bands.size(); ++i) {
        double expected = frequency_bands[i] / 1000000.0;
        double deviation = std::abs(signature[i] - expected) / (expected + 1e-10);
        total_deviation += deviation;
        valid_bands++;
    }
    
    double avg_deviation = valid_bands > 0 ? total_deviation / valid_bands : 0.0;
    
    // Calculate quantum coherence
    double coherence = calculateQuantumCoherence(data);
    
    // Combine into threat score
    double score = 0.6 * avg_deviation + 0.4 * (1.0 - coherence);
    
    return std::min(1.0, score * sampling_rate);
}

double V13_1GHz_EnhancedEngine_Optimized::calculateDeepAnalysisConfidence(
    double neural_score, double microarch_score, double cache_score,
    double branch_score, double rowhammer_score, double quantum_score) {
    
    // Weighted combination based on configuration
    double weights[] = {
        config.enable_deep_learning ? 0.25 : 0.0,
        config.enable_1GHz_microarch_detection ? 0.20 : 0.0,
        config.enable_1GHz_cache_timing ? 0.15 : 0.0,
        config.enable_1GHz_branch_prediction ? 0.15 : 0.0,
        config.enable_1GHz_rowhammer ? 0.10 : 0.0,
        config.enable_quantum_hardware ? 0.15 : 0.0
    };
    
    double scores[] = {neural_score, microarch_score, cache_score, 
                       branch_score, rowhammer_score, quantum_score};
    
    double total_weight = 0.0;
    double weighted_sum = 0.0;
    
    for (int i = 0; i < 6; ++i) {
        if (weights[i] > 0) {
            weighted_sum += weights[i] * scores[i];
            total_weight += weights[i];
        }
    }
    
    if (total_weight > 0) {
        double base_confidence = weighted_sum / total_weight;
        
        // Boost confidence if multiple indicators agree
        int high_indicators = 0;
        for (double score : scores) {
            if (score > 0.7) high_indicators++;
        }
        
        if (high_indicators >= 2) {
            base_confidence = std::min(1.0, base_confidence * (1.0 + high_indicators * 0.1));
        }
        
        return base_confidence;
    }
    
    return 0.0;
}

ThreatSeverity V13_1GHz_EnhancedEngine_Optimized::determineThreatSeverity(double confidence) {
    if (confidence > 0.85) {
        return ThreatSeverity::CRITICAL;
    } else if (confidence > 0.65) {
        return ThreatSeverity::HIGH;
    } else if (confidence > 0.45) {
        return ThreatSeverity::MEDIUM;
    } else if (confidence > 0.25) {
        return ThreatSeverity::LOW;
    } else {
        return ThreatSeverity::LOW;
    }
}

// ====================== UTILITY HELPER FUNCTIONS ======================

std::vector<double> V13_1GHz_EnhancedEngine_Optimized::generateHarmonicSignature(
    const std::vector<uint8_t>& data) {
    
    std::vector<double> signature(8, 0.0);
    
    if (data.size() < 256) return signature;
    
    std::vector<double> frequency_bands = {
        27000.0, 54000.0, 108000.0, 216000.0,
        432000.0, 864000.0, 1728000.0, 3456000.0
    };
    
    for (size_t band = 0; band < frequency_bands.size(); ++band) {
        double freq = frequency_bands[band];
        double sum = 0.0;
        
        size_t step = std::max(size_t(1), data.size() / 100);
        for (size_t i = 0; i < data.size(); i += step) {
            double sample = data[i];
            double transformed = sample * std::sin(2 * M_PI * freq * i / data.size());
            sum += transformed;
        }
        
        signature[band] = sum / (data.size() / step);
    }
    
    return signature;
}

double V13_1GHz_EnhancedEngine_Optimized::calculateEntropyVariation(
    const std::vector<uint8_t>& data, size_t block_size) {
    
    if (data.size() < block_size * 4) return 0.0;
    
    std::vector<double> block_entropies;
    
    for (size_t i = 0; i + block_size <= data.size(); i += block_size) {
        std::vector<uint8_t> block(data.begin() + i, data.begin() + i + block_size);
        block_entropies.push_back(calculateEntropy(block));
    }
    
    if (block_entropies.size() < 2) return 0.0;
    
    // Calculate variance of entropies
    double mean = 0.0;
    for (double e : block_entropies) mean += e;
    mean /= block_entropies.size();
    
    double variance = 0.0;
    for (double e : block_entropies) {
        double diff = e - mean;
        variance += diff * diff;
    }
    variance /= block_entropies.size();
    
    return std::min(1.0, variance / 2.0); // Normalize
}

bool V13_1GHz_EnhancedEngine_Optimized::containsFlushPatterns(const std::vector<uint8_t>& data) {
    std::vector<std::vector<uint8_t>> flush_patterns = {
        {0x0F, 0xAE, 0xF8}, // CLFLUSH
        {0x0F, 0xAE, 0xF0}, // CLFLUSHOPT
        {0x66, 0x0F, 0xAE, 0xF8} // CLFLUSH with prefix
    };
    
    for (const auto& pattern : flush_patterns) {
        if (std::search(data.begin(), data.end(), pattern.begin(), pattern.end()) != data.end()) {
            return true;
        }
    }
    
    return false;
}

bool V13_1GHz_EnhancedEngine_Optimized::containsIndirectBranches(const std::vector<uint8_t>& data) {
    // Check for indirect call/jump patterns
    std::vector<std::vector<uint8_t>> indirect_patterns = {
        {0xFF, 0x10}, // call [eax]
        {0xFF, 0x20}, // jmp [eax]
        {0xFF, 0x50}, // call [eax+offset]
        {0xFF, 0x60}, // jmp [eax+offset]
    };
    
    for (const auto& pattern : indirect_patterns) {
        if (std::search(data.begin(), data.end(), pattern.begin(), pattern.end()) != data.end()) {
            return true;
        }
    }
    
    return false;
}

bool V13_1GHz_EnhancedEngine_Optimized::containsBitFlipPatterns(const std::vector<uint8_t>& data) {
    if (data.size() < 128) return false;
    
    // Look for patterns that could cause bit flips (adjacent bits changing)
    int bit_flip_count = 0;
    
    for (size_t i = 0; i < data.size() - 1; ++i) {
        uint8_t a = data[i];
        uint8_t b = data[i + 1];
        
        // Check if only one bit differs (potential bit flip)
        uint8_t xor_result = a ^ b;
        if (xor_result && ((xor_result & (xor_result - 1)) == 0)) {
            bit_flip_count++;
        }
    }
    
    return bit_flip_count > 10; // Threshold
}

double V13_1GHz_EnhancedEngine_Optimized::analyzeTimingVariance(const std::vector<uint8_t>& data) {
    if (data.size() < 512) return 0.0;
    
    // Analyze timing patterns by looking at instruction sequences
    int timing_sequences = 0;
    
    for (size_t i = 0; i + 4 <= data.size(); ++i) {
        // Check for instruction pairs with timing implications
        if ((data[i] == 0x90 && data[i + 1] == 0x90) || // NOP NOP
            (data[i] == 0xF3 && data[i + 1] == 0x90)) { // PAUSE
            timing_sequences++;
        }
    }
    
    return std::min(1.0, timing_sequences / 100.0);
}

double V13_1GHz_EnhancedEngine_Optimized::analyzeMemoryAccessPatterns(const std::vector<uint8_t>& data) {
    if (data.size() < 1024) return 0.0;
    
    // Check for memory access instructions
    std::vector<uint8_t> memory_instructions = {
        0x8B, // MOV reg, [mem]
        0x89, // MOV [mem], reg
        0xA1, // MOV eax, [mem]
        0xA3, // MOV [mem], eax
    };
    
    int memory_access_count = 0;
    for (uint8_t byte : data) {
        if (std::find(memory_instructions.begin(), memory_instructions.end(), byte) != memory_instructions.end()) {
            memory_access_count++;
        }
    }
    
    double density = static_cast<double>(memory_access_count) / data.size();
    return std::min(1.0, density * 10.0);
}

// ====================== INTEGRATION HELPER ======================

// Example usage function
ThreatReport analyzeWithV13(const std::vector<uint8_t>& data) {
    static V13_1GHz_EnhancedEngine_Optimized engine;
    return engine.analyzeDeep(data);
}