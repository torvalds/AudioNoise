# cuda_multi_gpu.py - OMQ CUDA Multi-GPU
data = list(range(100))
print('1 GPU: 100 Zahlen allein!')
print('4 GPUs: Jeder 25 Zahlen!')
gpus = {0: data[0:25], 1: data[25:50], 2: data[50:75], 3: data[75:100]}
for gpu, teil in gpus.items():
    print(f'GPU {gpu}: cudaSetDevice({gpu}) - Rechnet {len(teil)} Zahlen!')
    ergebnis = sum(teil)
    print(f'GPU {gpu}: Ergebnis = {ergebnis}')
print('Alle fertig: Ergebnisse zusammen!')
print('Multi-GPU: Teilen! Rechnen! Zusammen! Wenig Kopieren!')
