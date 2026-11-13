# cuda_occupancy.py - OMQ CUDA Occupancy
MAX_WARPS = 64
def occupancy(aktive_warps):
    pct = aktive_warps * 100 // MAX_WARPS
    print(f'Aktive Warps: {aktive_warps}/{MAX_WARPS} = Occupancy {pct}%')
    return pct
print('SCHLECHT: Viel Register + Viel Shared!')
occupancy(16)
print('GUT: Wenig Register + Wenig Shared!')
occupancy(64)
print('TIPP: Block 256 Threads! Wenig Register! Wenig Shared!')
print('Occupancy: Stadion voll! Schnell!')
