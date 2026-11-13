# cuda_divergence.py - OMQ CUDA Warp Divergence
print('SCHLECHT: threadIdx % 2 - Jeder Warp teilt sich!')
for tid in range(8):
    weg = 'links' if tid % 2 == 0 else 'rechts'
    print(f'Thread {tid}: Geht {weg} - Warp muss BEIDE Wege!')
print('GUT: Warp-weise! Warp 0 = links, Warp 1 = rechts!')
for tid in range(8):
    warp = tid // 4
    weg = 'links' if warp == 0 else 'rechts'
    print(f'Thread {tid} Warp {warp}: Geht {weg} - Alle im Warp gleich!')
print('Divergence: Nicht auseinander! Alle gleich!')
