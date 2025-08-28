#!/usr/bin/env python3

import sys
import io
import fcntl
import time
import rospy

from i2c_for_esp32 import WirePacker  # pip3 install i2c-for-esp32
from std_msgs.msg import UInt16
from std_msgs.msg import Float32
from std_msgs.msg import Float32MultiArray

I2C_SLAVE = 0x0703

if sys.hexversion < 0x03000000:
   def _b(x):
      return x
else:
   def _b(x):
      return x.encode('latin-1')


class I2C:
   def __init__(self, device=0x42, bus=0):
      self.fr = io.open("/dev/i2c-"+str(bus), "rb", buffering=0)
      self.fw = io.open("/dev/i2c-"+str(bus), "wb", buffering=0)
      # set device address
      fcntl.ioctl(self.fr, I2C_SLAVE, device)
      fcntl.ioctl(self.fw, I2C_SLAVE, device)

   def write(self, data):
      if type(data) is list:
         data = bytearray(data)
      elif type(data) is str:
         data = _b(data)
      self.fw.write(data)

   def read(self, count):
      return self.fr.read(count)

   def close(self):
      self.fw.close()
      self.fr.close()




i2c_right = I2C(device=0x42,bus=0)
i2c_left = I2C(device=0x42,bus=5)

eye_status = 0
look_at = 0.0
v = 0.0
a = 0.0
d = 0.0

# lid_y = 0.0
# lid_d = 0.0  
# iris_v = 0.0
# changer_v = 0.0
# switch_flag = True

def send_eye_status(data):
   sent_str = str(data)
   rospy.loginfo("sent_str: %s", sent_str)
   packer = WirePacker(buffer_size=len(sent_str) + 8)
   for s in sent_str:
      packer.write(ord(s))
   packer.end()
   if packer.available():
      packet = packer.buffer[:packer.available()]
      try:
         i2c_right.write(packet)
         i2c_left.write(packet)
      except OSError as e:
         print(e)

# def sub_lid_y_cb(msg):
#    global lid_y
#    lid_y = msg.data

# def sub_lid_d_cb(msg):
#    global lid_d
#    lid_d = msg.data

# def sub_iris_v_cb(msg):
#    global iris_v
#    iris_v = msg.data

# def sub_changer_v_cb(msg):
#     global changer_v
#     changer_v = msg.data

def sub_eye_status_cb(msg):
   global eye_status
   eye_status = msg.data
   # switch_flag = False
#    if eye_status > 0:
#       switch_flag = True
#       if eye_status == 7:
#          switch_flag = False


def sub_vad_cb(msg):
   global v, a, d
   if len(msg.data) >= 3:
       v = msg.data[0]
       a = msg.data[1]
       d = msg.data[2]
        
def eye(event):
   send_eye_status(f"V{v:.2f}")
   send_eye_status(f"A{a:.2f}")
   send_eye_status(f"D{d:.2f}")
   send_eye_status(f"X{look_at:.2f}")
   # print(switch_flag)
   # if switch_flag:
   #    send_eye_status(f"S{eye_status}")
   # else:
   #    # send_eye_status(f"X{look_at:.2f}")
   #    send_eye_status(f"L{lid_y:.2f}")
   #    send_eye_status(f"D{lid_d:.2f}")
   #    send_eye_status(f"V{iris_v:.2f}")
   #    send_eye_status(f"C{changer_v:.2f}")
            

def sub_look_at_cb(msg):
   global look_at
   look_at = msg.data
   # rospy.loginfo("look_at: %f", look_at)
   # time.sleep(0.1)



if __name__ == '__main__':
   rospy.init_node('eye_status_to_I2C')
   rospy.Subscriber('/eye_status',UInt16,sub_eye_status_cb)
   rospy.Subscriber('/look_at',Float32,sub_look_at_cb)
   # rospy.Subscriber("lid_y", Float32, sub_lid_y_cb)
   # rospy.Subscriber('/lid_d', Float32, sub_lid_d_cb)
   # rospy.Subscriber('/iris_v', Float32, sub_iris_v_cb)
   # rospy.Subscriber("changer_v", Float32, sub_changer_v_cb)
   rospy.Subscriber('/vad', Float32MultiArray, sub_vad_cb) 
   rospy.Timer(rospy.Duration(0.1), eye)
   rospy.spin()

# eye_status = 0
# while True:
#     eye_status += 1
#     send_eye_status(eye_status)
#     time.sleep(5.0)
#     if eye_status > 6:
#         eye_status = 0

