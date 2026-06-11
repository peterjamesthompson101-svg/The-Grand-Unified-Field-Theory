double gpuParallelAnalysis(const std::vector<uint8_t>& data) {
    static GPUAcceleratedAnalyzer analyzer("cuda"); // Or detect automatically
    return analyzer.analyzeData(data);
}