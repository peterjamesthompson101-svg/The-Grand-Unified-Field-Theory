import numpy as np
import matplotlib.pyplot as plt

# Battery Parameters
num_layers = 5  # Define total quartz layers
layer_length = 1.0  # meters
fiber_optic_efficiency = 0.95  # Infrared transmission efficiency
initial_ir_intensity = 100  # Arbitrary pulse strength (normalized)

# Infrared Decomposition Function
def infrared_decay(depth, doping_concentration, fiber_efficiency):
    return initial_ir_intensity * np.exp(-doping_concentration * depth) * fiber_efficiency

# Simulating Charge Propagation Across Layers
layer_positions = np.linspace(0, num_layers * layer_length, num_layers)
doping_levels = np.linspace(0.2, 1.0, num_layers)  # 0.2% to 1% doping concentration
charge_activation_threshold = 10  # Minimum IR intensity required for charge cycle

ir_intensity_levels = [
    infrared_decay(depth, doping_levels[i], fiber_optic_efficiency)
    for i, depth in enumerate(layer_positions)
]

# Plotting Infrared Decomposition
plt.figure(figsize=(8, 5))
plt.plot(layer_positions, ir_intensity_levels, marker="o", label="IR Intensity")
plt.axhline(y=charge_activation_threshold, color="r", linestyle="--", label="Charge Threshold")
plt.xlabel("Layer Depth (m)")
plt.ylabel("IR Intensity")
plt.title("Infrared Decomposition Across Battery Layers")
plt.legend()
plt.grid()
plt.show()