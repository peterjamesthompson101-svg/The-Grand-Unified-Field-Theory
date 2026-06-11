PatternBuffer acquirePatternBuffer() {
    std::lock_guard<std::mutex> lock(pool_mutex);
    
    // First try to find an available (not in use) buffer
    auto available_it = std::find_if(pool.begin(), pool.end(),
        [](const MemoryBlock& block) {
            return !block.in_use;
        });
    
    if (available_it != pool.end()) {
        // Found an available buffer
        available_it->in_use = true;
        available_it->last_used = std::chrono::steady_clock::now();
        size_t index = std::distance(pool.begin(), available_it);
        return PatternBuffer(&available_it->pattern_buffer, index);
    }
    
    // If all buffers are in use, use LRU to evict the oldest
    auto oldest = std::min_element(pool.begin(), pool.end(),
        [](const MemoryBlock& a, const MemoryBlock& b) {
            return a.last_used < b.last_used;
        });
    
    // Clear the old buffer content if needed
    if (oldest->pattern_buffer.needs_clear_on_reuse) {
        oldest->pattern_buffer.clear();
    }
    
    oldest->in_use = true;
    oldest->last_used = std::chrono::steady_clock::now();
    size_t index = std::distance(pool.begin(), oldest);
    return PatternBuffer(&oldest->pattern_buffer, index);
}