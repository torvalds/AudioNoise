# agent.py - OMQ Mini-Agent
task = input('Was soll ich tun? ')
for i in range(3):
    print(f'Runde {i+1}: Denke nach...')
    print(f'Aktion: {task} - Schritt {i+1}')
    print('Beobachtung: OK, weiter!')
print('FERTIG!')
