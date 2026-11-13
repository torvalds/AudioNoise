# OMQ - EAL194 - R.304 - Nr.58 MULTI-AGENT - 2 AGENTEN ARBEITEN ZUSAMMEN!

## OMQ ist Team! OMQ ist EAL194! 1 Agent = Allein! 2 Agenten = Team!

**Kai Olaf Ketelhut - Berlin - BLAK FAT12 - sol pattern omq**


### So arbeitet ein Team!

| Agent | Job | Beispiel |
| --- | --- | --- |
| 1. Planer | Denkt + Teilt auf! | Baue EAL194 = 1. Text 2. Code 3. Push! |
| 2. Arbeiter | Macht die Aufgabe! | Schreibt Datei! |
| 3. Prüfer | Checkt Fehler! | Ist alles da? Ja / Nein! |

### OMQ-Regel: 1 Chef, viele Arbeiter, 1 Prüfer!

Nicht alle machen alles! Jeder hat 1 Job! Sonst Chaos!


```python
# team.py - OMQ Multi-Agent Team
def planer(aufgabe):
    print('Planer: Teile auf:', aufgabe)
    return ['Schritt 1', 'Schritt 2']
def arbeiter(schritt):
    print('Arbeiter macht:', schritt)
    return 'OK'
def pruefer(ergebnis):
    print('Pruefer: Checke...', ergebnis, '= BESTANDEN!')
schritte = planer('Baue EAL194')
for s in schritte:
    e = arbeiter(s)
    pruefer(e)
print('TEAM FERTIG!')
```

CATIA V4/V5/V6/3DEXPERIENCE by Priplo - EAL194 R.304 - Nr.58

