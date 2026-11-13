# write_pointer.py - OMQ Write Pointer
class Zone:
    def __init__(self, name):
        self.name = name
        self.pointer = 0
        self.data = []
    def write(self, block):
        print(f'{self.name}: Schreibe bei {self.pointer}: {block}')
        self.data.append(block)
        self.pointer += 1
    def reset(self):
        self.pointer = 0
        self.data = []
        print(f'{self.name}: Reset! Pointer = 0!')

z = Zone('Zone 0')
z.write('Block A')
z.write('Block B')
z.write('Block C')
print(f'Pointer jetzt: {z.pointer}')
z.reset()
print('Write Pointer: Nur vorwaerts! Nie zurueck!')
