#!/usr/bin/env python

import random
import sys
import time
import sched

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
    for i, cube in enumerate(cubes):
        if i % 4 == 0:
            sys.stdout.write('\n')
        sys.stdout.write(random.choice(cube) + ' ')
    sys.stdout.write('\n\n')

    time.sleep(3)
    print 'Start!'

    def stop():
        print "Stop!"
    s = sched.scheduler(time.time, time.sleep)
    s.enter(3*60, 1, stop, ())
    s.run()
    

