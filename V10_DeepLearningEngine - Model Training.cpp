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
        int input_size;
        int output_size;
        std::string activation;
        double dropout_rate;
        bool use_batch_norm;
        
        Layer(int in_size, int out_size, const std::string& act = "relu", 
              double dropout = 0.0, bool batch_norm = false)
            : input_size(in_size), output_size(out_size), 
              activation(act), dropout_rate(dropout), use_batch_norm(batch_norm) {
            
            // Initialize weights with He initialization
            std::random_device rd;
            std::mt19937 gen(rd());
            std::normal_distribution<double> dist(0.0, std::sqrt(2.0 / input_size));
            
            weights.resize(output_size);
            for (int i = 0; i < output_size; ++i) {
                weights[i].resize(input_size);
                for (int j = 0; j < input_size; ++j) {
                    weights[i][j] = dist(gen);
                }
            }
            
            biases.resize(output_size, 0.1);
            activations.resize(output_size, 0.0);
            gradients.resize(output_size, 0.0);
            weight_gradients.resize(output_size * input_size, 0.0);
            bias_gradients.resize(output_size, 0.0);
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
                    activation_grad *= (activations[i] > 0) ? 1.0 : 0.01; // leaky ReLU derivative
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
                bias_gradients[i] = 0.0;
                
                // Update weights with weight decay
                for (int j = 0; j < input_size; ++j) {
                    int idx = i * input_size + j;
                    // L2 regularization
                    weights[i][j] -= scale * (weight_gradients[idx] + weight_decay * weights[i][j]);
                    weight_gradients[idx] = 0.0;
                }
            }
        }
        
    private:
        std::vector<double> batchNorm(const std::vector<double>& input, bool training) {
            static std::vector<double> running_mean(output_size, 0.0);
            static std::vector<double> running_var(output_size, 1.0);
            static double momentum = 0.9;
            
            std::vector<double> normalized(output_size);
            
            if (training) {
                // Calculate batch statistics
                double mean = 0.0, var = 0.0;
                for (double val : input) mean += val;
                mean /= output_size;
                
                for (double val : input) var += (val - mean) * (val - mean);
                var /= output_size;
                
                // Update running statistics
                for (int i = 0; i < output_size; ++i) {
                    running_mean[i] = momentum * running_mean[i] + (1 - momentum) * mean;
                    running_var[i] = momentum * running_var[i] + (1 - momentum) * var;
                }
                
                // Normalize
                for (int i = 0; i < output_size; ++i) {
                    normalized[i] = (input[i] - mean) / sqrt(var + 1e-8);
                }
            } else {
                // Use running statistics during inference
                for (int i = 0; i < output_size; ++i) {
                    normalized[i] = (input[i] - running_mean[i]) / sqrt(running_var[i] + 1e-8);
                }
            }
            
            return normalized;
        }
        
        void applyDropout(double rate) {
            static std::random_device rd;
            static std::mt19937 gen(rd());
            static std::uniform_real_distribution<> dis(0.0, 1.0);
            
            double scale = 1.0 / (1.0 - rate);
            for (int i = 0; i < output_size; ++i) {
                if (dis(gen) < rate) {
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
        
        // Add hidden layers with ReLU activation
        for (int hidden_size : hidden_sizes) {
            layers.emplace_back(prev_size, hidden_size, "leaky_relu", 0.2, true);
            prev_size = hidden_size;
        }
        
        // Add output layer (sigmoid for binary classification)
        layers.emplace_back(prev_size, out_size, "sigmoid", 0.0, false);
    }
    
    double predict(const std::vector<double>& input) {
        std::vector<double> activation = input;
        
        // Forward pass through all layers
        for (auto& layer : layers) {
            activation = layer.forward(activation, false); // false for inference
        }
        
        // For binary classification, return the single output
        return activation[0];
    }
    
    double trainBatch(const std::vector<std::vector<double>>& batch_inputs,
                     const std::vector<double>& batch_targets) {
        double batch_loss = 0.0;
        
        // Forward and backward pass for each sample in batch
        for (size_t i = 0; i < batch_inputs.size(); ++i) {
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
            std::vector<double> grad = computeLossGradient(output, batch_targets[i]);
            
            // Propagate backwards through layers
            for (int l = layers.size() - 1; l >= 0; --l) {
                // Get input to this layer (or original input for first layer)
                std::vector<double> layer_input = (l == 0) ? batch_inputs[i] : layers[l-1].activations;
                grad = layers[l].backward(layer_input, grad);
            }
        }
        
        // Update weights for all layers
        for (auto& layer : layers) {
            layer.updateWeights(learning_rate, batch_inputs.size());
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
        
        std::cout << "Training Deep Learning Model...\n";
        std::cout << "Training samples: " << train_inputs.size() << "\n";
        std::cout << "Validation samples: " << val_inputs.size() << "\n";
        std::cout << "Epochs: " << epochs << "\n";
        std::cout << "Batch size: " << batch_size << "\n";
        
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
            
            epoch_loss /= num_batches;
            
            // Calculate validation loss
            double val_loss = 0.0;
            for (size_t i = 0; i < val_inputs.size(); ++i) {
                double output = predict(val_inputs[i]);
                val_loss += computeLoss(output, val_targets[i]);
            }
            val_loss /= val_inputs.size();
            
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
            std::cout << "Epoch " << epoch + 1 << "/" << epochs
                      << " - Loss: " << epoch_loss
                      << " - Val Loss: " << val_loss
                      << " - Train Acc: " << train_accuracy
                      << " - Val Acc: " << val_accuracy
                      << " - Time: " << epoch_duration.count() << "s\n";
            
            // Learning rate decay
            learning_rate *= learning_rate_decay;
            
            // Early stopping check
            if (early_stopping) {
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
    }
    
    void saveModel(const std::string& filename) {
        std::ofstream file(filename, std::ios::binary);
        if (!file) {
            std::cerr << "Error: Could not save model to " << filename << "\n";
            return;
        }
        
        // Save architecture
        file.write(reinterpret_cast<const char*>(&input_size), sizeof(int));
        file.write(reinterpret_cast<const char*>(&output_size), sizeof(int));
        
        int num_layers = layers.size();
        file.write(reinterpret_cast<const char*>(&num_layers), sizeof(int));
        
        // Save each layer
        for (const auto& layer : layers) {
            int layer_input = layer.input_size;
            int layer_output = layer.output_size;
            file.write(reinterpret_cast<const char*>(&layer_input), sizeof(int));
            file.write(reinterpret_cast<const char*>(&layer_output), sizeof(int));
            
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
        }
        
        file.close();
        std::cout << "Model saved to " << filename << "\n";
    }
    
    void loadModel(const std::string& filename) {
        std::ifstream file(filename, std::ios::binary);
        if (!file) {
            std::cerr << "Error: Could not load model from " << filename << "\n";
            return;
        }
        
        // Load architecture
        file.read(reinterpret_cast<char*>(&input_size), sizeof(int));
        file.read(reinterpret_cast<char*>(&output_size), sizeof(int));
        
        int num_layers;
        file.read(reinterpret_cast<char*>(&num_layers), sizeof(int));
        
        layers.clear();
        
        // Load each layer
        for (int l = 0; l < num_layers; ++l) {
            int layer_input, layer_output;
            file.read(reinterpret_cast<char*>(&layer_input), sizeof(int));
            file.read(reinterpret_cast<char*>(&layer_output), sizeof(int));
            
            Layer layer(layer_input, layer_output, "relu");
            
            // Load weights
            for (int i = 0; i < layer_output; ++i) {
                for (int j = 0; j < layer_input; ++j) {
                    double weight;
                    file.read(reinterpret_cast<char*>(&weight), sizeof(double));
                    layer.weights[i][j] = weight;
                }
            }
            
            // Load biases
            for (int i = 0; i < layer_output; ++i) {
                double bias;
                file.read(reinterpret_cast<char*>(&bias), sizeof(double));
                layer.biases[i] = bias;
            }
            
            layers.push_back(layer);
        }
        
        file.close();
        std::cout << "Model loaded from " << filename << "\n";
    }
    
    void plotTrainingHistory() {
        std::cout << "\nTraining History:\n";
        std::cout << "Epoch\tTrain Loss\tVal Loss\tTrain Acc\tVal Acc\n";
        for (const auto& record : training_history) {
            std::cout << static_cast<int>(record[0]) + 1 << "\t"
                      << record[1] << "\t"
                      << record[2] << "\t"
                      << record[3] << "\t"
                      << record[4] << "\n";
        }
    }
    
private:
    double computeLoss(double output, double target) {
        if (loss_function == "binary_crossentropy") {
            // Binary cross-entropy loss
            double eps = 1e-8;
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
            return output - target;
        } else if (loss_function == "mse") {
            // Gradient of MSE
            return 2.0 * (output - target);
        }
        return 0.0;
    }
    
    double calculateAccuracy(const std::vector<std::vector<double>>& inputs,
                           const std::vector<double>& targets) {
        int correct = 0;
        for (size_t i = 0; i < inputs.size(); ++i) {
            double prediction = predict(inputs[i]);
            double predicted_class = (prediction > 0.5) ? 1.0 : 0.0;
            if (predicted_class == targets[i]) {
                correct++;
            }
        }
        return static_cast<double>(correct) / inputs.size();
    }
};

// Data Preprocessing and Augmentation
class DataProcessor {
public:
    static std::pair<std::vector<std::vector<double>>, std::vector<double>> 
    loadDataset(const std::string& filepath, double train_ratio = 0.8) {
        std::vector<std::vector<double>> features;
        std::vector<double> labels;
        
        // Simulated data loading - in real implementation, load from CSV/JSON
        std::cout << "Loading dataset from " << filepath << "...\n";
        
        // This would be replaced with actual file reading
        // For demonstration, create synthetic data
        std::random_device rd;
        std::mt19937 gen(rd());
        std::normal_distribution<> normal_dist(0.5, 0.2);
        std::uniform_int_distribution<> label_dist(0, 1);
        
        int num_samples = 10000;
        int feature_size = 256;
        
        for (int i = 0; i < num_samples; ++i) {
            std::vector<double> sample(feature_size);
            for (int j = 0; j < feature_size; ++j) {
                sample[j] = normal_dist(gen);
            }
            features.push_back(sample);
            labels.push_back(label_dist(gen));
        }
        
        std::cout << "Loaded " << features.size() << " samples\n";
        
        // Split into train and validation
        return splitDataset(features, labels, train_ratio);
    }
    
    static std::pair<std::vector<std::vector<double>>, std::vector<double>>
    splitDataset(const std::vector<std::vector<double>>& features,
                const std::vector<double>& labels,
                double train_ratio) {
        
        std::vector<size_t> indices(features.size());
        std::iota(indices.begin(), indices.end(), 0);
        std::shuffle(indices.begin(), indices.end(), 
                    std::mt19937(std::random_device{}()));
        
        size_t split_idx = static_cast<size_t>(features.size() * train_ratio);
        
        std::vector<std::vector<double>> train_features, val_features;
        std::vector<double> train_labels, val_labels;
        
        for (size_t i = 0; i < indices.size(); ++i) {
            if (i < split_idx) {
                train_features.push_back(features[indices[i]]);
                train_labels.push_back(labels[indices[i]]);
            } else {
                val_features.push_back(features[indices[i]]);
                val_labels.push_back(labels[indices[i]]);
            }
        }
        
        std::cout << "Train set: " << train_features.size() << " samples\n";
        std::cout << "Validation set: " << val_features.size() << " samples\n";
        
        return {{train_features, train_labels}, {val_features, val_labels}};
    }
    
    static void normalizeData(std::vector<std::vector<double>>& features) {
        if (features.empty()) return;
        
        size_t num_features = features[0].size();
        std::vector<double> means(num_features, 0.0);
        std::vector<double> stddevs(num_features, 0.0);
        
        // Calculate means
        for (const auto& sample : features) {
            for (size_t i = 0; i < num_features; ++i) {
                means[i] += sample[i];
            }
        }
        for (size_t i = 0; i < num_features; ++i) {
            means[i] /= features.size();
        }
        
        // Calculate standard deviations
        for (const auto& sample : features) {
            for (size_t i = 0; i < num_features; ++i) {
                double diff = sample[i] - means[i];
                stddevs[i] += diff * diff;
            }
        }
        for (size_t i = 0; i < num_features; ++i) {
            stddevs[i] = std::sqrt(stddevs[i] / features.size());
        }
        
        // Normalize
        for (auto& sample : features) {
            for (size_t i = 0; i < num_features; ++i) {
                if (stddevs[i] > 0) {
                    sample[i] = (sample[i] - means[i]) / stddevs[i];
                }
            }
        }
    }
    
    static std::vector<std::vector<double>> augmentData(
        const std::vector<std::vector<double>>& features,
        const std::vector<double>& labels,
        double noise_level = 0.1) {
        
        std::vector<std::vector<double>> augmented = features;
        std::random_device rd;
        std::mt19937 gen(rd());
        std::normal_distribution<> noise_dist(0.0, noise_level);
        
        for (const auto& sample : features) {
            std::vector<double> noisy_sample = sample;
            for (double& val : noisy_sample) {
                val += noise_dist(gen);
            }
            augmented.push_back(noisy_sample);
        }
        
        return augmented;
    }
};

// Main Training Program
class DeepLearningTrainer {
private:
    DeepLearningModel model;
    std::string dataset_path;
    
public:
    DeepLearningTrainer(const std::string& model_name = "threat_detector_v10")
        : model(256, {128, 64, 32}, 1, "binary_crossentropy", 0.001, 0.98) {
        dataset_path = model_name + "_dataset.csv";
    }
    
    void runTraining() {
        std::cout << "=== Deep Learning Model Training ===\n\n";
        
        // Step 1: Load and preprocess data
        auto [train_data, val_data] = DataProcessor::loadDataset(dataset_path);
        auto& [train_features, train_labels] = train_data;
        auto& [val_features, val_labels] = val_data;
        
        // Normalize features
        DataProcessor::normalizeData(train_features);
        DataProcessor::normalizeData(val_features);
        
        // Step 2: Train the model
        std::cout << "\n=== Starting Training ===\n";
        model.train(train_features, train_labels, 
                   val_features, val_labels,
                   50,  // epochs
                   64,  // batch size
                   true, // early stopping
                   5);  // patience
        
        // Step 3: Save the trained model
        model.saveModel("v10_deeplearning_model.bin");
        
        // Step 4: Show training history
        std::cout << "\n=== Training Summary ===\n";
        model.plotTrainingHistory();
        
        // Step 5: Evaluate on validation set
        evaluateModel(val_features, val_labels);
    }
    
    void evaluateModel(const std::vector<std::vector<double>>& features,
                      const std::vector<double>& labels) {
        std::cout << "\n=== Model Evaluation ===\n";
        
        int true_positives = 0, false_positives = 0;
        int true_negatives = 0, false_negatives = 0;
        
        for (size_t i = 0; i < features.size(); ++i) {
            double prediction = model.predict(features[i]);
            double predicted_class = (prediction > 0.5) ? 1.0 : 0.0;
            double actual_class = labels[i];
            
            if (predicted_class == 1.0 && actual_class == 1.0) true_positives++;
            else if (predicted_class == 1.0 && actual_class == 0.0) false_positives++;
            else if (predicted_class == 0.0 && actual_class == 0.0) true_negatives++;
            else if (predicted_class == 0.0 && actual_class == 1.0) false_negatives++;
        }
        
        double accuracy = static_cast<double>(true_positives + true_negatives) / features.size();
        double precision = static_cast<double>(true_positives) / (true_positives + false_positives + 1e-8);
        double recall = static_cast<double>(true_positives) / (true_positives + false_negatives + 1e-8);
        double f1_score = 2 * (precision * recall) / (precision + recall + 1e-8);
        
        std::cout << "Accuracy: " << accuracy * 100 << "%\n";
        std::cout << "Precision: " << precision << "\n";
        std::cout << "Recall: " << recall << "\n";
        std::cout << "F1 Score: " << f1_score << "\n";
        std::cout << "\nConfusion Matrix:\n";
        std::cout << "TP: " << true_positives << " | FP: " << false_positives << "\n";
        std::cout << "FN: " << false_negatives << " | TN: " << true_negatives << "\n";
    }
    
    void crossValidate(int k_folds = 5) {
        std::cout << "\n=== K-Fold Cross Validation (k=" << k_folds << ") ===\n";
        
        // Load full dataset
        auto [full_data, _] = DataProcessor::loadDataset(dataset_path);
        auto& [all_features, all_labels] = full_data;
        
        DataProcessor::normalizeData(all_features);
        
        std::vector<double> fold_accuracies;
        
        for (int fold = 0; fold < k_folds; ++fold) {
            std::cout << "\nFold " << fold + 1 << "/" << k_folds << ":\n";
            
            // Split data for this fold
            size_t fold_size = all_features.size() / k_folds;
            size_t val_start = fold * fold_size;
            size_t val_end = (fold == k_folds - 1) ? all_features.size() : val_start + fold_size;
            
            std::vector<std::vector<double>> train_features, val_features;
            std::vector<double> train_labels, val_labels;
            
            for (size_t i = 0; i < all_features.size(); ++i) {
                if (i >= val_start && i < val_end) {
                    val_features.push_back(all_features[i]);
                    val_labels.push_back(all_labels[i]);
                } else {
                    train_features.push_back(all_features[i]);
                    train_labels.push_back(all_labels[i]);
                }
            }
            
            // Create and train model for this fold
            DeepLearningModel fold_model(256, {128, 64, 32}, 1);
            fold_model.train(train_features, train_labels, 
                           val_features, val_labels,
                           30, 64, true, 5);
            
            // Evaluate
            int correct = 0;
            for (size_t i = 0; i < val_features.size(); ++i) {
                double prediction = fold_model.predict(val_features[i]);
                double predicted_class = (prediction > 0.5) ? 1.0 : 0.0;
                if (predicted_class == val_labels[i]) correct++;
            }
            
            double accuracy = static_cast<double>(correct) / val_features.size();
            fold_accuracies.push_back(accuracy);
            
            std::cout << "Fold accuracy: " << accuracy * 100 << "%\n";
        }
        
        // Calculate average accuracy
        double avg_accuracy = 0.0;
        for (double acc : fold_accuracies) avg_accuracy += acc;
        avg_accuracy /= k_folds;
        
        std::cout << "\n=== Cross Validation Results ===\n";
        std::cout << "Average accuracy: " << avg_accuracy * 100 << "%\n";
        std::cout << "Standard deviation: ";
        
        double variance = 0.0;
        for (double acc : fold_accuracies) {
            variance += (acc - avg_accuracy) * (acc - avg_accuracy);
        }
        variance /= k_folds;
        std::cout << std::sqrt(variance) * 100 << "%\n";
    }
};

// Integration with V10_DeepLearningEngine
// This function can be called from V10_DeepLearningEngine constructor or initialization
void initializeV10DeepLearningModel(const std::string& model_path = "") {
    if (!model_path.empty()) {
        // Load pre-trained model
        DeepLearningModel model(256, {128, 64, 32}, 1);
        model.loadModel(model_path);
        std::cout << "[V10] Deep Learning Model loaded successfully\n";
    } else {
        // Train new model
        DeepLearningTrainer trainer;
        trainer.runTraining();
    }
}

// Main training executable
int main() {
    std::cout << "V10 Deep Learning Model Training System\n";
    std::cout << "========================================\n\n";
    
    DeepLearningTrainer trainer;
    
    // Ask user what to do
    std::cout << "Select operation:\n";
    std::cout << "1. Train new model\n";
    std::cout << "2. Cross-validation\n";
    std::cout << "3. Load and evaluate existing model\n";
    std::cout << "Choice: ";
    
    int choice;
    std::cin >> choice;
    
    switch (choice) {
        case 1:
            trainer.runTraining();
            break;
        case 2:
            trainer.crossValidate(5);
            break;
        case 3:
            trainer.evaluateModel({}, {}); // Would need actual data
            break;
        default:
            std::cout << "Invalid choice\n";
    }
    
    return 0;
}