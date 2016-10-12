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
            if(not self.caps[-1].isOpened()):
                raise Exception("Unable to open camera# `%s`" %str(p));

    def read(self):
        frames = [];
        for cap in self.caps: cap.grab();
        for cap in self.caps:
            flag, frame = cap.retrieve();
            if not flag: frames.append(None);
            frames.append(frame);
        return frames;

    def __del__(self):
        for cap in self.caps: cap.release();