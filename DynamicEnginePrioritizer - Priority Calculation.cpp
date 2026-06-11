#include <chrono>
#include <unordered_map>
#include <cmath>
#include <algorithm>
#include <vector>

class EnginePriority {
public:
    double base_priority;
    double current_priority;
    std::chrono::steady_clock::time_point last_boost;
    std::chrono::steady_clock::time_point last_activity;
    double decay_rate;  // Custom decay rate per engine
    int threat_count;   // Number of relevant threats
    
    EnginePriority(double base = 0.5) 
        : base_priority(base), current_priority(base), 
          last_boost(std::chrono::steady_clock::now()),
          last_activity(std::chrono::steady_clock::now()),
          decay_rate(0.05), threat_count(0) {}
};

class ThreatIntelligence {
public:
    struct ThreatData {
        double relevance_score;
        double confidence;
        std::chrono::steady_clock::time_point detection_time;
        std::vector<std::string> affected_engines;
    };
    
private:
    std::unordered_map<std::string, ThreatData> current_threats;
    
public:
    bool isEngineRelevant(const std::string& engine_name) const {
        for (const auto& [threat_id, threat] : current_threats) {
            if (std::find(threat.affected_engines.begin(), 
                         threat.affected_engines.end(), 
                         engine_name) != threat.affected_engines.end()) {
                return true;
            }
        }
        return false;
    }
    
    double getEngineRelevance(const std::string& engine_name) const {
        double max_relevance = 0.0;
        auto now = std::chrono::steady_clock::now();
        
        for (const auto& [threat_id, threat] : current_threats) {
            if (std::find(threat.affected_engines.begin(),
                         threat.affected_engines.end(),
                         engine_name) != threat.affected_engines.end()) {
                
                // Apply time decay to relevance (newer threats more relevant)
                auto age = std::chrono::duration_cast<std::chrono::hours>(now - threat.detection_time);
                double time_decay = exp(-static_cast<double>(age.count()) / 24.0); // 24-hour half-life
                
                double adjusted_relevance = threat.relevance_score * 
                                           threat.confidence * 
                                           time_decay;
                
                max_relevance = std::max(max_relevance, adjusted_relevance);
            }
        }
        return max_relevance;
    }
    
    void addThreat(const std::string& threat_id, const ThreatData& data) {
        current_threats[threat_id] = data;
    }
    
    void removeThreat(const std::string& threat_id) {
        current_threats.erase(threat_id);
    }
    
    // Getters for ML integration
    const auto& getAllThreats() const { return current_threats; }
};

class DynamicEnginePrioritizer {
private:
    std::unordered_map<std::string, EnginePriority> engine_priorities;
    
    // ML model parameters (simplified - would be loaded from actual model)
    struct MLParams {
        double threat_boost_factor = 0.3;
        double activity_boost_factor = 0.1;
        double exponential_decay_base = 0.9;
        double min_priority = 0.1;
        double max_priority = 1.0;
    } ml_params;
    
public:
    void updateBasedOnThreatIntelligence(const ThreatIntelligence& intel) {
        auto now = std::chrono::steady_clock::now();
        
        for (auto& [name, priority] : engine_priorities) {
            // 1. Calculate time-based decay with exponential smoothing
            auto time_since_boost = std::chrono::duration_cast<std::chrono::minutes>(
                now - priority.last_boost);
            auto time_since_activity = std::chrono::duration_cast<std::chrono::minutes>(
                now - priority.last_activity);
            
            // Exponential decay: priority * decay_factor^time
            double decay_factor = pow(ml_params.exponential_decay_base, 
                                     time_since_boost.count() / 60.0); // Convert to hours
            
            double decayed_priority = priority.base_priority + 
                                     (priority.current_priority - priority.base_priority) * 
                                     decay_factor;
            
            // 2. Apply activity-based adjustment (recently active engines get small boost)
            double activity_adjustment = 0.0;
            if (time_since_activity.count() < 30) { // Active in last 30 minutes
                activity_adjustment = ml_params.activity_boost_factor * 
                                     exp(-time_since_activity.count() / 30.0);
            }
            
            // 3. Apply threat intelligence with ML-based adjustment
            double threat_adjustment = 0.0;
            if (intel.isEngineRelevant(name)) {
                double raw_relevance = intel.getEngineRelevance(name);
                
                // ML-based adjustment: non-linear response to threat relevance
                threat_adjustment = ml_params.threat_boost_factor * 
                                   tanh(raw_relevance * 2.0); // Saturating function
                
                // Boost threat counter for persistence
                priority.threat_count++;
                
                // Higher threat count leads to higher sustained priority
                double persistence_boost = 0.05 * log1p(priority.threat_count);
                threat_adjustment += persistence_boost;
                
                priority.last_boost = now; // Reset decay timer for threats
            } else {
                // Gradually reduce threat count when no threats
                priority.threat_count = std::max(0, priority.threat_count - 1);
            }
            
            // 4. Combine all adjustments
            double new_priority = decayed_priority + 
                                 threat_adjustment + 
                                 activity_adjustment;
            
            // 5. Apply bounds and smooth transitions
            new_priority = std::clamp(new_priority, 
                                     ml_params.min_priority, 
                                     ml_params.max_priority);
            
            // Smooth transition to avoid abrupt changes (low-pass filter)
            priority.current_priority = 0.7 * priority.current_priority + 
                                       0.3 * new_priority;
            
            // Update activity timestamp if this engine was processed
            priority.last_activity = now;
        }
    }
    
    // Add engine to prioritizer
    void addEngine(const std::string& name, double base_priority = 0.5) {
        engine_priorities.emplace(name, EnginePriority(base_priority));
    }
    
    // Get current priority for an engine
    double getPriority(const std::string& name) const {
        auto it = engine_priorities.find(name);
        if (it != engine_priorities.end()) {
            return it->second.current_priority;
        }
        return 0.0;
    }
    
    // Manually adjust engine priority (for testing/override)
    void setPriority(const std::string& name, double priority) {
        auto it = engine_priorities.find(name);
        if (it != engine_priorities.end()) {
            it->second.current_priority = std::clamp(priority, 
                                                    ml_params.min_priority, 
                                                    ml_params.max_priority);
            it->second.last_boost = std::chrono::steady_clock::now();
        }
    }
    
    // Get sorted list of engines by priority
    std::vector<std::pair<std::string, double>> getPrioritizedEngines() const {
        std::vector<std::pair<std::string, double>> result;
        result.reserve(engine_priorities.size());
        
        for (const auto& [name, priority] : engine_priorities) {
            result.emplace_back(name, priority.current_priority);
        }
        
        std::sort(result.begin(), result.end(),
                 [](const auto& a, const auto& b) {
                     return a.second > b.second; // Descending order
                 });
        
        return result;
    }
    
    // Update ML parameters (would normally come from trained model)
    void updateMLParameters(const MLParams& params) {
        ml_params = params;
    }
};