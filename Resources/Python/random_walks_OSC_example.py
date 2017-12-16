'''
Simulate and send spiral 2D trajectory via OSC message
Input to OpenEphys GUI
'''


import OSC
import time
import numpy as np
import matplotlib.pylab as plt

n_sources = 10
ports = range(27020, 27020 + n_sources)
addresses = ['/red', '/green', '/blue',  '/magenta', '/cyan', '/orange', '/pink', '/grey', '/violet', '/yellow']
frequencies = np.linspace(30, 300, n_sources)  # Hz
duration = 60  # sec
nsamples = []
osc_ports = []
periods = []
counts = []
x_vec = []
y_vec = []
box_xlen = 1.
box_ylen = 1.
width = 1.
height = 1.

print 'Creating trajectories'
for (p, addr, freq) in zip(ports, addresses, frequencies):
    c = OSC.OSCClient()
    c.connect(('127.0.0.1', p))
    osc_ports.append(c)
    nsamples.append(int(duration*freq))
    periods.append(1./float(freq))

    x = []
    y = []
    x.append(np.random.uniform(0, 1))
    y.append(np.random.uniform(0, 1))

    while len(x) < nsamples[-1]:
        dx, dy = 0.05*np.random.randn(2)
        x_n, y_n = (x[-1] + dx) * box_xlen, (y[-1] + dy) * box_ylen

        while x_n < 0 or y_n < 0 or x_n > box_xlen or y_n > box_ylen:
            dx, dy = 0.05*np.random.randn(2)
            x_n, y_n = (x[-1] + dx) * box_xlen, (y[-1] + dy) * box_ylen


        x.append(x_n)
        y.append(y_n)

    x_vec.append(x)
    y_vec.append(y)

counts = [0] * n_sources
prev_t = [time.time()] * n_sources
max_samples = np.max(nsamples)

sent_trajectories = [[] for _ in range(n_sources)]

print 'Sending trajectories'
try:
    while True:
        curr_t = time.time()

        for idx, c in enumerate(osc_ports):
            if curr_t - prev_t[idx] > periods[idx]:

                if counts[idx] < max_samples:
                    m_x = x_vec[idx][counts[idx]]
                    m_y = y_vec[idx][counts[idx]]
                else:
                    raise Exception('Finished sending samples')

                rand = np.random.rand(1)
                if rand > 0.7:
                    m_x = np.nan
                    m_y = np.nan

                counts[idx] += 1

                oscmsg = OSC.OSCMessage()
                oscmsg.setAddress(addresses[idx])
                oscmsg.append([m_x, m_y, width, height])
                sent_trajectories[idx].append([m_x, m_y, width, height])
                c.send(oscmsg)

                prev_t[idx] = curr_t
                print 'sent port: ', ports[idx] , 'address: ', addresses[idx]

except KeyboardInterrupt:
    pass



