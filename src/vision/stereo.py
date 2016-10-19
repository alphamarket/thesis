import cv2, time, os
from multiprocessing import Pool

class Stereo(object):
    """docstring for Stereo"""
    def __init__(self, ports):
        super(Stereo, self).__init__()
        self.caps = []
        assert len(ports) == 2, 'Expecting stereo camera ports as inputs!'
        for p in ports:
            self.caps.append(cv2.VideoCapture(p))
            cap = self.caps[-1];
            cap.set(cv2.cv.CV_CAP_PROP_FRAME_HEIGHT,240) # CV_CAP_PROP_FRAME_WIDTH
            cap.set(cv2.cv.CV_CAP_PROP_FRAME_WIDTH,320) # CV_CAP_PROP_FRAME_WIDTH
            cap.set(cv2.cv.CV_CAP_PROP_FOURCC, cv2.cv.CV_FOURCC('M','J','P','G'))

            if(not self.caps[-1].isOpened()):
                raise Exception("Unable to open camera# `%s`" %str(p));

    def read(self):
        grabed = [];
        frames = [];
        for cap in self.caps: grabed.append(cap.grab());
        for idx, cap in enumerate(self.caps):
            if not grabed[idx]: print 'Nothing grabed'; continue;
            flag, frame = cap.retrieve();
            if not flag: frames.append(None);
            frames.append(frame);
            print frame.shape
        return frames;

    def __del__(self):
        for cap in self.caps: cap.release();