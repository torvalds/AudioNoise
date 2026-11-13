# OMQ - EAL253 - R.363 - Nr.117 ZNS-SSDs 2 - ZONEN IM DETAIL!

## OMQ ist ZNS-SSDs 2! OMQ ist EAL253! Zone ist nicht Zone! Jede Zone hat Regeln! Jede Zone hat Anfang! Jede Zone hat Ende! Wir schauen genau!

**Kai Olaf Ketelhut - Berlin - BLAK FAT12 - sol pattern omq**


### Zonen im Detail!

| Zone | Zustand | Was darf man? |
| --- | --- | --- |
| Empty | Leer! | Schreiben von Anfang! |
| Open | Offen! | Weiter schreiben! |
| Full | Voll! | Nur lesen! Nicht schreiben! |
| Closed | Zu! | Reset dann neu! |

### Zone-Regeln!

1. **Nur vorwärts!** Nie rückwärts schreiben!

2. **Append only!** Immer hinten dran!

3. **Reset!** Zone leeren = wieder Empty!


```python
# zns_zonen.py - OMQ ZNS Zonen im Detail
zonen = [('Zone 0','Empty','Schreiben!'),('Zone 1','Open','Weiter!'),('Zone 2','Full','Nur lesen!'),('Zone 3','Closed','Reset!')]
for name, zustand, aktion in zonen:
    print(f'{name}: {zustand} -> {aktion}')
print('ZNS-Regel: Nur vorwaerts! Append only!')
```

CATIA V4/V5/V6/3DEXPERIENCE by Priplo - EAL253 R.363 - Nr.117

