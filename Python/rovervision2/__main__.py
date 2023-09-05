#!/usr/bin/env python
from __future__ import print_function
import argparse
#import numpy as np
import sys
import os

import cv2

from pipeline.videoprocessinglayers import VideoProcessingLayer

from pipeline.videoprocessinglayers.roverhud import RoverHUD
from pipeline.videoprocessinglayers.display import VideoDisplay
#from pipeline.videoprocessinglayers.writer import VideoWriter

class RoverVision:

  def __init__(self, cap):
    self.ctx = {}
    self.cap = cap

    self.hud = RoverHUD(640,480)    
    self.layers = [      
      self.hud, # 4
      VideoDisplay(), # 5
        # 6
       #self.video_writer
    ]

    # Default layer state
    layer_states = [      
      True,   
      True,   
    ]
    
    for l in range(len(layer_states)):
      self.layers[l].enable(layer_states[l])
      self.hud.set_pipeline_stage_state(l, layer_states[l] )

    for l in self.layers:
      l.setup(self.ctx)

  def on_frame(self,frame):          
      self.ctx["frame"] = frame
      rows,cols,channels = self.ctx["frame"].shape
      self.ctx["rows"] = rows
      self.ctx["cols"] = cols      
      self.run_pipeline()

  def run_pipeline(self):    
      for l in self.layers:
        if l.is_enabled():
          l.process(self.ctx)

      key_pressed = cv2.waitKey(1)
      if key_pressed >= ord('1') and key_pressed<=ord('9'):        
        layer = key_pressed-ord('1')
        if(layer < len(self.layers)):
          new_state = not self.layers[layer].is_enabled()          
          self.layers[layer].enable( new_state )
          self.hud.set_pipeline_stage_state(layer, new_state)
     # elif key_pressed == ord('r'):
     #   #self.video_writer.start_recording()
     # elif key_pressed == ord('t'):
     #   #self.video_writer.stop_recording()


  def run(self):
    while True:
        ret, frame = cap.read()
        if not ret:
            break
        self.on_frame(frame)

        if cv2.waitKey(1) & 0xFF == ord('q'):
            break
    cv2.destroyAllWindows()    
    for l in self.layers:
        l.release(self.ctx)

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Rover Vision')
    #parser.add_argument('--vehicle_id', type=str, default=DEFAULT_ROVER_ID, help='rover identifier')    
    parser.add_argument('--port', type=int, default=5004, help='UDP video port' )
    args, unknown = parser.parse_known_args()
    udp_source = f"udpsrc port={args.port} ! application/x-rtp, encoding-name=JPEG,payload=26 ! " \
              "rtpjpegdepay ! jpegparse ! jpegdec ! videoconvert ! appsink sync=false"
    # Create a VideoCapture object with the GStreamer pipeline
    cap = cv2.VideoCapture(udp_source, cv2.CAP_GSTREAMER)

    if not cap.isOpened():
        print("Error: Could not open video stream.")
        error_msg = cv2.error()
        print("Error message:", error_msg)
        exit()

    vision = RoverVision( cap=cap)
    vision.run()