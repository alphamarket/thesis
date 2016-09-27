import cv2, numpy as np
import vision_utils as vu
from inspect import ismethod

class Headup(object):
    """docstring for Headup"""

    __margin_rate = .71
    __padding_rate = .1

    def lux_apply_HUD(self, frame, data, color = [0, 255, 0]):
        """ Applies the Head Up Displays to the frame """
        c = vu.center_of_frame(frame);

        frame[c[0]-10:c[0]+10,c[1]-10:c[1]+10] = color

        for name in dir(self):
            attribute = getattr(self, name)
            if ismethod(attribute) and name.startswith('_Headup__apply_'):
                attribute(frame, c, data, color);

    def __apply_altitude_bar(self, frame, c, data, color):
        vu.validate_hash(data, ['alt']);

        half_width = 5;
        top        = int(c[0] + (frame.shape[0] - c[0]) * self.__margin_rate);
        bottom     = int(c[0] - (frame.shape[0] - c[0]) * self.__margin_rate);
        right      = int(c[1] + (frame.shape[1] - c[1]) * self.__margin_rate) + 25;

        if(top < bottom): top, bottom = bottom, top;

        # create label
        frame[bottom-35:bottom-15,right - half_width:right + 10 * half_width] = color;
        cv2.putText(frame,"ALT", (right + 7, bottom-20), cv2.FONT_HERSHEY_COMPLEX, .5, 0, 1)

        # draw vertical lines
        counter, alt_counter, alt_plotted_rate = 1, 1, 1;
        # create alt range list, with data['alt'] in middel
        alts = list(reversed(range(int(int(data['alt']) - 3), int(int(data['alt']) + 4))));

        for i in xrange(bottom, top, 10):
            w = half_width;
            # show between alt. flag
            if(alts[alt_counter] < float(data['alt']) < alts[alt_counter - 1] and alt_plotted_rate > 0):
                if(alt_plotted_rate - .2 < (data['alt'] - alts[alt_counter]) <= alt_plotted_rate):
                    # dis-allow to mark again
                    alt_plotted_rate = -1;
                    cv2.putText(frame, "<", (right + w * 9, i + w / 2), cv2.FONT_HERSHEY_COMPLEX, .7, color, 2)
                alt_plotted_rate -= .2
            # plot the numbers
            if(counter % 6 == 0):
                w *= 2
                alt = str(alts[alt_counter])
                if(abs(float(data['alt'] - alts[alt_counter])) <= 1e-1 and alt_plotted_rate > 0) : alt += ' <'; alt_plotted_rate = -1;
                cv2.putText(frame, alt, (right + w * 2, i + w / 2), cv2.FONT_HERSHEY_COMPLEX, .7, color, 2)
                alt_counter += 1;
            # plot the horizontal step indicators
            frame[i-1:i+1, right-w:right+w] = color;
            counter += 1;


