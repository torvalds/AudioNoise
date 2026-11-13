# OMQ - EAL258 - R.368 - Nr.122 NVMe 2 - QUEUES - VIELE WEGE!

## OMQ ist NVMe Queues! OMQ ist EAL258! Ein Weg = Stau! Viele Wege = Schnell! NVMe hat 64.000 Queues! Jede Queue 64.000 Befehle! Kein Warten!

**Kai Olaf Ketelhut - Berlin - BLAK FAT12 - sol pattern omq**


### Queues - Vergleich!

| | SATA / AHCI | NVMe |
| --- | --- | --- |
| Queues | 1 Queue! | 64.000 Queues! |
| Befehle pro Queue | 32! | 64.000! |
| Stau? | JA! Stau! | NEIN! Viele Wege! |

### 2 Queues - Immer!

1. **Submission Queue (SQ)** - Ich will! Befehl rein!

2. **Completion Queue (CQ)** - Fertig! Antwort raus!


```python
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
```

CATIA V4/V5/V6/3DEXPERIENCE by Priplo - EAL258 R.368 - Nr.122

