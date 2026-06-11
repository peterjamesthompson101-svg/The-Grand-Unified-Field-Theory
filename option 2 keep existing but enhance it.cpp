// Enhanced version within V6_EnterpriseQuantumSystem class
bool checkSignature(const std::vector<uint8_t>& data, const std::string& signature) {
    // Try enhanced signature checking first
    EnterpriseSignatureChecker enhancedChecker;
    if (enhancedChecker.checkSignature(data, signature)) {
        return true;
    }
    
    // Fallback to original method
    std::vector<uint8_t> pattern;
    for (char c : signature) {
        pattern.push_back(static_cast<uint8_t>(c));
    }
    return std::search(data.begin(), data.end(), pattern.begin(), pattern.end()) != data.end();
}