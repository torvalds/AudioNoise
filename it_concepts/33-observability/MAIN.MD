# OMQ - EAL169 - R.279 KOMET - Nr.33 OBSERVABILITY - DEEP DIVE!

## OMQ ist Komet! OMQ ist EAL169! Schweif zeigt, wo er war - wie Observability!

**Kai Olaf Ketelhut - Berlin - BLAK FAT12 - sol pattern omq**


### Observability = Sehen, was wirklich los ist!
Monitoring = Ist Server an? Observability = WARUM ist Seite langsam? Wo hängt es? Wer ist schuld?

### Die 3 Säulen
| Säule | Was? | OMQ-Frage |
| --- | --- | --- |
| Logs | Was passiert? Zeile für Zeile! | Was steht im Log? |
| Metrics | Wie viel? Zahlen! | Wie viele User? Wie langsam? |
| Traces | Wo lang? Welcher Weg? | Wo hängt Anfrage? Bei DB? Bei API? |

### Logs vs Metrics vs Traces
|  | Logs | Metrics | Traces |
| --- | --- | --- | --- |
| Beispiel | `ERROR: MAIN.MD fehlt!` | `500 Fehler: 12x` | `User -> API (20ms) -> DB (800ms)` |
| Frage | WAS ist passiert? | WIE OFT? WIE VIEL? | WO ist es langsam? |
| Tool | Loki, CloudWatch | Prometheus, Grafana | Jaeger, Tempo |

### OMQ-Beispiel - Dein OMQ langsam?
Monitoring: Seite langsam! Observability: Trace sagt: DB 800ms, API 20ms = DB schuld! Log sagt: Kein Index auf nr! Fix: Index! Fertig!
Ohne Trace: Du rätst! Mit Trace: Du weisst!

### 3 Fehler vermeiden
1. **Nur Monitoring:** Grün = Alles gut? Nein! User sagt langsam! Traces brauchen!
2. **Zu viele Logs:** Alles loggen = Teuer + nix finden! Nur wichtiges loggen!
3. **Kein Correlation-ID:** 100 User, 100 Logs - wem gehört was? 1 ID pro Anfrage!

CATIA V4/V5/V6/3DEXPERIENCE by Priplo - EAL169 R.279 KOMET - Nr.33 von 42

MAIN.TEX MAIN.MD MAIN.TXT MAIN.RST MAIN.EPUB MAIN.MOBI MAIN.PDF MAIN.CATScript
