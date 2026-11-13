# cuda_debugging.py - OMQ CUDA Debugging
def cuda_check(fehler):
    if fehler != 'OK':
        print(f'FEHLER: {fehler} - Hier stoppen!')
        return False
    print('OK: Kein Fehler!')
    return True
print('Nach Kernel 1:')
cuda_check('OK')
print('Nach Kernel 2: Index-Fehler!')
cuda_check('Invalid Argument - Index zu gross!')
print('FIX: if (idx < N) pruefen!')
print('TIPP: compute-sanitizer ./programm = Findet Fehler!')
print('Debugging: Check! Klein! Finden!')
