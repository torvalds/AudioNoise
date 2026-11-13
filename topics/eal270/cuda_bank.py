# cuda_bank.py - OMQ CUDA Bank Conflicts
print('SCHLECHT: Stride 32 - Alle gleiche Bank!')
for tid in range(4):
    idx = tid * 32
    bank = idx % 32
    print(f'Thread {tid}: Fach {idx} -> Bank {bank} - CONFLICT!')
print('GUT: Stride 1 - Alle andere Bank!')
for tid in range(8):
    idx = tid
    bank = idx % 32
    print(f'Thread {tid}: Fach {idx} -> Bank {bank} - Kein Conflict!')
print('TIPP: Padding +1! shared[33] statt shared[32]!')
print('Bank: Nicht gleiche Bank! Alle andere!')
