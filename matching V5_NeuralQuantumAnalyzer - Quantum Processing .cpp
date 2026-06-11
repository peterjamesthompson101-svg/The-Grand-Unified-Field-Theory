// V5_NeuralQuantumAnalyzer - Quantum Processing.cpp
// Simple, working version

#include <complex>
#include <vector>
#include <cstdint>
#include <cmath>

std::vector<std::complex<double>> prepareQuantumState(const std::vector<uint8_t>& data) {
    std::vector<std::complex<double>> state(256, std::complex<double>(0, 0));
    
    if (data.empty()) return state;
    
    // Map bytes to quantum amplitudes
    for (size_t i = 0; i < std::min(data.size(), state.size()); ++i) {
        double amplitude = data[i] / 255.0;
        double phase = (i % 8) * M_PI / 4.0;
        state[i] = std::polar(amplitude, phase);
    }
    
    // Normalize (critical for quantum state preparation)
    double norm = 0.0;
    for (const auto& amp : state) {
        double mag = std::abs(amp);
        norm += mag * mag;
    }
    
    if (norm > 0.0) {
        norm = std::sqrt(norm);
        for (auto& amp : state) {
            amp /= norm;
        }
    }
    
    return state;
}