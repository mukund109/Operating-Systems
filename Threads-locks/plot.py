import matplotlib.pyplot as plt
import subprocess
from math import log


runtimes = [] #microseconds
n = [i for i in range(1,8)]

for num_threads in n:
	time = 0;
	for s in range(10): #10 samples
		process = subprocess.run(["./counter-approx.exe",str(num_threads)], stdout=subprocess.PIPE)
		time += int(process.stdout)/1000
	runtimes.append(time/10)

print(runtimes)
print(n)
plt.plot(n, runtimes, marker = "o")
plt.xlabel("Number of threads")
plt.ylabel("Time taken (milliseconds)")
plt.savefig("thread-scaling_approx-counter.png")
