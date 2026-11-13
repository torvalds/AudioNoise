# nvme_reset.py - OMQ NVMe + Reset Zone
class NVMeZone:
    def __init__(self, name):
        self.name = name
        self.pointer = 0
        self.zustand = 'Empty'
    def write(self, daten):
        print(f'NVMe {self.name}: Schreibe bei {self.pointer}: {daten}')
        self.pointer += 1
        self.zustand = 'Open'
    def reset(self):
        print(f'NVMe {self.name}: RESET Befehl!')
        self.pointer = 0
        self.zustand = 'Empty'
        print(f'{self.name}: Neu! Pointer = 0! Zustand = Empty!')

z = NVMeZone('Zone 0')
z.write('Daten A')
z.write('Daten B')
z.reset()
z.write('NEU: Daten C')
print('NVMe + Reset = Schnell + Neu anfangen!')
