"""
ldr.py
Display analog data from Arduino using Python (matplotlib)
Author: Mahesh Venkitachalam
Website: electronut.in
"""

import sys, serial, argparse
import numpy as np
# from time import sleep
from collections import deque
import time
import copy
import collections
import matplotlib.pyplot as plt 
import matplotlib.animation as animation

    
# plot class
class AnalogPlot:
  # constr
  def __init__(self, strPort, maxLen):
      # open serial port
      self.ser = serial.Serial(strPort, 115200)

    #   self.ax = deque([0.0]*maxLen)
    #   self.ay = deque([0.0]*maxLen)
      self.data = []
      for i in range(maxLen):   # give an array for each type of data and store them in a list
          self.data.append(collections.deque([0] * maxLen, maxlen=maxLen))
        
      self.maxLen = maxLen
      self.previousTimer = 0

  # add to buffer
#   def addToBuf(self, buf, val):
#       if len(buf) < self.maxLen:
#           buf.append(val)
#       else:
#           buf.pop()
#           buf.appendleft(val)

#   # add data
#   def add(self, data):
#       assert(len(data) == 2)
#       self.addToBuf(self.ax, data[0])
#       self.addToBuf(self.ay, data[1])


  def getSerialData(self, frame, lines, lineValueText, lineLabel, timeText):
        currentTimer = time.perf_counter()
        self.plotTimer = int((currentTimer - self.previousTimer) * 1000)     # the first reading will be erroneous
        self.previousTimer = currentTimer
        timeText.set_text('Plot Interval = ' + str(self.plotTimer) + 'ms')
        privateData = copy.deepcopy(self.rawData[:])    # so that the 3 values in our plots will be synchronized to the same sample time
        for i in range(self.numPlots):
            data = privateData[(i*self.dataNumBytes):(self.dataNumBytes + i*self.dataNumBytes)]
            value,  = struct.unpack(self.dataType, data)
            self.data[i].append(value)    # we get the latest data point and append it to our array
            lines[i].set_data(range(self.plotMaxLength), self.data[i])
            lineValueText[i].set_text('[' + lineLabel[i] + '] = ' + str(value))



  # update plot
  def update(self, frameNum, lines, lineValueText, lineLabel, timeText):
      
      try:
          line = self.ser.readline()
          print(line)
          if (len(line)==0):
              return
          

          currentTimer = time.perf_counter()
          self.plotTimer = int((currentTimer - self.previousTimer) * 1000)     # the first reading will be erroneous
          self.previousTimer = currentTimer
          timeText.set_text('Plot Interval = ' + str(self.plotTimer) + 'ms')
          data = [val for val in line.split()]
          if (str(data[0],'utf-8')=='#'):
            divisor = int(data[1])  
            for i in range(len(data)-3):
                value = float(int(data[i+2]) / divisor)
              #   print (lines)
                self.data[i].append(value)
                lines[i].set_data(range(self.maxLen), self.data[i])
                lineValueText[i].set_text('[' + lineLabel[i] + '] = ' + str(value))

          else:
              print("ignoring:" + str(data[0],'utf-8') + ":" + str(type(data[0])))


      except KeyboardInterrupt:
          print('exiting')
      
        #   lines[0].set_data(range(self.maxLen), self.ay)
      
    #   privateData = copy.deepcopy(self.rawData[:])    # so that the 3 values in our plots will be synchronized to the same sample time
    #     for i in range(self.numPlots):
    #         data = privateData[(i*self.dataNumBytes):(self.dataNumBytes + i*self.dataNumBytes)]
    #         value,  = struct.unpack(self.dataType, data)
    #         self.data[i].append(value)    # we get the latest data point and append it to our array
    #         lines[i].set_data(range(self.plotMaxLength), self.data[i])
    #         lineValueText[i].set_text('[' + lineLabel[i] + '] = ' + str(value))

      
      
    #   return a0, 

# def update(self, frameNum, a0, a1, lineValueText):
#       try:
#           line = self.ser.readline()
#           data = [float(val) for val in line.split()]
#           # print data
#           if(len(data) == 2):
#               self.add(data)
#               a0.set_data(range(self.maxLen), self.ax)
#               a1.set_data(range(self.maxLen), self.ay)
#       except KeyboardInterrupt:
#           print('exiting')
      
#       return a0, 


  # clean up
  def close(self):
      # close serial
      self.ser.flush()
      self.ser.close()    

# main() function
def main():
  # create parser
  parser = argparse.ArgumentParser(description="LDR serial")
  # add expected arguments
  parser.add_argument('--port', dest='port', required=True)

  # parse args
  args = parser.parse_args()
  
  #strPort = '/dev/tty.usbserial-A7006Yqh'
  strPort = args.port

  print('reading from serial port %s...' % strPort)

  # plot parameters
  analogPlot = AnalogPlot(strPort, 100)

  print('plotting data...')

  # set up animation
  fig = plt.figure(figsize=[15,5])
  ax = plt.axes(xlim=(0, 100), ylim=(-100, 100))
  ax.set_title('Arduino Robot data')
  ax.set_xlabel("time")
#  a0, = ax.plot([], [], label='val1')
#  a1, = ax.plot([], [], label='val2')
#  lineValueText = ax.text(0.50, 0.90, '', transform=ax.transAxes)
#  anim = animation.FuncAnimation(fig, analogPlot.update, 
#                                 fargs=(a0, a1, lineValueText), 
#                                 interval=50)

  lineLabel = ['X', 'Y', 'Z']
  style = ['r-', 'c-', 'b-']  # linestyles for the different plots
  timeText = ax.text(0.70, 0.95, '', transform=ax.transAxes)
  lines = []
  lineValueText = []
  for i in range(len(lineLabel)):
    lines.append(ax.plot([], [], style[i], label=lineLabel[i])[0])
    lineValueText.append(ax.text(0.70, 0.90-i*0.05, '', transform=ax.transAxes))
  anim = animation.FuncAnimation(fig, analogPlot.update, fargs=(lines, lineValueText, lineLabel, timeText), interval=5)    # fargs has to be a tuple
                                

  # show plot
  plt.legend(loc="upper left")
  plt.show()
  
  # clean up
  analogPlot.close()

  print('exiting.')
  

# call main
if __name__ == '__main__':
  main()