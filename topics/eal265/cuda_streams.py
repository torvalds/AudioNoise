# cuda_streams.py - OMQ CUDA Streams
streams = ['Stream 0 - Default', 'Stream 1', 'Stream 2']
for s in streams:
    print(f'{s}: Kopieren -> Kernel -> Kopieren zurueck!')
    print(f'{s}: Laeuft parallel! Kein Warten auf andere!')
print('3 Streams = 3 Bahnen! Alle gleichzeitig!')
print('Streams: Mehr Bahnen! Kein Stau!')
