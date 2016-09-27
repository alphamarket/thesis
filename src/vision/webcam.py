import cv2, numpy as np
import luxury

class Webcam(luxury.Headup):
    """docstring for Webcam"""
    def capture(self):
        cap = cv2.VideoCapture(0)
        alt = 0

        while(True):
            ret, frame = cap.read()
            # Capture frame-by-frame
            self.lux_apply_HUD(frame, {'alt': alt})
            alt += .15
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