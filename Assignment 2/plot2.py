import matplotlib.pyplot as plt

# Data from the job output
niter_values = [31250000, 62500000, 125000000, 250000000, 500000000, 1000000000, 2000000000]
times = [0.050304, 0.074019, 0.137993, 0.164190, 0.298904, 0.560521, 1.487956]

# Plotting
plt.figure(figsize=(10, 6))
plt.plot(niter_values, times, marker='o', linestyle='-', label='Computation Time')
plt.title('Computation Time vs Number of Iterations', fontsize=14)
plt.xlabel('Number of Iterations (niter)', fontsize=12)
plt.ylabel('Computation Time (seconds)', fontsize=12)
# plt.xscale('log')  # Logarithmic scale for iterations
plt.grid(True, which="both", linestyle="--", linewidth=0.5)
plt.legend(fontsize=12)
plt.tight_layout()

# Show plot
plt.show()
