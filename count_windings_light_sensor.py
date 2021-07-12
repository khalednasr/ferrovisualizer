##%
import numpy as np
import scipy.signal
import matplotlib.pyplot as plt
# plt.closeall()

filename = r'data/20210625_oscilloscope_light_sensor_data_fancy_coil.csv'


data = np.loadtxt(filename, delimiter=',', skiprows=1)
t, x = data.T

# print(data.shape)
# print(data)

# peaks, properties = scipy.signal.find_peaks(-x, threshold=-3.5, height=-0.2)
indicator = np.diff((x < 4).astype(np.float64))
peaks = np.where(indicator==1)[0]

print(peaks.shape)
# print(peaks)

# print(np.diff(np.array([True, False, True])))

plt.plot(t, x, color='r')
plt.plot(t[1:], indicator, color='g')
# plt.scatter(t, t[peaks])
plt.vlines(t[peaks], ymin=0, ymax=5)
plt.show()


