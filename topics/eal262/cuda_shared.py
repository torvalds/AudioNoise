# cuda_shared.py - OMQ CUDA Shared Memory
shared = {}
def thread_schreibt(tid, wert):
    shared[tid] = wert
    print(f'Thread {tid}: Schreibe {wert} in Shared!')
def syncthreads():
    print('__syncthreads(): Alle warten! Alle da! Weiter!')
for t in range(4):
    thread_schreibt(t, t*10)
syncthreads()
print(f'Alle sehen Shared: {shared}')
print('Shared Memory: Gemeinsam! Schnell!')
