# cuda_tensor.py - OMQ CUDA Tensor Cores
print('Normale Cores: 1 Zahl x 1 Zahl - 16 Mal fuer 4x4!')
A = [[1,2],[3,4]]
B = [[5,6],[7,8]]
C = [[0,0],[0,0]]
for i in range(2):
    for j in range(2):
        C[i][j] = A[i][0]*B[0][j] + A[i][1]*B[1][j]
print(f'Normal: {C} - Viele Schritte!')
print('Tensor Cores: Ganze Matrix in 1 Schritt!')
print('wmma: load A + load B -> mma_sync -> store C - Boom!')
print('FP16: Halb genau! Doppelt schnell! Fuer AI perfekt!')
print('Tensor: Mathe-Monster! Nur Matrizen!')
