import luxury
import cv2, numpy as np
from dronekit import connect

class Webcam(luxury.Headup):
    """docstring for Webcam"""
    def capture(self):
        # vehicle = connect('/dev/ttyUSB0', baud=57600, wait_ready=True)

        cap = cv2.VideoCapture(0)
        alt, speed, head = 0, 0, 0

        while(True):
            # Capture frame-by-frame
            ret, frame = cap.read()
            self.lux_apply_HUD(frame, {'alt': alt, 'speed': speed, 'heading': head, 'pitch': 0, 'roll': 30})
            alt += .15
            speed += 1
            head += 3
            # Our operations on the frame come here
            # Display the resulting frame
            cv2.imshow('frame',frame)
            # break;
            if cv2.waitKey(100) & 0xFF == ord('q'):
                break

        # When everything done, release the capture
        cap.release()
        cv2.destroyAllWindows()

if __name__ == '__main__':
    wc = Webcam();
    wc.capture()