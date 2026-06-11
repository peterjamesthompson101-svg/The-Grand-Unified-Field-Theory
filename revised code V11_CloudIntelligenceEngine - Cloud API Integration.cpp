// V10_DeepLearningEngine - Model Training.cpp
#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#include <algorithm>
#include <fstream>
#include <chrono>
#include <thread>
#include <memory>
#include <queue>
#include <limits>
#include <numeric>
#include <iomanip>
#include <cstring>
#include <sstream>

// Enhanced Deep Learning Model with Training Capability
class DeepLearningModel {
private:
    struct Layer {
        std::vector<std::vector<double>> weights;
        std::vector<double> biases;
        std::vector<double> activations;
        std::vector<double> gradients;
        std::vector<double> weight_gradients;
        std::vector<double> bias_gradients;
        std::vector<double> running_mean;
        std::vector<double> running_var;
        
        int input_size;
        int output_size;
        std::string activation;
        double dropout_rate;
        bool use_batch_norm;
        
        std::mt19937 rng;
        
        Layer(int in_size, int out_size, const std::string& act = "relu", 
              double dropout = 0.0, bool batch_norm = false)
            : input_size(in_size), output_size(out_size), 
              activation(act), dropout_rate(dropout), 
              use_batch_norm(batch_norm),
              rng(std::random_device{}()) {
            
            // Initialize batch normalization statistics
            running_mean.resize(output_size, 0.0);
            running_var.resize(output_size, 1.0);
            
            // Initialize weights with He initialization
            std::normal_distribution<double> dist(0.0, std::sqrt(2.0 / input_size));
            
            weights.resize(output_size);
            for (int i = 0; i < output_size; ++i) {
                weights[i].resize(input_size);
                for (int j = 0; j < input_size; ++j) {
                    weights[i][j] = dist(rng);
                }
            }
            
            biases.resize(output_size, 0.1);
            activations.resize(output_size, 0.0);
            gradients.resize(output_size, 0.0);
            weight_gradients.resize(output_size * input_size, 0.0);
            bias_gradients.resize(output_size, 0.0);
        }
        
        void resetGradients() {
            std::fill(weight_gradients.begin(), weight_gradients.end(), 0.0);
            std::fill(bias_gradients.begin(), bias_gradients.end(), 0.0);
        }
        
        std::vector<double> forward(const std::vector<double>& input, bool training = true) {
            std::vector<double> pre_activation(output_size, 0.0);
            
            // Linear transformation: Wx + b
            for (int i = 0; i < output_size; ++i) {
                double sum = biases[i];
                for (int j = 0; j < input_size; ++j) {
                    sum += weights[i][j] * input[j];
                }
                pre_activation[i] = sum;
            }
            
            // Batch normalization if enabled
            if (use_batch_norm && training) {
                pre_activation = batchNorm(pre_activation, training);
            }
            
            // Apply activation function
            for (int i = 0; i < output_size; ++i) {
                if (activation == "relu") {
                    activations[i] = relu(pre_activation[i]);
                } else if (activation == "sigmoid") {
                    activations[i] = sigmoid(pre_activation[i]);
                } else if (activation == "tanh") {
                    activations[i] = tanh(pre_activation[i]);
                } else if (activation == "leaky_relu") {
                    activations[i] = leakyRelu(pre_activation[i]);
                } else {
                    activations[i] = pre_activation[i]; // linear
                }
            }
            
            // Apply dropout during training
            if (training && dropout_rate > 0.0) {
                applyDropout(dropout_rate);
            }
            
            return activations;
        }
        
        std::vector<double> backward(const std::vector<double>& input, 
                                    const std::vector<double>& upstream_grad) {
            std::vector<double> downstream_grad(input_size, 0.0);
            
            // Compute gradients for weights and biases
            for (int i = 0; i < output_size; ++i) {
                double activation_grad = upstream_grad[i];
                
                // Apply derivative of activation function
                if (activation == "relu") {
                    activation_grad *= (activations[i] > 0) ? 1.0 : 0.01;
                } else if (activation == "sigmoid") {
                    activation_grad *= activations[i] * (1 - activations[i]);
                } else if (activation == "tanh") {
                    activation_grad *= 1 - activations[i] * activations[i];
                } else if (activation == "leaky_relu") {
                    activation_grad *= (activations[i] > 0) ? 1.0 : 0.01;
                }
                
                gradients[i] = activation_grad;
                
                // Accumulate weight gradients
                for (int j = 0; j < input_size; ++j) {
                    int idx = i * input_size + j;
                    weight_gradients[idx] += activation_grad * input[j];
                    downstream_grad[j] += activation_grad * weights[i][j];
                }
                
                // Accumulate bias gradients
                bias_gradients[i] += activation_grad;
            }
            
            return downstream_grad;
        }
        
        void updateWeights(double learning_rate, int batch_size, 
                          double weight_decay = 0.0001) {
            double scale = learning_rate / batch_size;
            
            for (int i = 0; i < output_size; ++i) {
                // Update biases
                biases[i] -= scale * bias_gradients[i];
                
                // Update weights with weight decay
                for (int j = 0; j < input_size; ++j) {
                    int idx = i * input_size + j;
                    // L2 regularization
                    weights[i][j] -= scale * (weight_gradients[idx] + weight_decay * weights[i][j]);
                }
            }
            
            // Reset gradients for next batch
            resetGradients();
        }
        
    private:
        std::vector<double> batchNorm(const std::vector<double>& input, bool training) {
            static const double momentum = 0.9;
            static const double epsilon = 1e-8;
            
            std::vector<double> normalized(output_size);
            
            if (training) {
                // Calculate batch statistics
                double mean = 0.0, var = 0.0;
                for (double val : input) mean += val;
                mean /= output_size;
                
                for (double val : input) {
                    double diff = val - mean;
                    var += diff * diff;
                }
                var /= output_size;
                
                // Update running statistics
                for (int i = 0; i < output_size; ++i) {
                    running_mean[i] = momentum * running_mean[i] + (1 - momentum) * mean;
                    running_var[i] = momentum * running_var[i] + (1 - momentum) * var;
                }
                
                // Normalize
                double stddev = std::sqrt(std::max(var, 0.0) + epsilon);
                for (int i = 0; i < output_size; ++i) {
                    normalized[i] = (input[i] - mean) / stddev;
                }
            } else {
                // Use running statistics during inference
                for (int i = 0; i < output_size; ++i) {
                    double stddev = std::sqrt(std::max(running_var[i], 0.0) + epsilon);
                    normalized[i] = (input[i] - running_mean[i]) / stddev;
                }
            }
            
            return normalized;
        }
        
        void applyDropout(double rate) {
            std::uniform_real_distribution<> dis(0.0, 1.0);
            
            double scale = 1.0 / (1.0 - rate);
            for (int i = 0; i < output_size; ++i) {
                if (dis(rng) < rate) {
                    activations[i] = 0.0;
                } else {
                    activations[i] *= scale;
                }
            }
        }
        
        double relu(double x) { return std::max(0.0, x); }
        double sigmoid(double x) { return 1.0 / (1.0 + std::exp(-x)); }
        double tanh(double x) { return std::tanh(x); }
        double leakyRelu(double x) { return (x > 0) ? x : 0.01 * x; }
    };
    
    std::vector<Layer> layers;
    std::vector<std::vector<double>> training_history;
    std::string loss_function;
    double learning_rate;
    double learning_rate_decay;
    int input_size;
    int output_size;
    
public:
    DeepLearningModel(int in_size, const std::vector<int>& hidden_sizes, int out_size,
                     const std::string& loss = "binary_crossentropy",
                     double lr = 0.001, double lr_decay = 0.95)
        : input_size(in_size), output_size(out_size),
          loss_function(loss), learning_rate(lr), learning_rate_decay(lr_decay) {
        
        // Build network architecture
        int prev_size = in_size;
        
        // Add hidden layers with Leaky ReLU activation
        for (size_t idx = 0; idx < hidden_sizes.size(); ++idx) {
            int hidden_size = hidden_sizes[idx];
            // Use different dropout rates for different layers
            double dropout = 0.2 * (1.0 - static_cast<double>(idx) / hidden_sizes.size());
            layers.emplace_back(prev_size, hidden_size, "leaky_relu", dropout, true);
            prev_size = hidden_size;
        }
        
        // Add output layer (sigmoid for binary classification)
        layers.emplace_back(prev_size, out_size, "sigmoid", 0.0, false);
    }
    
    double predict(const std::vector<double>& input) {
        if (input.size() != static_cast<size_t>(input_size)) {
            std::cerr << "Error: Input size mismatch. Expected " << input_size 
                      << ", got " << input.size() << std::endl;
            return 0.0;
        }
        
        std::vector<double> activation = input;
        
        // Forward pass through all layers
        for (auto& layer : layers) {
            activation = layer.forward(activation, false);
        }
        
        // For binary classification, return the single output
        return activation[0];
    }
    
    double trainBatch(const std::vector<std::vector<double>>& batch_inputs,
                     const std::vector<double>& batch_targets) {
        if (batch_inputs.empty()) return 0.0;
        
        double batch_loss = 0.0;
        
        // Reset gradients for all layers
        for (auto& layer : layers) {
            layer.resetGradients();
        }
        
        // Forward and backward pass for each sample in batch
        for (size_t i = 0; i < batch_inputs.size(); ++i) {
            if (batch_inputs[i].size() != static_cast<size_t>(input_size)) {
                std::cerr << "Error: Batch input size mismatch at index " << i << std::endl;
                continue;
            }
            
            // Forward pass
            std::vector<double> activation = batch_inputs[i];
            for (auto& layer : layers) {
                activation = layer.forward(activation, true);
            }
            
            // Compute loss and gradient
            double output = activation[0];
            double loss = computeLoss(output, batch_targets[i]);
            batch_loss += loss;
            
            // Backward pass (start with output gradient)
            std::vector<double> grad = {computeLossGradient(output, batch_targets[i])};
            
            // Propagate backwards through layers
            for (int l = static_cast<int>(layers.size()) - 1; l >= 0; --l) {
                // Get input to this layer
                std::vector<double> layer_input;
                if (l == 0) {
                    layer_input = batch_inputs[i];
                } else {
                    layer_input = layers[l-1].activations;
                }
                grad = layers[l].backward(layer_input, grad);
            }
        }
        
        // Update weights for all layers
        for (auto& layer : layers) {
            layer.updateWeights(learning_rate, static_cast<int>(batch_inputs.size()));
        }
        
        return batch_loss / batch_inputs.size();
    }
    
    void train(const std::vector<std::vector<double>>& train_inputs,
              const std::vector<double>& train_targets,
              const std::vector<std::vector<double>>& val_inputs,
              const std::vector<double>& val_targets,
              int epochs = 100,
              int batch_size = 32,
              bool early_stopping = true,
              int patience = 10) {
        
        if (train_inputs.empty() || train_inputs.size() != train_targets.size()) {
            std::cerr << "Error: Invalid training data" << std::endl;
            return;
        }
        
        std::cout << "Training Deep Learning Model...\n";
        std::cout << "Training samples: " << train_inputs.size() << "\n";
        std::cout << "Validation samples: " << val_inputs.size() << "\n";
        std::cout << "Epochs: " << epochs << "\n";
        std::cout << "Batch size: " << batch_size << "\n";
        std::cout << "Initial learning rate: " << learning_rate << "\n";
        
        double best_val_loss = std::numeric_limits<double>::max();
        int epochs_without_improvement = 0;
        
        for (int epoch = 0; epoch < epochs; ++epoch) {
            auto epoch_start = std::chrono::high_resolution_clock::now();
            
            // Shuffle training data
            std::vector<size_t> indices(train_inputs.size());
            std::iota(indices.begin(), indices.end(), 0);
            std::shuffle(indices.begin(), indices.end(), 
                        std::mt19937(std::random_device{}()));
            
            double epoch_loss = 0.0;
            int num_batches = 0;
            
            // Mini-batch training
            for (size_t start = 0; start < train_inputs.size(); start += batch_size) {
                size_t end = std::min(start + batch_size, train_inputs.size());
                
                // Create batch
                std::vector<std::vector<double>> batch_inputs;
                std::vector<double> batch_targets;
                
                for (size_t i = start; i < end; ++i) {
                    size_t idx = indices[i];
                    batch_inputs.push_back(train_inputs[idx]);
                    batch_targets.push_back(train_targets[idx]);
                }
                
                // Train on batch
                double batch_loss = trainBatch(batch_inputs, batch_targets);
                epoch_loss += batch_loss;
                num_batches++;
            }
            
            if (num_batches > 0) {
                epoch_loss /= num_batches;
            }
            
            // Calculate validation loss
            double val_loss = 0.0;
            for (size_t i = 0; i < val_inputs.size(); ++i) {
                double output = predict(val_inputs[i]);
                val_loss += computeLoss(output, val_targets[i]);
            }
            if (!val_inputs.empty()) {
                val_loss /= val_inputs.size();
            }
            
            // Calculate accuracy
            double train_accuracy = calculateAccuracy(train_inputs, train_targets);
            double val_accuracy = calculateAccuracy(val_inputs, val_targets);
            
            auto epoch_end = std::chrono::high_resolution_clock::now();
            auto epoch_duration = std::chrono::duration<double>(epoch_end - epoch_start);
            
            // Store training history
            training_history.push_back({static_cast<double>(epoch), 
                                       epoch_loss, val_loss, 
                                       train_accuracy, val_accuracy});
            
            // Print progress
            std::cout << std::fixed << std::setprecision(6);
            std::cout << "Epoch " << std::setw(3) << epoch + 1 << "/" << epochs
                      << " - Loss: " << std::setw(10) << epoch_loss
                      << " - Val Loss: " << std::setw(10) << val_loss
                      << " - Train Acc: " << std::setw(6) << std::setprecision(4) << train_accuracy
                      << " - Val Acc: " << std::setw(6) << val_accuracy
                      << " - Time: " << std::setw(6) << std::setprecision(2) 
                      << epoch_duration.count() << "s";
            
            // Learning rate decay
            double old_lr = learning_rate;
            learning_rate *= learning_rate_decay;
            if (learning_rate < 1e-6) learning_rate = 1e-6;
            
            std::cout << " - LR: " << std::scientific << std::setprecision(1) 
                      << old_lr << "→" << learning_rate << std::fixed << std::endl;
            
            // Early stopping check
            if (early_stopping && !val_inputs.empty()) {
                if (val_loss < best_val_loss) {
                    best_val_loss = val_loss;
                    epochs_without_improvement = 0;
                } else {
                    epochs_without_improvement++;
                    if (epochs_without_improvement >= patience) {
                        std::cout << "Early stopping triggered at epoch " << epoch + 1 << "\n";
                        break;
                    }
                }
            }
        }
        
        std::cout << "Training completed.\n";
    }
    
    void saveModel(const std::string& filename) {
        std::ofstream file(filename, std::ios::binary);
        if (!file) {
            std::cerr << "Error: Could not open file for writing: " << filename << "\n";
            return;
        }
        
        try {
            // Save architecture
            file.write(reinterpret_cast<const char*>(&input_size), sizeof(int));
            file.write(reinterpret_cast<const char*>(&output_size), sizeof(int));
            
            int num_layers = static_cast<int>(layers.size());
            file.write(reinterpret_cast<const char*>(&num_layers), sizeof(int));
            
            // Save each layer
            for (const auto& layer : layers) {
                int layer_input = layer.input_size;
                int layer_output = layer.output_size;
                file.write(reinterpret_cast<const char*>(&layer_input), sizeof(int));
                file.write(reinterpret_cast<const char*>(&layer_output), sizeof(int));
                
                // Save activation function
                size_t act_len = layer.activation.size();
                file.write(reinterpret_cast<const char*>(&act_len), sizeof(size_t));
                file.write(layer.activation.c_str(), act_len);
                
                // Save dropout rate
                file.write(reinterpret_cast<const char*>(&layer.dropout_rate), sizeof(double));
                
                // Save batch normalization flag
                file.write(reinterpret_cast<const char*>(&layer.use_batch_norm), sizeof(bool));
                
                // Save weights
                for (int i = 0; i < layer.output_size; ++i) {
                    for (int j = 0; j < layer.input_size; ++j) {
                        double weight = layer.weights[i][j];
                        file.write(reinterpret_cast<const char*>(&weight), sizeof(double));
                    }
                }
                
                // Save biases
                for (double bias : layer.biases) {
                    file.write(reinterpret_cast<const char*>(&bias), sizeof(double));
                }
                
                // Save batch normalization statistics if used
                if (layer.use_batch_norm) {
                    for (double mean : layer.running_mean) {
                        file.write(reinterpret_cast<const char*>(&mean), sizeof(double));
                    }
                    for (double var : layer.running_var) {
                        file.write(reinterpret_cast<const char*>(&var), sizeof(double));
                    }
                }
            }
            
            if (!file) {
                std::cerr << "Error writing to file: " << filename << "\n";
            } else {
                std::cout << "Model saved to " << filename << " (" 
                          << file.tellp() << " bytes)\n";
            }
        } catch (const std::exception& e) {
            std::cerr << "Error saving model: " << e.what() << "\n";
        }
        
        file.close();
    }
    
    bool loadModel(const std::string& filename) {
        std::ifstream file(filename, std::ios::binary);
        if (!file) {
            std::cerr << "Error: Could not open file for reading: " << filename << "\n";
            return false;
        }
        
        try {
            // Load architecture
            file.read(reinterpret_cast<char*>(&input_size), sizeof(int));
            if (!file) throw std::runtime_error("Failed to read input_size");
            
            file.read(reinterpret_cast<char*>(&output_size), sizeof(int));
            if (!file) throw std::runtime_error("Failed to read output_size");
            
            int num_layers;
            file.read(reinterpret_cast<char*>(&num_layers), sizeof(int));
            if (!file) throw std::runtime_error("Failed to read num_layers");
            
            layers.clear();
            
            // Load each layer
            for (int l = 0; l < num_layers; ++l) {
                int layer_input, layer_output;
                file.read(reinterpret_cast<char*>(&layer_input), sizeof(int));
                if (!file) throw std::runtime_error("Failed to read layer_input");
                
                file.read(reinterpret_cast<char*>(&layer_output), sizeof(int));
                if (!file) throw std::runtime_error("Failed to read layer_output");
                
                // Read activation function
                size_t act_len;
                file.read(reinterpret_cast<char*>(&act_len), sizeof(size_t));
                if (!file) throw std::runtime_error("Failed to read activation length");
                
                std::string activation(act_len, '\0');
                file.read(&activation[0], act_len);
                if (!file) throw std::runtime_error("Failed to read activation string");
                
                // Read dropout rate
                double dropout_rate;
                file.read(reinterpret_cast<char*>(&dropout_rate), sizeof(double));
                if (!file) throw std::runtime_error("Failed to read dropout_rate");
                
                // Read batch normalization flag
                bool use_batch_norm;
                file.read(reinterpret_cast<char*>(&use_batch_norm), sizeof(bool));
                if (!file) throw std::runtime_error("Failed to read use_batch_norm");
                
                // Create layer
                Layer layer(layer_input, layer_output, activation, dropout_rate, use_batch_norm);
                
                // Load weights
                for (int i = 0; i < layer_output; ++i) {
                    for (int j = 0; j < layer_input; ++j) {
                        double weight;
                        file.read(reinterpret_cast<char*>(&weight), sizeof(double));
                        if (!file) throw std::runtime_error("Failed to read weight");
                        layer.weights[i][j] = weight;
                    }
                }
                
                // Load biases
                for (int i = 0; i < layer_output; ++i) {
                    double bias;
                    file.read(reinterpret_cast<char*>(&bias), sizeof(double));
                    if (!file) throw std::runtime_error("Failed to read bias");
                    layer.biases[i] = bias;
                }
                
                // Load batch normalization statistics if used
                if (use_batch_norm) {
                    for (int i = 0; i < layer_output; ++i) {
                        double mean;
                        file.read(reinterpret_cast<char*>(&mean), sizeof(double));
                        if (!file) throw std::runtime_error("Failed to read running_mean");
                        layer.running_mean[i] = mean;
                    }
                    
                    for (int i = 0; i < layer_output; ++i) {
                        double var;
                        file.read(reinterpret_cast<char*>(&var), sizeof(double));
                        if (!file) throw std::runtime_error("Failed to read running_var");
                        layer.running_var[i] = var;
                    }
                }
                
                layers.push_back(std::move(layer));
            }
            
            if (!file) {
                throw std::runtime_error("File read error before EOF");
            }
            
            std::cout << "Model loaded from " << filename << "\n";
            return true;
            
        } catch (const std::exception& e) {
            std::cerr << "Error loading model: " << e.what() << "\n";
            layers.clear();
            return false;
        }
    }
    
    void plotTrainingHistory() {
        if (training_history.empty()) {
            std::cout << "No training history available.\n";
            return;
        }
        
        std::cout << "\nTraining History:\n";
        std::cout << "Epoch\tTrain Loss\tVal Loss\tTrain Acc\tVal Acc\n";
        std::cout << std::fixed << std::setprecision(6);
        
        for (const auto& record : training_history) {
            std::cout << std::setw(3) << static_cast<int>(record[0]) + 1 << "\t"
                      << std::setw(10) << record[1] << "\t"
                      << std::setw(10) << record[2] << "\t"
                      << std::setprecision(4) << std::setw(8) << record[3] << "\t"
                      << std::setw(8) << record[4] << "\n";
            std::cout << std::setprecision(6);
        }
    }
    
    const std::vector<std::vector<double>>& getTrainingHistory() const {
        return training_history;
    }
    
private:
    double computeLoss(double output, double target) {
        if (loss_function == "binary_crossentropy") {
            // Binary cross-entropy loss with clipping
            const double eps = 1e-12;
            output = std::max(eps, std::min(1.0 - eps, output));
            return - (target * log(output) + (1 - target) * log(1 - output));
        } else if (loss_function == "mse") {
            // Mean squared error
            double error = output - target;
            return error * error;
        }
        return 0.0;
    }
    
    double computeLossGradient(double output, double target) {
        if (loss_function == "binary_crossentropy") {
            // Gradient of binary cross-entropy with sigmoid
            const double eps = 1e-12;
            output = std::max(eps, std::min(1.0 - eps, output));
            return output - target;
        } else if (loss_function == "mse") {
            // Gradient of MSE
            return 2.0 * (output - target);
        }
        return 0.0;
    }
    
    double calculateAccuracy(const std::vector<std::vector<double>>& inputs,
                           const std::vector<double>& targets) {
        if (inputs.empty() || inputs.size() != targets.size()) {
            return 0.0;
        }
        
        int correct = 0;
        for (size_t i = 0; i < inputs.size(); ++i) {
            double prediction = predict(inputs[i]);
            double predicted_class = (prediction > 0.5) ? 1.0 : 0.0;
            if (std::abs(predicted_class - targets[i]) < 0.5) {
                correct++;
            }
        }
        return static_cast<double>(correct) / inputs.size();
    }
};

// Main function with proper error handling
int main() {
    std::cout << "V10 Deep Learning Model Training System\n";
    std::cout << "========================================\n\n";
    
    try {
        // Test the model with a simple example
        DeepLearningModel model(2, {4, 3}, 1, "binary_crossentropy", 0.01, 0.99);
        
        // Create simple XOR-like dataset
        std::vector<std::vector<double>> train_inputs = {
            {0, 0}, {0, 1}, {1, 0}, {1, 1},
            {0, 0}, {0, 1}, {1, 0}, {1, 1}
        };
        std::vector<double> train_targets = {0, 1, 1, 0, 0, 1, 1, 0};
        
        std::vector<std::vector<double>> val_inputs = {
            {0, 0}, {0, 1}, {1, 0}, {1, 1}
        };
        std::vector<double> val_targets = {0, 1, 1, 0};
        
        std::cout << "Testing with XOR dataset...\n";
        model.train(train_inputs, train_targets, val_inputs, val_targets, 50, 2);
        
        // Test predictions
        std::cout << "\nTesting predictions:\n";
        for (const auto& input : val_inputs) {
            double pred = model.predict(input);
            std::cout << "Input [" << input[0] << ", " << input[1] << "] -> " 
                      << pred << " (expected: " 
                      << ((input[0] != input[1]) ? 1.0 : 0.0) << ")\n";
        }
        
        // Save model
        model.saveModel("xor_model.bin");
        
        // Load and test
        DeepLearningModel loaded_model(2, {4, 3}, 1);
        if (loaded_model.loadModel("xor_model.bin")) {
            std::cout << "\nLoaded model prediction for [1, 0]: " 
                      << loaded_model.predict({1, 0}) << "\n";
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}