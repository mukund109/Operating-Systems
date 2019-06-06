import matplotlib.pyplot as plt
import subprocess
from math import log

samples = "10000000"
runtimes = [] #milliseconds
n = [2**i for i in range(0,14)]
for num_pages in n:
	process = subprocess.run(["./tlb.exe",str(num_pages),samples], stdout=subprocess.PIPE)
	runtimes.append(int(process.stdout)*1000/float(samples))

print(runtimes)
print(n)
plt.plot(n, runtimes, marker = "o")
plt.xlabel("Number of pages touched")
plt.xscale('log', basex = 2)
plt.ylabel("Time taken per access (nanoseconds)")
plt.savefig("TLB-size-measurement.png")
