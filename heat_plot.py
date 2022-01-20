import os
import numpy as np
import matplotlib
import matplotlib.pyplot as plt
sols= np.array([ np.loadtxt(f) for f in os.listdir('.') if 'sol' in f])
for i in range(1,sols.shape[0]):
  sol = np.loadtxt('sol_%05d' % (i-1))
  plt.imsave('%05d.png' % (i-1), sol,cmap = plt.cm.jet,vmin=0,vmax=1)
comm="ffmpeg -i %05d.png heat.avi"
os.system(comm)
