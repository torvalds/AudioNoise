# nvme_queues.py - OMQ NVMe Queues
from collections import deque
sq = deque()  # Submission Queue
cq = deque()  # Completion Queue

def submit(befehl):
    sq.append(befehl)
    print(f'SQ: Rein: {befehl}')

def complete():
    b = sq.popleft()
    cq.append(f'Fertig: {b}')
    print(f'CQ: Fertig: {b}')

submit('Lesen Zone 0')
submit('Schreiben Zone 1')
submit('Reset Zone 2')
complete()
complete()
complete()
print('NVMe: Viele Queues! Kein Stau!')
