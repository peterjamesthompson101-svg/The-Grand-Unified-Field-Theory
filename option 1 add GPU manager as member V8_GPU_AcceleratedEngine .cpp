class V8_GPU_AcceleratedEngine {
private:
    std::unique_ptr<GPUKernelManager> gpu_manager;
    // ... existing members
    
public:
    V8_GPU_AcceleratedEngine(const std::string& backend = "cuda") 
        : gpu_backend(backend) {
        std::cout << "[V8] GPU Accelerated Engine Initialized (" << backend << ")\n";
        gpu_manager = std::make_unique<GPUKernelManager>(
            backend == "cuda" ? GPUBackend::CUDA : GPUBackend::CPU_SIMULATION
        );
    }
    
    double gpuParallelAnalysis(const std::vector<uint8_t>& data) {
        return gpu_manager->gpuParallelAnalysis(data);
    }
    
    double gpuPatternMatching(const std::vector<uint8_t>& data) {
        return gpu_manager->gpuPatternMatching(data);
    }
};