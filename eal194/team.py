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
