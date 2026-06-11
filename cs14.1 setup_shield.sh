#!/bin/bash

# QUANTUM SHIELD V11.1 - DEPENDENCY INSTALLATION SCRIPT
# Target: Raspberry Pi OS (Debian) / Ubuntu

echo "--- Starting Quantum Shield Environment Setup ---"

# Update package lists
sudo apt-get update

# Install Essential Build Tools
sudo apt-get install -y build-essential cmake git pkg-config

# Install OpenSSL (Cryptographic Layer)
sudo apt-get install -y libssl-dev

# Install nlohmann-json (Data Parsing)
sudo apt-get install -y nlohmann-json3-dev

# Install spdlog (High-Performance Logging)
sudo apt-get install -y libspdlog-dev

# Install C++ REST SDK (Blockchain/Cloud Networking)
sudo apt-get install -y libcpprest-dev

# Install WiringPi (Hardware GPIO/SPI Layer)
# Note: Newer Pi OS versions may require building from GitHub
if ! command -v gpio &> /dev/null
then
    echo "WiringPi not found. Attempting to install..."
    sudo apt-get install -y wiringpi || {
        echo "Standard install failed. Building from source..."
        cd /tmp
        git clone https://github.com/WiringPi/WiringPi.git
        cd WiringPi
        ./build
    }
fi

echo "--- Environment Ready. You can now build the project. ---"
