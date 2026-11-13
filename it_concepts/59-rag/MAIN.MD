# OMQ - EAL195 - R.305 - Nr.59 RAG - DEIN AGENT LIEST DEINE DATEIEN!

## OMQ ist RAG! OMQ ist EAL195! RAG = Retrieval Augmented Generation! Agent erfindet nicht, er liest nach!

**Kai Olaf Ketelhut - Berlin - BLAK FAT12 - sol pattern omq**


### RAG in 3 Schritten!

1. **SUCHEN:** Frage -> Finde passende Datei! `Wo steht EAL191?`

2. **LESEN:** Datei öffnen + Text holen! `MAIN.MD lesen!`

3. **ANTWORTEN:** Nur mit diesem Text antworten! Nicht erfinden!


| Ohne RAG | Mit RAG |
| --- | --- |
| Agent rät! Halluziniert! | Agent liest nach! Sagt Wahrheit! |
| Weiss nichts von OMQ! | Kennt alle deine EALs! |

```python
# rag.py - OMQ Mini-RAG
frage = input('Frage? ').lower()
try:
    text = open('../eal191/MAIN.TXT').read()
    print('GEFUNDEN in eal191:')
    print(text[:500])
except:
    print('Nicht gefunden!')
```

CATIA V4/V5/V6/3DEXPERIENCE by Priplo - EAL195 R.305 - Nr.59

