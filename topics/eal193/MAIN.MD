# OMQ - EAL193 - R.303 - Nr.57 MEMORY - DEIN AGENT VERGISST NICHTS MEHR!

## OMQ ist Memory! OMQ ist EAL193! Agent ohne Memory = Jedes Mal neu! Agent mit Memory = Erinnert sich!

**Kai Olaf Ketelhut - Berlin - BLAK FAT12 - sol pattern omq**


### 3 Arten von Memory!

| Memory | Was? | Beispiel | Wo? |
| --- | --- | --- | --- |
| 1. Kurz | Dieser Chat! | Was hast du gerade gesagt? | RAM / Liste |
| 2. Lang | Für immer! | Kai mag CATIA + Berlin! | Datei / memory.txt |
| 3. Zusammenfassung | Altes komprimiert! | 300 Runden = OMQ-System! | summary.txt |

### OMQ-Regel: Alles was wichtig ist, wird aufgeschrieben!

Mensch vergisst. Datei vergisst nicht! Deshalb: Agent schreibt nach jeder Runde in memory.txt!


```python
# memory.py - OMQ Agent mit Memory
# Liest altes Memory, schreibt neues dazu!
try:
    alt = open('memory.txt').read()
    print('Ich erinnere mich:', alt)
except:
    print('Kein Memory - Neu starten!')
neu = input('Was soll ich mir merken? ')
open('memory.txt','a').write(neu + '\n')
print('Gemerkt!')
```

CATIA V4/V5/V6/3DEXPERIENCE by Priplo - EAL193 R.303 - Nr.57

