import luxury
import cv2, numpy as np
from dronekit import connect

class Webcam(luxury.Headup):
    """docstring for Webcam"""
    def capture(self):
        # vehicle = connect('/dev/ttyUSB0', baud=57600, wait_ready=True)

        cap = cv2.VideoCapture(0)
        alt, speed, head = 0, 0, 0
        roll, pitch, yaw = 0, 0, 0
        k_roll = 1

        while(True):
            # Capture frame-by-frame
            ret, frame = cap.read()
            frame = cv2.flip(frame, 1)
            frame = cv2.resize(frame, (640, 480))
            alt += .015 if alt < 2.3 else 0
            speed += .01
            head += .3
            roll += k_roll
            pitch += k_roll
            yaw += k_roll
            if(roll > 15 or roll < -15): k_roll *= -1;

            # Our operations on the frame come here
            # Display the resulting frame
            hud = frame.copy()
            self.lux_apply_HUD(hud, {'alt': alt, 'speed': speed, 'heading': head, 'pitch': pitch, 'roll': roll, 'yaw': yaw, 'headup': 1})
            cv2.imshow('frame2',hud)
            # break;
            if cv2.waitKey(1) & 0xFF == ord('q'):
                break

        # When everything done, release the capture
        cap.release()
        cv2.destroyAllWindows()

if __name__ == '__main__':
    wc = Webcam();
    wc.capture()