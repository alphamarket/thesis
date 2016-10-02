from math import *
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
                threads[-1].daemon = True
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
                cv2.putText(frame, speed, (left - w * 6, i + w / 2), cv2.FONT_HERSHEY_COMPLEX, .7, color, 2)
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
        data['heading'] %= 360

        top        = int(c[0] + (frame.shape[0] - c[0]) * self.__margin_rate);
        bottom     = int(c[0] - (frame.shape[0] - c[0]) * self.__margin_rate) - 20 - half_width;
        left       = int(c[1] - (frame.shape[1] - c[1]) * self.__margin_rate) + 55;
        right      = int(c[1] + (frame.shape[1] - c[1]) * self.__margin_rate) - 55;

        if(right < left): right, left = left, right;

        # create label
        frame[top + 4 * half_width:top + 8 * half_width, 0:80] = color;
        cv2.putText(frame,"HEADING: ", (0, top + 7 * half_width), cv2.FONT_HERSHEY_COMPLEX, .5, 0, 1)

        direction = "";

        for k in [
                    (0, 45, 'N', 'NE'), (45, 90, 'NE', 'E'),
                    (90, 135, 'E', 'SE'), (135, 180, 'SE', 'S'),
                    (180, 225, 'S', 'SW'), (225, 270, 'SW', 'W'),
                    (270, 315, 'W', 'NW'), (315, 360, 'NW', 'N'),
                ]:
            if(k[0] < data['heading'] < k[1]):
                direction += '%s/%s' %(k[2], k[3])
                break;

        cv2.putText(frame, "%.2f %s" %(data['heading'], direction), (85, top + 7 * half_width), cv2.FONT_HERSHEY_COMPLEX, .5, color, 1)


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

    def __apply_roll_pitch_angle(self, frame, c, data, color, half_width):
        vu.validate_hash(data, ['pitch','roll', 'yaw', 'headup']);
        top        = int(c[0] + (frame.shape[0] - c[0]) * self.__margin_rate);
        bottom     = int(c[0] - (frame.shape[0] - c[0]) * self.__margin_rate);
        left       = int(c[1] - (frame.shape[1] - c[1]) * self.__margin_rate) + 20;
        right      = int(c[1] + (frame.shape[1] - c[1]) * self.__margin_rate) - 20;

        def draw_circle(im, c, r):
            for i in xrange(0, 360):
                x = c[0] + r * np.cos(i * np.pi / 180);
                y = c[1] + r * np.sin(i * np.pi / 180);
                im[x-1:x+1, y-1:y+1] = [0, 255, 0]

        def tangent2(c, roll, r, x):
            theta = roll * np.pi / 180;
            x0 = c[0] - r * np.sin(theta);
            y0 = c[1] + r * np.cos(theta);
            f = (c[0]**2 + c[1]**2 - r**2);
            y = ((c[0]-x0) * x + c[0]*x0 + c[1]*y0 - f)/((y0 - c[1]) if roll not in [90, 270] else 1)
            return y

        def location(p, w, theta):
            return (int(p[1] + w * sin(theta)), int(p[0] + w * cos(theta)))

        def location_perpendicular(p, w, theta):
            m = pi / 2 + theta;
            return (int(p[0] + w * sin(m)), int(p[1] + w * cos(m)))

        def make_point(im, p, t = 2, color = [0,0,255]):
            im[p[1]-t:p[1]+t, p[0]-t:p[0]+t] = color

        def normdegree(angle):
            # reduce the angle
            angle =  angle % 360;
            # force it to be the positive remainder, so that 0 <= angle < 360
            angle = (angle + 360) % 360;
             # force into the minimum absolute value residue class, so that -180 < angle <= 180
            if (angle > 180): angle -= 360;
            return angle

        roll = normdegree(data['roll'])
        yaw = normdegree(data['yaw'])

        pitch_step_size = 5;
        pitch = normdegree(data['pitch'])
        pitch_step = int(pitch_step_size * round( pitch / pitch_step_size ));

        theta = roll * pi / 180 + pi / 2;

        counter = 0;
        width = 30 * half_width
        radius = 30 * half_width;
        r = 0;
        draw_circle(frame, c, 4 * half_width)
        p0 = np.asarray([c[0] - r * np.sin(theta), c[1] + r * np.cos(theta)]);

        point_sets = {
            'start': location(p0, -width, theta),
            'start_mid': location(p0, -width/4, theta),
            'end_mid': location(p0, +width/4, theta),
            'end': location(p0, +width, theta),
        }

        for s, e in [('start', 'start_mid'), ('end_mid', 'end')]:
            cv2.line(frame,
                point_sets[s],
                point_sets[e],
                color,
                2);
        k = 5 * half_width;
        for p in ['start', 'end']:
            cv2.line(frame,
                point_sets[p],
                location_perpendicular(point_sets[p], np.sign(data['headup']) * k, theta),
                color,
                2);

        frame[top + 4 * half_width:top + 8 * half_width, 200:250] = color;
        cv2.putText(frame, "ROLL: ", (200, top + 7 * half_width), cv2.FONT_HERSHEY_COMPLEX, .5, 0, 1)
        cv2.putText(frame, "%.1f" %roll, (255, top + 7 * half_width), cv2.FONT_HERSHEY_COMPLEX, .5, color, 1)

        frame[top + 4 * half_width:top + 8 * half_width, 320:380] = color;
        cv2.putText(frame, "PITCH: ", (320, top + 7 * half_width), cv2.FONT_HERSHEY_COMPLEX, .5, 0, 1)
        cv2.putText(frame, "%.1f" %pitch, (385, top + 7 * half_width), cv2.FONT_HERSHEY_COMPLEX, .5, color, 1)

        frame[top + 4 * half_width:top + 8 * half_width, 450:495] = color;
        cv2.putText(frame, "YAW: ", (450, top + 7 * half_width), cv2.FONT_HERSHEY_COMPLEX, .5, 0, 1)
        cv2.putText(frame, "%.1f" %yaw, (500, top + 7 * half_width), cv2.FONT_HERSHEY_COMPLEX, .5, color, 1)

