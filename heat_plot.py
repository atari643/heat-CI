import os
import numpy as np
import matplotlib
import matplotlib.pyplot as plt

def heat_plot():

    #on compte le nombre de fichier (et on peut s'apercevoir qu'il ne sont pas lu dans l'ordre alphabétique)
    nb = 0
    for f in os.listdir('.'):
        if 'sol' in f:
            nb = nb + 1

    file = []
    # on construit un tableau avec les noms de fichier dans l'ordre alphabétique
    for i in range(0,nb):
        file.append("sol_" + str('{:05d}'.format(i)))

    sols= np.array([ np.loadtxt(f) for f in file], dtype=object)
    for i in range(1,sols.shape[0]):
        plt.imsave('%05d.png' % i, sols[i],cmap = plt.cm.jet,vmin=0,vmax=1)

    comm="ffmpeg -i %05d.png heat.avi"
    os.system(comm)

if __name__ == '__main__':
    heat_plot()
