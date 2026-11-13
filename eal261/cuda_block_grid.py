# cuda_block_grid.py - OMQ CUDA Block + Grid
GRID_BLOCKS = 4
WARPS_PER_BLOCK = 2
THREADS_PER_WARP = 32
total = 0
for block in range(GRID_BLOCKS):
    print(f'Block {block} startet auf SM!')
    for warp in range(WARPS_PER_BLOCK):
        for thread in range(THREADS_PER_WARP):
            total += 1
    print(f'  Block {block}: {WARPS_PER_BLOCK * THREADS_PER_WARP} Threads fertig!')
print(f'Grid fertig! Armee: {total} Threads!')
print('Block + Grid = Grosse Armee!')
