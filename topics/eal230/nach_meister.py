# nach_meister.py - OMQ Nach Meister
print('DU BIST MEISTER! Was jetzt?')
print('1 = BAUEN - Deine Fabrik!')
print('2 = LEHREN - Hilf anderen!')
print('3 = WEITER - Neues Kapitel 5!')
wahl = input('Deine Wahl? 1/2/3: ')
antwort = {'1':'BAUEN! Deine Fabrik wartet!','2':'LEHREN! Du bist jetzt OMQ-Lehrer!','3':'WEITER! Kapitel 5 geht los!'}
print(antwort.get(wahl,'BAUEN + LEHREN + WEITER = ALLES!'))
