# append_only.py - OMQ Append Only
log = []
def append(eintrag):
    log.append(eintrag)
    print(f'Hinten dran: {eintrag} -> Log: {log}')

append('Tag 1: Start')
append('Tag 2: Weiter')
append('Tag 3: Mehr Daten')
print('Versuch Mitte aendern? NEIN! Verboten!')
print('Append Only: Nur hinten dran! Wie Tagebuch!')
