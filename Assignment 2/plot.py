import matplotlib.pyplot as plt

# Data from the job output
threads = [8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48]
times = [2.192591, 1.500027, 1.093880, 0.873364, 0.746132, 0.623460, 0.546192, 0.736210, 0.700426, 0.697526, 0.650290]

# Plotting
plt.figure(figsize=(10, 6))
plt.plot(threads, times, marker='o', linestyle='-', label='Execution Time')
plt.title('Execution Time vs Number of Threads', fontsize=14)
plt.xlabel('Number of Threads', fontsize=12)
plt.ylabel('Execution Time (seconds)', fontsize=12)
plt.grid(True)
plt.legend(fontsize=12)
plt.xticks(threads)
plt.tight_layout()

# Show plot
plt.show()
