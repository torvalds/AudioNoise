# cuda_unified.py - OMQ CUDA Unified Memory
print('cudaMallocManaged(): 1 Speicher fuer CPU + GPU!')
data = [1, 2, 3, 4]
print(f'CPU schreibt: {data}')
print('GPU liest gleiches data - Kein cudaMemcpy!')
result = [x * 10 for x in data]
print(f'GPU rechnet: {result}')
print('CPU liest Ergebnis: Gleicher Speicher!')
print('Unified Memory: 1 Speicher! Kein Kopieren!')
