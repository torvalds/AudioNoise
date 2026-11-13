# OMQ - EAL168 - R.278 ASTEROID - Nr.32 EVENT-DRIVEN - DEEP DIVE!

## OMQ ist Asteroid! OMQ ist EAL168! Fliegt los, wenn was passiert - wie Event!

**Kai Olaf Ketelhut - Berlin - BLAK FAT12 - sol pattern omq**


### Event-Driven = Nicht fragen, reagieren!
Alt: Alle 5 Min fragen: Ist was neues da? Neu: System sagt dir: Hey, was passiert! Event!

### Polling vs Event
|  | Polling | Event-Driven |
| --- | --- | --- |
| Wie? | Immer fragen: Da? Da? Da? | Warten, bis Event kommt! |
| Kosten | Viel Traffic, nix los! | Nur wenn was passiert! |
| Tempo | Langsam, bis 5 Min warten! | Sofort! Millisekunden! |
| Merksatz | Kind: Sind wir da? Sind wir da? | Klingel: Ding-Dong, Paket da! |

### Event Flow - 3 Teile
| Teil | Was? | OMQ-Beispiel |
| --- | --- | --- |
| Producer | Schickt Event | Du pushst! |
| Broker | Verteilt Event | Kafka / RabbitMQ / SNS |
| Consumer | Reagiert auf Event | GitHub Pages baut Seite neu! |

### OMQ-Beispiel - Dein OMQ ist Event-Driven!
Du: `git push` = Event: `Push passiert!` -> Broker: GitHub -> Consumer: GitHub Pages: Baut Seite neu! Du fragst nicht, es passiert!
Anderes: Bestellung bezahlt = Event -> Lager packt + Mail schickt + Rechnung macht! Alles parallel!

### 3 Fehler vermeiden
1. **Kein Retry:** Consumer krank? Event weg! Immer Retry + Dead-Letter-Queue!
2. **Reihenfolge egal denken:** Event 2 vor Event 1? Falscher Stand! Reihenfolge sichern!
3. **Alles Event:** Einfache Abfrage? REST reicht! Nicht alles Event machen!

CATIA V4/V5/V6/3DEXPERIENCE by Priplo - EAL168 R.278 ASTEROID - Nr.32 von 42

MAIN.TEX MAIN.MD MAIN.TXT MAIN.RST MAIN.EPUB MAIN.MOBI MAIN.PDF MAIN.CATScript
