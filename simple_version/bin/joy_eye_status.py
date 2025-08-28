# #!/usr/bin/env python3

# import rospy
# from std_msgs.msg import UInt16
# from std_msgs.msg import Float32
# from sensor_msgs.msg import Joy
# pub = rospy.Publisher('eye_status',UInt16,queue_size=1)
# pub_look = rospy.Publisher('look_at',Float32,queue_size=1)

# eye_status = 0
# look_at = 0.0

# def joy_cb(msg):
#     global eye_status
#     global look_at
#     if len(msg.buttons)> 0:
#         if msg.buttons[0] == 1:
#             eye_status = 1
#         elif msg.buttons[1] == 1:
#             eye_status = 2
#         elif msg.buttons[2] == 1:
#             eye_status = 3
#         elif msg.buttons[3] == 1:
#             eye_status = 4
#         elif msg.buttons[4] == 1:
#             eye_status = 5
#         elif msg.buttons[5] == 1:
#             eye_status = 6
#         elif msg.buttons[6] == 1:
#             eye_status = 7
#         else:
#             eye_status = 0
#     else:
#         eye_status = 0
#     pub.publish(eye_status)

#     if len(msg.axes) >0:
#         look_at = float(msg.axes[0])*2.0+2.0
#         print(look_at)
#     pub_look.publish(look_at)


# if __name__ == '__main__':
#     rospy.init_node('eye_st_from_joy')
#     rospy.Subscriber('/quadrotor/joy',Joy,joy_cb)
#     rospy.spin()

#!/usr/bin/env python3

import rospy
from std_msgs.msg import UInt16, Float32
from sensor_msgs.msg import Joy
#!/usr/bin/env python3

# import rospy
# from std_msgs.msg import UInt16
# from std_msgs.msg import Float32
# from sensor_msgs.msg import Joy
# pub = rospy.Publisher('eye_status',UInt16,queue_size=1)
# pub_look = rospy.Publisher('look_at',Float32,queue_size=1)

# eye_status = 0
# look_at = 0.0

# def joy_cb(msg):
#     global eye_status
#     global look_at
#     if len(msg.buttons)> 0:
#         if msg.buttons[0] == 1:
#             eye_status = 1
#         elif msg.buttons[1] == 1:
#             eye_status = 2
#         elif msg.buttons[2] == 1:
#             eye_status = 3
#         elif msg.buttons[3] == 1:
#             eye_status = 4
#         elif msg.buttons[4] == 1:
#             eye_status = 5
#         elif msg.buttons[5] == 1:
#             eye_status = 6
#         elif msg.buttons[6] == 1:
#             eye_status = 7
#         else:
#             eye_status = 0
#     else:
#         eye_status = 0
#     pub.publish(eye_status)

#     if len(msg.axes) >0:
#         look_at = float(msg.axes[0])*2.0+2.0
#         print(look_at)
#     pub_look.publish(look_at)


# if __name__ == '__main__':
#     rospy.init_node('eye_st_from_joy')
#     rospy.Subscriber('/quadrotor/joy',Joy,joy_cb)
#     rospy.spin()

# Publisher定義
pub_eye_status = rospy.Publisher('/eye_status', UInt16, queue_size=1)
pub_look = rospy.Publisher('/look_at', Float32, queue_size=1)
pub_lid_y = rospy.Publisher('/lid_y', Float32, queue_size=1)
pub_lid_d = rospy.Publisher('/lid_d', Float32, queue_size=1)
pub_iris_v = rospy.Publisher('/iris_v', Float32, queue_size=1)
pub_changer_v = rospy.Publisher('/changer_v', Float32, queue_size=1)

def joy_cb(msg):
    eye_status = 0

    # ボタンで eye_status を変更
    if len(msg.buttons) > 0:
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
    pub_eye_status.publish(eye_status)

    # 左スティック左右: axes[0]、上下: axes[1]
    # 右スティック左右: axes[3]、上下: axes[4]
    # ジョイスティックの傾き（-1.0〜1.0）を特定変数に割り当てる

    if len(msg.axes) > 4:
        # ボタンを押している間にだけ、対応する値を送信
        if msg.buttons[7] == 1:
            pub_lid_y.publish(msg.axes[1])  # 左スティック上下
        if msg.buttons[8] == 1:
            pub_lid_d.publish(msg.axes[0])  # 左スティック左右
        if msg.buttons[9] == 1:
            pub_iris_v.publish(msg.axes[3])  # 右スティック左右
        if msg.buttons[10] == 1:
            pub_changer_v.publish(msg.axes[4])  # 右スティック上下

        # 常時 look_at（目の向き）も送信（必要に応じて変更）
        look_at = float(msg.axes[0]) * 2.0 + 2.0
        pub_look.publish(look_at)

if __name__ == '__main__':
    rospy.init_node('eye_st_from_joy')
    rospy.Subscriber('/quadrotor/joy', Joy, joy_cb)
    rospy.spin()
