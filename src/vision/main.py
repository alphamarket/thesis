import cv2, numpy as np
from stereo import Stereo

s = Stereo([0, 1])

while(True):
#     # Capture frame-by-frame
    frames = s.read();

#     # Our operations on the frame come here
#     gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

#     # Display the resulting frame
    cv2.imshow('frame',np.hstack(frames))
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cv2.destroyAllWindows()