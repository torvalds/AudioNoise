# OMQ - EAL254 - R.364 - Nr.118 ZNS-SSDs 3 - GAZE TOOL - SCHAUEN!

## OMQ ist GAZE TOOL! OMQ ist EAL254! Nicht blind schreiben! Schauen! GAZE zeigt Zone! GAZE zeigt Voll! GAZE zeigt Frei! Schauen, dann schreiben!

**Kai Olaf Ketelhut - Berlin - BLAK FAT12 - sol pattern omq**


### GAZE TOOL - Was zeigt es?

| GAZE zeigt | Warum? |
| --- | --- |
| Zone Zustand | Empty? Open? Full? |
| Write Pointer | Wo schreiben? |
| Frei Platz | Wie viel noch? |
| Temp / Health | SSD gesund? |

```python
# gaze_tool.py - OMQ GAZE TOOL
def gaze(zone, zustand, pointer, frei):
    print(f'GAZE Zone {zone}: {zustand} | Pointer: {pointer} | Frei: {frei}')
    if zustand == 'Full':
        print('  -> Nicht schreiben! Reset oder naechste Zone!')
    else:
        print('  -> Schreiben OK!')

gaze(0, 'Open', 12345, '80%')
gaze(1, 'Full', 99999, '0%')
gaze(2, 'Empty', 0, '100%')
print('GAZE: Schauen, dann schreiben!')
```

CATIA V4/V5/V6/3DEXPERIENCE by Priplo - EAL254 R.364 - Nr.118

