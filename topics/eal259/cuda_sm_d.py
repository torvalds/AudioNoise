# cuda_sm_d.py - OMQ CUDA SM_D
sms = ['SM 0', 'SM 1', 'SM 2', 'SM 3']
warps = [f'Warp {i} - 32 Threads' for i in range(8)]
for i, warp in enumerate(warps):
    sm = sms[i % len(sms)]
    print(f'SM_D: {warp} -> {sm} - Dispatch!')
print('CUDA: 1 SM = Viele Kerne! Viele SM = Super schnell!')
print('SM_D = Verteiler!')
