#!/usr/bin/env python

import random
import sys
import time
import sched
from Tkinter import *

cubes = [
    'AAEEGN',
    'ELRTTY',
    'AOOTTW',
    'ABBJOO',
    'EHRTVW',
    'CIMOTU',
    'DISTTY',
    'EIOSST',
    'DELRVY',
    'ACHOPS',
    'HIMNQU',
    'EEINSU',
    'EEGHNW',
    'AFFKPS',
    'HLNNRZ',
    'DEILRX',
]

if __name__ == '__main__':
    random.shuffle(cubes)
    root = Tk()


    row = 0
    for i, cube in enumerate(cubes):
        if i % 4 == 0 and i != 0:
            sys.stdout.write('\n')
            row += 1
        letter = random.choice(cube)
        sys.stdout.write(letter + ' ')
        Label(root, text=letter, font=("Helvetica", 30)).grid(row=row, column=(i % 4), ipady=20, ipadx=20)
    
    sys.stdout.write('\n\n')

    root.mainloop()
#    time.sleep(3)
#    print 'Start!'
#
#    def stop():
#        print "Stop!"
#    s = sched.scheduler(time.time, time.sleep)
#    s.enter(3*60, 1, stop, ())
#    s.run()
    

