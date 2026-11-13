# cuda_sync_race.py - OMQ CUDA Sync + Race
shared = [0] * 4
def sync():
    print('__syncthreads(): Alle warten! Alle da!')
print('FALSCH: Race! Alle schreiben Fach 0 gleichzeitig!')
print('RICHTIG: Jeder eigenes Fach! Dann Sync! Dann Lesen!')
for t in range(4):
    shared[t] = t * 10
    print(f'Thread {t}: Schreibe Fach {t} = {t*10}')
sync()
print(f'Alle lesen: {shared} - Sauber!')
print('Atomic: Einer nach dem anderen!')
zaehler = 0
for t in range(4):
    zaehler += 1
    print(f'Thread {t}: atomicAdd -> Zaehler = {zaehler}')
print('Sync + Atomic = Kein Chaos!')
