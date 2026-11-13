# cuda_warp.py - OMQ CUDA Warp + Thread
warp = list(range(32))
print('Warp startet: 32 Threads!')
results = []
for t in warp:
    r = t * 2
    results.append(r)
    print(f'Thread {t}: {t} * 2 = {r}')
print(f'Warp fertig! 32 Ergebnisse: {results[:5]} ...')
print('Warp: 32 Freunde! Alle gleich!')
