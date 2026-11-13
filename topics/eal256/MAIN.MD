# OMQ - EAL256 - R.366 - Nr.120 ZNS-SSDs 5 - APPEND ONLY - HINTEN DRAN!

## OMQ ist Append Only! OMQ ist EAL256! Nicht in Mitte schreiben! Nicht überschreiben! Nur hinten dran! Wie Tagebuch! Wie Zug! Immer hinten dran!

**Kai Olaf Ketelhut - Berlin - BLAK FAT12 - sol pattern omq**


### Append Only - So geht es!

| Was? | Darf man? |
| --- | --- |
| Hinten dran | JA! Immer! |
| Mitte ändern | NEIN! Verboten! |
| Anfang ändern | NEIN! Verboten! |
| Alles löschen + neu | JA! Reset! |

```python
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
```

CATIA V4/V5/V6/3DEXPERIENCE by Priplo - EAL256 R.366 - Nr.120

