# -*- coding: utf-8 -*-

from threading import Thread, Lock
from time      import sleep, time
from random    import random

import math, sys

LUCIOLE_FLASH_PHASE_LENGTH = 1000
LUCIOLE_WAIT_PHASE_LENGTH = 3000
LUCIOLE_VIEW = 5
LUCIOLE_ADD_IN_SWARM_PROB = 0.1
LUCIOLE_JUMP_THRESHOLD = 5

NETWORK_ALEA = 0.8
DELAY_ALEA = 0.05

GLOBAL_CLOCK = int(time() * 1000)

def current_ms():
    return int(time() * 1000) - GLOBAL_CLOCK

lock = Lock()
lucioles = []
alive = True

def finish():
    global alive

    print >> sys.stderr, "* Finishing..."
    alive = False
    for l in lucioles:
        l.join()
    print >> sys.stderr, "> Done"

class Luciole(Thread):
    def __init__(self, lid):
        Thread.__init__(self)

        self.lid = lid
        self.swarm = []
        self.swarm_offset = 0
        self.flashing = False
        self.last_flash = 0

    def run(self):
        global alive

        while alive:
            self.step()

    def step(self):
        global lucioles

        #
        # Phase de flash
        #

        sleep(0.255)
        self.last_flash = self.flashing = current_ms()
        sleep((LUCIOLE_FLASH_PHASE_LENGTH - 255) / 1000.0)
        self.flashing = False

        #
        # Phase d'écoute
        #

        clock = current_ms()
        past = []
        count = 0

        while current_ms() - clock < LUCIOLE_WAIT_PHASE_LENGTH and count < LUCIOLE_JUMP_THRESHOLD:
            for l in lucioles:
                if l not in past and l.flashing and l.flashing > clock and random() < NETWORK_ALEA:
                    past.append(l)

                    if l in self.swarm:
                        count += 1
                    elif len(self.swarm) < LUCIOLE_VIEW or random() < LUCIOLE_ADD_IN_SWARM_PROB:
                        if self.swarm_offset < len(self.swarm):
                            self.swarm[self.swarm_offset] = l
                        else:
                            self.swarm.append(l)

                        self.swarm_offset = (self.swarm_offset + 1) % LUCIOLE_VIEW
                        count += 1

            sleep(0.05 + random() * DELAY_ALEA)

print >> sys.stderr, "* Creating lucioles..."
for i in range(0, 30):
    lucioles.append(Luciole(i))
print >> sys.stderr, "> Done"

print >> sys.stderr, "* Starting lucioles..."
try:
    for l in lucioles:
        l.start()
        sleep(random() * 0.5)
except KeyboardInterrupt:
    finish()
    sys.exit(1)
print >> sys.stderr, "> Done"

while True:
    #mean = 0
    #e = 0
    #values = []
    count = 0
    clock = current_ms()

    for l in lucioles:
        #values.append(l.last_flash)
        if l.flashing:
            count += 1

    #for v in values:
    #    mean += v

    #mean /= len(values)

    #for i in range(0, len(values)):
    #    e += math.sqrt((mean - values[i]) * (mean - values[i]))

    #e /= len(values)

    print "%d %d" % (clock, count)

    if count is 30:
        print >> sys.stderr, "** SYNCHRONIZED @ %ds **" % (clock / 1000)

    try:
        sleep(0.1)
    except KeyboardInterrupt:
        break

finish()
