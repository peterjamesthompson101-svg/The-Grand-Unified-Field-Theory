import numpy as np
import matplotlib.pyplot as plt

# Simulated infrared waveform traveling through quartz medium
x = np.linspace(0, 10, 500)
primary_wave = np.sin(x * 2 * np.pi * 0.2)  # Primary infrared wave
birefringent_wave = np.sin(x * 2 * np.pi * 0.2 + np.pi/3)  # Birefringent secondary wave

# Creating the visualization
fig, ax = plt.subplots(figsize=(10, 5))
ax.plot(x, primary_wave, label="Primary IR Wave", color="red", linestyle="dashed")
ax.plot(x, birefringent_wave, label="Birefringent IR Wave", color="blue")

# Annotating the structural components
ax.axvspan(3, 7, color="gray", alpha=0.2, label="Quartz Medium (P-N Junction Sandwiched)")
ax.set_title("Infrared Birefringence in Superconducting Quartz Medium")
ax.set_xlabel("Propagation Distance")
ax.set_ylabel("Wave Intensity")
ax.legend()

plt.show()
