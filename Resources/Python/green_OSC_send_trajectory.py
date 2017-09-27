'''
Simulate and send spiral 2D trajectory via OSC message
Input to OpenEphys GUI
'''


import OSC
import time
import numpy as np

c = OSC.OSCClient()
c.connect(('127.0.0.1', 27021))   # connect to SuperCollider

freq = 60  # Hz
T = 1./freq

prevTime = time.time() # sec
m_rad = 0
m_count = 0
m_forward=True

try:
    while True:
        currTime = time.time()

        if currTime-prevTime > T:

            theta = float(m_count / 60.)

            if m_forward:
                if m_rad < 0.5:
                    m_rad += 0.001
                else:
                    m_forward = False
            else:
                if m_rad > 0:
                    m_rad -= 0.005
                else:
                    m_forward = True

            m_x = m_rad * np.cos(theta) + 0.5
            m_y = m_rad * np.sin(theta) + 0.5

            rand = np.random.rand(1)
            if rand > 0.7:
                m_x = np.nan
                m_y = np.nan

            m_count += 1

            oscmsg = OSC.OSCMessage()
            oscmsg.setAddress("/green")
            oscmsg.append([m_x, m_y, 1., 1.])
            c.send(oscmsg)

            prevTime = currTime
            print 'sent: mx= ', m_x, ', m_y= ', m_y

except KeyboardInterrupt:
    pass



