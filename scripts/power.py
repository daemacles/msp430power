import os
import sys
import numpy as np
import teksocket

#C = 101.4e-6
C = 10.78e-6
Vreg = 1.77
LDOCurrent = 1.4e-6

def calc(delta_T, delta_V):
    delta_Q = delta_V * C       # change in charge
    avg_A = delta_Q / delta_T - LDOCurrent

    E = delta_Q * Vreg #avg_A * Vreg * delta_T

    print "Energy: %12.9f J   Avg Current: %12.9f A"%(E, avg_A)

    return (E, avg_A)

def parse(filename):
    '''
    Reads a CSV datafile saved by a Tektronix MSO 3034

    returns a NumPy array of [time, [ch1,] [ch2,] [ch3,] [ch4]]
    '''
    dataFile = open(filename, 'r')
    headerLines = 15
    while headerLines != 0:
        headerLines -= 1
        dataFile.readline()

    return np.array([map(float, line.split(','))
                     for line in dataFile.readlines()[:-1]])

def mine(data, voltageCol=1, pinCol=2):
    '''
    Finds delta T and delta V from a data set.
    data - NumPy array like [time, ch1, ...]
    voltageCol - column of 'data' that holds capacitor voltages
    pinCol - column of 'data' that holds pin0.1 values
    '''
    HIGH = 1.0                  # logic high threshold
    LOW = 0.5                   # logic low threshold
    
    time = data[:,0]
    pin1 = data[:,pinCol]
    voltage = data[:,voltageCol]

    idx = pin1.shape[0] - 1     # move to end of array
    
    while pin1[idx] <= HIGH:
        idx -= 1
    T2 = time[idx]
    V2 = np.average(voltage[idx-5:idx+5])
    
    while pin1[idx] >= LOW:
        idx -= 1
    T1 = time[idx]
    V1 = np.average(voltage[idx-5:idx+5])

    return (T2-T1, V1-V2)

def readFiles(basePath):
    '''
    Reads all the tek*.csv files in a given directory, and writes the
    analyzed contents to the file 'outputName'
    basePath - the directory that holds input data files
    outputName - where to case space delimited data with the columns
        Sample dT dV Energy Current
    '''
    files = os.listdir(basePath)
    files = filter(lambda a: 'tek' in a, files)

    i = 1
    for fileName in files:
        dT, dV = mine(parse(basePath + '/' + fileName))
        E, A = calc(dT, dV)
        i += 1
        yield((i, dT, dV, E, A))

    return

def readSocket():
    '''
    Gets data from the o-scope over the network.
    '''
    tek = teksocket.TekSocket('10.0.0.9')

    i = 1
    while not raw_input("(Sample %2d) Press Enter to collect more data."%i):
        sample = tek.getData([1,2])
        dT, dV = mine(sample)
        E, A = calc(dT, dV)
        yield((i, dT, dV, E, A))
        i += 1

    return

def getSamples(sampleSource, outputName = 'output.dat'):
    '''
    Generic way to analyze a bunch of samples.
    Returns an array [sample#, dT, dV, E, A]
    sampleSource - iterator that will return arrays [time, voltage, pin1]
    outputName
    '''
    f = open(outputName, 'w')
    f.write('Sample dT dV Energy Current\n')
    
    data = []
    samples = sampleSource()
    for i, dT, dV, E, A in samples:
        f.write('%d %.9f %.9f %.9f %.9f\n'%(i, dT, dV, E, A))
        data.append((i, dT, dV, E, A))
    data = np.array(data)
        
    f.close()
    return data

def loadDat(filename):
    datFile = open(filename, 'r')
    header = datFile.readline()
    rawData = map(str.split, datFile.readlines())
    data = np.array([map(float, row) for row in rawData])
    datFile.close()
    return data
    
if __name__ == "__main__":
    getSamples(readSocket, sys.argv[1])
