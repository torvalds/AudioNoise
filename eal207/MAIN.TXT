# OMQ - EAL207 - R.317 - Nr.71 CATIA TOLERANZ - PASST ODER WACKELT?

## OMQ ist Toleranz! OMQ ist EAL207! Nichts ist exakt 100mm! Alles ist 100mm +/- etwas! Toleranz sagt: Wie viel darf es wackeln?

**Kai Olaf Ketelhut - Berlin - BLAK FAT12 - sol pattern omq**


### Warum Toleranz?

Welle D30 muss in Loch D30! Aber exakt gleich = passt nicht rein!

Lösung: Welle 29,9 + Loch 30,1 = 0,2 Luft = Passt!


| Passung | Welle | Loch | Ergebnis |
| --- | --- | --- | --- |
| Spielpassung | 29,9 | 30,1 | Wackelt, dreht sich! |
| Übergang | 30,0 | 30,0 | Mal passt, mal nicht! |
| Presspassung | 30,1 | 29,9 | Sitzt fest, nie wieder raus! |

### OMQ-Regel: Ohne Toleranz kein echtes Teil!

```python
# toleranz.py - OMQ Toleranz-Check
welle = float(input('Wellen-Durchmesser? '))
loch = float(input('Loch-Durchmesser? '))
spiel = loch - welle
print(f'Spiel: {spiel:.2f} mm')
if spiel > 0:
    print('SPIELPASSUNG - Dreht sich! Gut fuer Welle!')
elif spiel == 0:
    print('UEBERGANG - Mal passt, mal nicht!')
else:
    print('PRESSPASSUNG - Sitzt fest! Nur mit Presse!')
```

CATIA V4/V5/V6/3DEXPERIENCE by Priplo - EAL207 R.317 - Nr.71

