// In V6_EnterpriseQuantumSystem class:
private:
    EnterpriseSignatureChecker signatureChecker;
    
    bool checkSignature(const std::vector<uint8_t>& data, const std::string& signature) {
        return signatureChecker.checkSignature(data, signature);
    }