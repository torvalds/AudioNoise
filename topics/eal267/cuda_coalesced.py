# cuda_coalesced.py - OMQ CUDA Coalesced Memory
data = list(range(32))
print('RICHTIG: Coalesced! Alle nebeneinander!')
for tid in range(8):
    print(f'Thread {tid} liest Fach {tid} = {data[tid]}')
print('1 Fahrt! Schnell!')
print('FALSCH: Wild! Thread * 10 springt!')
for tid in range(4):
    idx = tid * 10
    print(f'Thread {tid} liest Fach {idx} - Springt! Langsam!')
print('Coalesced: Zusammen! 1 Fahrt!')
