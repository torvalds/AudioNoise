# tools.py - OMQ Agent mit Tools
import os, datetime
TOOLS = ['ls', 'datum', 'schreibe']
print('Erlaubte Tools:', TOOLS)
wahl = input('Tool? ls / datum / schreibe: ')
if wahl == 'ls':
    print(os.listdir('.'))
elif wahl == 'datum':
    print(datetime.datetime.now())
elif wahl == 'schreibe':
    open('agent_output.txt','w').write('Agent war hier!')
    print('Geschrieben!')
else:
    print('VERBOTEN! Nicht in TOOLS!')
