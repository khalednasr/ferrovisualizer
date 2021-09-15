from collections import deque
from threading import Thread
import numpy as np
from scipy import signal, fft

import matplotlib.pyplot as plt
from scipy.ndimage import filters

import sounddevice as sd

import pyqtgraph as pg
from pyqtgraph.Qt import QtGui, QtCore

update_interval = 0.01  # seconds

sampling_rate = 44100  # Hz
block_duration = 1e-3  # in seconds
buf_duration = 40e-3  # in seconds

blocklen = int(block_duration * sampling_rate) # number of samples per block
buflen = int(buf_duration * sampling_rate)   # number of samples
print(blocklen, buflen)
print(f"{blocklen=}, {buflen=}")
queue = deque(maxlen=buflen)

fftfreq = fft.rfftfreq(buflen, 1/sampling_rate)

filter_order = 3
filter_edges = [20, 500, 2000, 3000, 5000, 8000]
filter_ranges = [(f, filter_edges[i+1]) for i, f in enumerate(filter_edges[:-1])]

sig_ymax = 1
fft_ymax = 15
exp_ymax = 0.4

class MainWidow(pg.GraphicsLayoutWidget):
    def __init__(self):
        super(MainWidow, self).__init__(show=True, title="MTI Monitor", size=(1000,600))
        pg.setConfigOptions(antialias=True)
        
        self.data = []

        self.t = 0
        self.probes_plot = None
        self.probe_curves = None

        self.signal_plot = self.addPlot(title='Signal')
        self.signal_curve = self.signal_plot.plot([], [], name='Signal')
        self.signal_plot.setYRange(-sig_ymax,sig_ymax)

        self.fft_plot = self.addPlot(title='FFT')
        self.fft_curve = self.fft_plot.plot([1,2], [1], name='FFT', stepMode=True, fillLevel=0, fillOutline=True, brush=(0,0,255,150))
        self.fft_plot.setYRange(0,fft_ymax)

        self.exp_plot = self.addPlot(title='EXP')
        self.exp_curve = self.exp_plot.plot([1,2], [1], name='EXP', stepMode=True, fillLevel=0, fillOutline=True, brush=(0,0,255,150))
        self.exp_plot.setYRange(0,exp_ymax)

        self.timer = QtCore.QTimer()
        self.timer.timeout.connect(self.update)
        self.timer.start(int(update_interval*1e3))

        self.filters = []
        self.filtered_curves = []
        
        for i, range in enumerate(filter_ranges):
            self.filters.append(signal.butter(filter_order, np.array(range)/(0.5*sampling_rate), btype='band'))
            self.filtered_curves.append(self.signal_plot.plot([], [], pen=pg.intColor(i)))

                
    def update(self):
        if not len(self.data) == buflen:
            return
        
        spectrum = np.abs(fft.rfft(self.data))
        t = np.linspace(0, buf_duration, buflen)

        band_activity = np.zeros(len(self.filters))
        for i, filter in enumerate(self.filters):
            filtered_data = signal.lfilter(*filter, self.data)
            # filtered_data = signal.filtfilt(*filter, self.data)
            # filtered_data = signal.sosfilt(filter, self.data)
            self.filtered_curves[i].setData(t, filtered_data)
            band_activity[i] = np.ptp(filtered_data)
            # band_activity[i] = np.std(filtered_data)

        self.signal_curve.setData(t, self.data)
        self.fft_curve.setData(fftfreq, (spectrum[:-1]- np.min(spectrum[:-1])))

        # self.exp_curve.setData(np.log(filter_edges), band_activity - np.min(band_activity))
        self.exp_curve.setData(np.arange(len(filter_edges)), band_activity - np.min(band_activity))

        # print(f'{fftfreq[np.argmax(spectrum)]=}')

    def closeEvent(self, event):
        print("Close event sent")

    def callback_receive_data(self, indata, outdata, frames, time, status):
        queue.extend(indata.mean(axis=1))
        self.data = np.array(queue)

if __name__ == "__main__":
    app = QtGui.QApplication([])
    win = MainWidow()
    with sd.Stream(callback=win.callback_receive_data, samplerate=sampling_rate, blocksize=blocklen) as stream:  
        app.exec_()
