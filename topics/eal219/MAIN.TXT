# OMQ - EAL219 - R.329 - Nr.83 3D-DRUCK MATERIAL - PLA, ABS, PETG!

## OMQ ist Material! OMQ ist EAL219! Falsches Material = Kaputtes Teil! Richtiges Material = Perfektes Teil! PLA einfach! ABS stark! PETG beides!

**Kai Olaf Ketelhut - Berlin - BLAK FAT12 - sol pattern omq**


### Material-Tabelle - Was nimmst du wann?

| Material | Temp | Gut für | Schlecht für |
| --- | --- | --- | --- |
| PLA | 200°C | Anfänger! Spielzeug! Deko! | Draussen! Heiss! |
| ABS | 240°C | Stark! Auto-Teil! | Stinkt! Verzieht! |
| PETG | 230°C | Draussen! Wasser! Stark! | Schwer zu drucken! |
| TPU | 220°C | Flexibel! Gummi! | Langsam! |

### OMQ-Regel: Erst fragen: Wo wird Teil benutzt? Dann Material wählen!

```python
# material.py - OMQ Material-Wahl
wo = input('Wo wird Teil benutzt? (drinnen/draussen/auto/flexibel) ')
mat = {"drinnen":"PLA - 200°C - Einfach!","draussen":"PETG - 230°C - Wasserfest!","auto":"ABS - 240°C - Stark!","flexibel":"TPU - 220°C - Gummi!"}
print(f'NIMM: {mat.get(wo, "PLA - 200°C - Einfach!")}')
```

CATIA V4/V5/V6/3DEXPERIENCE by Priplo - EAL219 R.329 - Nr.83

