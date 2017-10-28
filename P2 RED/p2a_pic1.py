import matplotlib.pyplot as plt
import xlrd

book = xlrd.open_workbook('p2c_pic2.xlsx')
book = book.sheet_by_index(0)

x = []
y1 = []
y2 = []

for i in range(500):
    x.append(book.cell_value(i, 0))
    y1.append(book.cell_value(i, 1))
    y2.append(book.cell_value(i, 2))

plt.plot(x, y1, 'r')
plt.plot(x, y2)
plt.xlabel('Time')
plt.ylabel('Queue for Gate B')
plt.title('Queue size (blue line) and average queue size (red line)')
plt.show()