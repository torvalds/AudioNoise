# werkzeug.py - OMQ Werkzeug-Wahl
aufgabe = input('Was willst du? (flach/rund/loch/gewinde/drehen) ')
werkzeuge = {"flach":"Schaftfräser D10","rund":"Kugelfräser D6","loch":"Bohrer D8","gewinde":"Gewindebohrer M6","drehen":"Drehmeissel"}
print(f'NIMM: {werkzeuge.get(aufgabe, "Schaftfräser D10")} - Richtiges Werkzeug = Perfektes Teil!')
