# memory.py - OMQ Agent mit Memory
try:
    alt = open('memory.txt').read()
    print('Ich erinnere mich:', alt)
except:
    print('Kein Memory - Neu starten!')
neu = input('Was soll ich mir merken? ')
open('memory.txt','a').write(neu + '\n')
print('Gemerkt!')
