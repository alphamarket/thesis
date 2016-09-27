import vision_utils as vu
from inspect import ismethod
import cv2, numpy as np, threading

class Headup(object):
    """docstring for Headup"""

    __margin_rate = .71
    __padding_rate = .1

    def lux_apply_HUD(self, frame, data, color = [0, 255, 0]):
        """ Applies the Head Up Displays to the frame """
        c = vu.center_of_frame(frame);

        half_width = 5

        threads = [];

        for name in dir(self):
            attribute = getattr(self, name)
            if ismethod(attribute) and name.startswith('_Headup__apply_'):
                threads.append(threading.Thread(target = attribute, args=[frame, c, data, color, half_width]));
                threads[-1].start()

        for t in threads: t.join();

    def __apply_altitude_bar(self, frame, c, data, color, half_width):
        vu.validate_hash(data, ['alt']);

        half_width = 5;
        top        = int(c[0] + (frame.shape[0] - c[0]) * self.__margin_rate);
        bottom     = int(c[0] - (frame.shape[0] - c[0]) * self.__margin_rate);
        right      = int(c[1] + (frame.shape[1] - c[1]) * self.__margin_rate) + 10;

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
            blockify = False
            # show between alt. flag
            if(alts[alt_counter] < float(data['alt']) < alts[alt_counter - 1] and alt_plotted_rate > 0):
                blockify = (alt_plotted_rate - .2 < (data['alt'] - alts[alt_counter]) <= alt_plotted_rate)
                alt_plotted_rate -= .2
            # plot the numbers
            if(counter % 6 == 0):
                w *= 2
                alt = str(alts[alt_counter])
                if(abs(float(data['alt'] - alts[alt_counter])) <= 1e-1 and alt_plotted_rate > 0) : blockify = True
                cv2.putText(frame, alt, (right + w * 4, i + w / 2), cv2.FONT_HERSHEY_COMPLEX, .7, color, 2)
                alt_counter += 1;
            # plot the horizontal step indicators
            frame[i-1:i+1, right-w:right+w] = color;
            # display block indicator
            if(blockify):
                # dis-allow to mark again
                alt_plotted_rate = -1;
                frame[i-10:i+10, right-10:right+10] = color;
            counter += 1;

    def __apply_speed_bar(self, frame, c, data, color, half_width):
        vu.validate_hash(data, ['speed']);

        top        = int(c[0] + (frame.shape[0] - c[0]) * self.__margin_rate);
        bottom     = int(c[0] - (frame.shape[0] - c[0]) * self.__margin_rate);
        left       = int(c[1] - (frame.shape[1] - c[1]) * self.__margin_rate)-10;

        if(top < bottom): top, bottom = bottom, top;

        # create label
        frame[bottom-35:bottom-15,left - 10 * half_width:left + 5] = color;
        cv2.putText(frame,"SPEED", (left - 50, bottom-20), cv2.FONT_HERSHEY_COMPLEX, .5, 0, 1)

        # draw vertical lines
        counter, speed_counter, speed_plotted_rate = 1, 1, 1;
        # create speed range list, with data['speed'] in middel
        speeds = list(reversed(range(int(int(data['speed']) - 3), int(int(data['speed']) + 4))));

        for i in xrange(bottom, top, 10):
            w = half_width;
            blockify = False
            # show between speed. flag
            if(speeds[speed_counter] < float(data['speed']) < speeds[speed_counter - 1] and speed_plotted_rate > 0):
                blockify = (speed_plotted_rate - .2 < (data['speed'] - speeds[speed_counter]) <= speed_plotted_rate)
                speed_plotted_rate -= .2
            # plot the numbers
            if(counter % 6 == 0):
                w *= 2
                speed = str(speeds[speed_counter])
                cv2.putText(frame, speed, (left - w * 5, i + w / 2), cv2.FONT_HERSHEY_COMPLEX, .7, color, 2)
                if(abs(float(data['speed'] - speeds[speed_counter])) <= 1e-1 and speed_plotted_rate > 0) : blockify = True
                speed_counter += 1;
            # plot the horizontal step indicators
            frame[i-1:i+1, left-w:left+w] = color;
            # display block indicator
            if(blockify):
                # dis-allow to mark again
                speed_plotted_rate = -1;
                frame[i-10:i+10, left-10:left+10] = color;
            counter += 1;

    def __apply_heading(self, frame, c, data, color, half_width):
        vu.validate_hash(data, ['heading']);

        top        = int(c[0] + (frame.shape[0] - c[0]) * self.__margin_rate);
        bottom     = int(c[0] - (frame.shape[0] - c[0]) * self.__margin_rate) - 20 - half_width;
        left       = int(c[1] - (frame.shape[1] - c[1]) * self.__margin_rate) + 55;
        right      = int(c[1] + (frame.shape[1] - c[1]) * self.__margin_rate) - 55;

        if(right < left): right, left = left, right;

        # create label
        frame[bottom-35:bottom-15, c[0] + 7 * half_width:c[0] + 23 * half_width] = color;
        cv2.putText(frame,"HEADING", (c[0] + 8 * half_width, bottom - half_width * 4), cv2.FONT_HERSHEY_COMPLEX, .5, 0, 1)

        # draw vertical lines
        counter, heading_counter, heading_plotted_rate = 1, 1, 1;
        head_range = [int((data['heading']-30)), int((data['heading']+40))]
        # create heading range list, with data['heading'] in middel
        headings = list(reversed(range(min(head_range), max(head_range), 10)));
        for i in xrange(left, right, 10):
            w = half_width;
            blockify = False
            # show between heading. flag
            if(headings[heading_counter] < float(data['heading']) < headings[heading_counter - 1] and heading_plotted_rate > 0):
                blockify = (heading_plotted_rate - .2 < (data['heading'] - headings[heading_counter]) <= heading_plotted_rate)
                heading_plotted_rate -= .2
            # plot the numbers
            if(counter % 6 == 0):
                w *= 2
                heading = str(int(headings[heading_counter] % 360))
                cv2.putText(frame, heading, (i - 3 * half_width / 2, bottom + w * 3), cv2.FONT_HERSHEY_COMPLEX, .7, color, 2)
                if(abs(float(data['heading'] - headings[heading_counter])) <= 1e-1 and heading_plotted_rate > 0) : blockify = True
                heading_counter += 1;
            # plot the horizontal step indicators
            frame[bottom-w:bottom+w, i-1:i+1] = color;
            # display block indicator
            if(blockify):
                # dis-allow to mark again
                heading_plotted_rate = -1;
                frame[bottom-10:bottom+10,i-10:i+10] = color;
            counter += 1;

    def __apply_focus_area(self, frame, c, data, color, half_width):
        top        = int(c[0] + (frame.shape[0] - c[0]) * self.__margin_rate);
        bottom     = int(c[0] - (frame.shape[0] - c[0]) * self.__margin_rate);
        left       = int(c[1] - (frame.shape[1] - c[1]) * self.__margin_rate) + 20;
        right      = int(c[1] + (frame.shape[1] - c[1]) * self.__margin_rate) - 20;

        if(top < bottom): top, bottom = bottom, top;

        # left/right lines
        frame[bottom:top,left-1:left+1] = color;
        frame[bottom:top,right-1:right+1] = color;
        # small lines
        frame[bottom-1:bottom+1,left:left+20] = color;
        frame[bottom-1:bottom+1,right-20:right] = color;
        frame[top-1:top+1,left:left+20] = color;
        frame[top-1:top+1,right-20:right] = color;
        frame[c[0]-1:c[0]+1,left:left+20] = color;
        frame[c[0]-1:c[0]+1,right-20:right] = color;

    def __apply_pitch_angle(self, frame, c, data, color, half_width):
        vu.validate_hash(data, ['pitch','roll']);
        top        = int(c[0] + (frame.shape[0] - c[0]) * self.__margin_rate);
        bottom     = int(c[0] - (frame.shape[0] - c[0]) * self.__margin_rate);
        left       = int(c[1] - (frame.shape[1] - c[1]) * self.__margin_rate) + 20;
        right      = int(c[1] + (frame.shape[1] - c[1]) * self.__margin_rate) - 20;

        pitch_range = [int((data['pitch']-10)), int((data['pitch']+10+1))]
        pitches = list(reversed(range(pitch_range[0], pitch_range[1], 5)));
        length = 25 * half_width;
        for p in pitches:
            x1 = c[0]+15*p-1
            y1 = c[1] - half_width * 20
            x2 = x1 + length * np.cos(data['roll'] * np.pi / 180)
            y2 = y1 + length * np.sin(data['roll'] * np.pi / 180)
            cv2.line(frame, (int(x1), int(y1)), (int(x2), int(y2)), color)

            # frame[c[0]+15*p-1:c[0]+15*p+1, c[1] - half_width * 20:c[1] - half_width * 5] = color;
            # frame[c[0]+15*p-1:c[0]+15*p+1, c[1] + half_width * 5:c[1] + half_width * 20] = color;
            # if(p < 0):
            #     frame[c[0]+10*p-1:c[0]+15*p+1, c[1] + half_width * 20:c[1] + half_width * 20+1] = color;
            # else:
            #     frame[c[0]+15*p-1:c[0]+10*p+1, c[1] + half_width * 20:c[1] + half_width * 20+1] = color;
        # center line
        # frame[c[0]-1:c[0]+1,c[1] - half_width * 30:c[1] + half_width * 30] = color;

