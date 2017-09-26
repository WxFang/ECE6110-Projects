import matplotlib.pyplot as plt
import sys
try:
    from PIL import Image
except:
    import Image
tcpList = ["TcpNewReno", "TcpHybla", "TcpWestwood", "TcpWestwoodPlus"]
spokes = [1, 4, 8, 16, 32]
for tcp in tcpList:
    vals = []
    for spoke in spokes:
        with open(str(tcp)+str(spoke)+".txt",'r') as fn:
            for line in fn.readlines():
                if "Totals" in line:
                    vals.append(float(line.split()[-1].strip("Mpbs")))
    plt.plot(spokes,vals)
plt.xlabel("Spokes")
plt.ylabel("Throughput (Mbps)")
plt.grid(True)
plt.savefig("results.jpg")
plt.show() 