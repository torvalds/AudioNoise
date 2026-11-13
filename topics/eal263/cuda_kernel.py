# cuda_kernel.py - OMQ CUDA Kernel
def kernel_add(thread_id, a, b):
    return a[thread_id] + b[thread_id]
a = [1, 2, 3, 4, 5, 6, 7, 8]
b = [10, 20, 30, 40, 50, 60, 70, 80]
print('CPU: Starte Kernel <<<2 Blocks, 4 Threads>>>')
result = []
for tid in range(8):
    r = kernel_add(tid, a, b)
    print(f'Thread {tid}: {a[tid]} + {b[tid]} = {r}')
    result.append(r)
print(f'Kernel fertig! Ergebnis: {result}')
print('Kernel: Gleicher Code! Andere Daten!')
