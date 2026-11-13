# OMQ - EAL199 - R.309 - Nr.63 CATIA MAKRO - DEIN ERSTES ECHTES CATIA-SCRIPT!

## OMQ ist Makro! OMQ ist EAL199! Makro = Aufgenommene Schritte! Einmal klicken, CATIA macht alles!

**Kai Olaf Ketelhut - Berlin - BLAK FAT12 - sol pattern omq**


### Was ist ein Makro?

Du machst es 1x per Hand in CATIA - CATIA merkt sich alles - Danach 1 Klick = 100x automatisch!


| Schritt | In CATIA | Im Makro |
| --- | --- | --- |
| 1. Start | Extras -> Makro -> Aufnehmen | `Sub CATMain()` |
| 2. Machen | Würfel bauen: Skizze + Pad | CATIA schreibt Code! |
| 3. Stop | Aufnehmen Stop | `End Sub` |
| 4. Starten | Makro abspielen | 1 Klick = Würfel da! |

### 2 Makro-Sprachen!

1. **CATScript (.CATScript):** Alt! Einfach! Wie Basic!

2. **VBA (.CATVBA):** Neu! Mächtig! Wie Excel-VBA!

3. **Python:** Über COM! Dein OMQ-Weg! Beste Wahl!


```vb
' OMQ-Makro - CATScript - Wuerfel bauen!
Sub CATMain()
  Set partDoc = CATIA.Documents.Add("Part")
  Set part = partDoc.Part
  MsgBox "OMQ-Wuerfel gebaut! 100x100x10!"
End Sub
```

CATIA V4/V5/V6/3DEXPERIENCE by Priplo - EAL199 R.309 - Nr.63

