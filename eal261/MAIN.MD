# OMQ - EAL261 - R.371 - Nr.125 CUDA 3 - BLOCK + GRID - GROSSE ARMEE!

## OMQ ist Block! OMQ ist Grid! OMQ ist EAL261! 1 Thread = Soldat! 1 Warp = 32 Soldaten! 1 Block = Kompanie! 1 Grid = Armee! Grid ruft alle Blocks! Alle kämpfen zusammen!

**Kai Olaf Ketelhut - Berlin - BLAK FAT12 - sol pattern omq**


### Block + Grid - Armee!

| Stufe | Was? | Wie viele? |
| --- | --- | --- |
| Thread | Soldat! | 1! |
| Warp | Trupp! | 32 Threads! |
| Block | Kompanie! | Viele Warps! Bis 1024 Threads! |
| Grid | Armee! | Viele Blocks! |

### Grid-Regel!

1. **Grid startet Kernel!** Kernel = Programm für GPU!

2. **Blocks teilen sich!** Jeder Block auf 1 SM!

3. **Threads teilen Speicher!** Shared Memory im Block!


```python
# cuda_block_grid.py - OMQ CUDA Block + Grid
# Grid = 4 Blocks, jeder Block = 2 Warps = 64 Threads
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
```

CATIA V4/V5/V6/3DEXPERIENCE by Priplo - EAL261 R.371 - Nr.125

