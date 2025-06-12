#!/usr/bin/env python3

import rospy
from std_msgs.msg import UInt16
from std_msgs.msg import Float32
from sensor_msgs.msg import Joy
pub = rospy.Publisher('eye_status',UInt16,queue_size=1)
pub_look = rospy.Publisher('look_at',Float32,queue_size=1)

eye_status = 0
look_at = 0.0

def joy_cb(msg):
    global eye_status
    global look_at
    if len(msg.buttons)> 0:
        if msg.buttons[0] == 1:
            eye_status = 1
        elif msg.buttons[1] == 1:
            eye_status = 2
        elif msg.buttons[2] == 1:
            eye_status = 3
        elif msg.buttons[3] == 1:
            eye_status = 4
        elif msg.buttons[4] == 1:
            eye_status = 5
        elif msg.buttons[5] == 1:
            eye_status = 6
        elif msg.buttons[6] == 1:
            eye_status = 7
        else:
            eye_status = 0
    else:
        eye_status = 0
    pub.publish(eye_status)

    if len(msg.axes) >0:
        look_at = float(msg.axes[0])*2.0+2.0
        print(look_at)
    pub_look.publish(look_at)


if __name__ == '__main__':
    rospy.init_node('eye_st_from_joy')
    rospy.Subscriber('/quadrotor/joy',Joy,joy_cb)
    rospy.spin()
