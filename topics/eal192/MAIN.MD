# OMQ - EAL192 - R.302 - Nr.56 TOOLS GEBEN - DEIN AGENT DARF BASH BENUTZEN!

## OMQ ist Tools! OMQ ist EAL192! Agent ohne Tools = Nur reden! Agent mit Tools = Macht was!

**Kai Olaf Ketelhut - Berlin - BLAK FAT12 - sol pattern omq**


### Tool = Erlaubte Aktion! Agent darf nur, was du erlaubst!

| Tool | Was macht es? | Gefährlich? |
| --- | --- | --- |
| ls / ls_dateien | Zeigt Dateien | NEIN - Nur lesen! |
| datum | Zeigt Datum | NEIN |
| schreibe_datei | Schreibt Datei | JA - Nur in OMQ-Ordner! |
| loesche_alles | Löscht alles | VERBOTEN! Nie geben! |

### OMQ-Regel: Whitelist, nicht Blacklist!

Nicht sagen: Alles erlaubt ausser löschen! Sondern: NUR diese 3 Tools erlaubt!

Agent fragt: Darf ich das? Antwort: Nur wenn Tool in Liste!


```python
# tools.py - OMQ Agent mit Tools
import os, datetime
TOOLS = ['ls', 'datum', 'schreibe']
print('Erlaubte Tools:', TOOLS)
wahl = input('Tool? ls / datum / schreibe: ')
if wahl == 'ls':
    print(os.listdir('.'))
elif wahl == 'datum':
    print(datetime.datetime.now())
elif wahl == 'schreibe':
    open('agent_output.txt','w').write('Agent war hier!')
    print('Geschrieben!')
else:
    print('VERBOTEN! Nicht in TOOLS!')
```

CATIA V4/V5/V6/3DEXPERIENCE by Priplo - EAL192 R.302 - Nr.56

