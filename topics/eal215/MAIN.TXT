# OMQ - EAL215 - R.325 - Nr.79 CNC WERKZEUG - FRAESER, BOHRER, DREHMEISSEL!

## OMQ ist Werkzeug! OMQ ist EAL215! Falsches Werkzeug = Kaputtes Teil! Richtiges Werkzeug = Perfektes Teil! Fräser fräst! Bohrer bohrt! Drehmeissel dreht!

**Kai Olaf Ketelhut - Berlin - BLAK FAT12 - sol pattern omq**


### Werkzeug-Tabelle - Was macht was?

| Werkzeug | Macht was? | Für was? |
| --- | --- | --- |
| Schaftfräser D10 | Flach fräsen! | Platte, Tasche! |
| Kugelfräser D6 | Rund fräsen! | Form, Rundung! |
| Bohrer D8 | Loch bohren! | Loch D8! |
| Gewindebohrer M6 | Gewinde schneiden! | Schraube M6! |
| Drehmeissel | Drehen! | Welle, rund! |

### OMQ-Regel: Erst Werkzeug wählen, dann fräsen! Nie umgekehrt!

```python
# werkzeug.py - OMQ Werkzeug-Wahl
aufgabe = input('Was willst du? (flach/rund/loch/gewinde/drehen) ')
werkzeuge = {"flach":"Schaftfräser D10","rund":"Kugelfräser D6","loch":"Bohrer D8","gewinde":"Gewindebohrer M6","drehen":"Drehmeissel"}
print(f'NIMM: {werkzeuge.get(aufgabe, "Schaftfräser D10")} - Richtiges Werkzeug = Perfektes Teil!')
```

CATIA V4/V5/V6/3DEXPERIENCE by Priplo - EAL215 R.325 - Nr.79

