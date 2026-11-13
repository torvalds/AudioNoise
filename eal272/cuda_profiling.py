# cuda_profiling.py - OMQ CUDA Profiling
import time
def kernel_langsam():
    time.sleep(0.09)
    return 'Kernel'
def kopieren():
    time.sleep(0.01)
    return 'Kopieren'
start = time.time()
kernel_langsam()
kopieren()
gesamt = time.time() - start
print(f'Gesamt: {gesamt:.2f}s')
print('Kernel: 90% - DORT optimieren!')
print('Kopieren: 10% - Nicht wichtig!')
print('Profiling: Messen! Nicht raten!')
print('nsys profile ./programm - Dann nsys stats!')
