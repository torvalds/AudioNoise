# rag.py - OMQ Mini-RAG
frage = input('Frage? ').lower()
try:
    text = open('../eal191/MAIN.TXT').read()
    print('GEFUNDEN in eal191:')
    print(text[:500])
except:
    print('Nicht gefunden!')
