# OMQ - EAL167 - R.277 MOND - Nr.31 REST vs GraphQL - DEEP DIVE!

## OMQ ist Mond! OMQ ist EAL167! Umkreist Erde = Umkreist deine Daten!

**Kai Olaf Ketelhut - Berlin - BLAK FAT12 - sol pattern omq**


### REST vs GraphQL = Bestellen vs Buffet!
REST = Menü: Du bekommst, was Küche sagt! GraphQL = Buffet: Du nimmst, was du willst, nur das!

### REST vs GraphQL - Tabelle
|  | REST | GraphQL |
| --- | --- | --- |
| Anfragen | Viele: /eal + /thema + /autor | 1 Anfrage, alles drin! |
| Daten | Fest: Immer alles! | Genau: Nur was du fragst! |
| Beispiel | GET /eal/167 | query { eal(nr:167) { titel, thema } } |
| Gut für | Einfach, Standard, Cache | Handy-App, wenig Daten, genau! |

### REST Problem - Overfetching + Underfetching
| Problem | Was? | Beispiel |
| --- | --- | --- |
| Overfetching | Zu viel! | Du willst nur Titel, bekommst 100 Felder! |
| Underfetching | Zu wenig! | Du willst Titel + Autor, musst 2x fragen! |
| GraphQL Lösung | Genau richtig! | 1 Frage, nur Titel + Autor! |

### OMQ-Beispiel
REST: `GET /eal167` -> Gibt alles: Titel, Text, Autor, Datum, Links...
GraphQL: `query { eal(nr:167) { titel } }` -> Gibt nur Titel! Schnell für Handy!
Dein OMQ: MAIN.MD = REST-Denken! Alles in 1 Datei! GraphQL wäre: Nur Titel aus allen 42!

### 3 Fehler vermeiden
1. **Alles GraphQL:** Einfache Seite? REST reicht! Nicht überbauen!
2. **Kein Limit:** GraphQL: User fragt 10000 Felder = Server tot! Depth Limit setzen!
3. **Kein Cache:** REST cached gut, GraphQL nicht! Cache selbst bauen!

CATIA V4/V5/V6/3DEXPERIENCE by Priplo - EAL167 R.277 MOND - Nr.31 von 42

MAIN.TEX MAIN.MD MAIN.TXT MAIN.RST MAIN.EPUB MAIN.MOBI MAIN.PDF MAIN.CATScript
