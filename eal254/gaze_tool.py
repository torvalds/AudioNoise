# gaze_tool.py - OMQ GAZE TOOL
def gaze(zone, zustand, pointer, frei):
    print(f'GAZE Zone {zone}: {zustand} | Pointer: {pointer} | Frei: {frei}')
    if zustand == 'Full':
        print('  -> Nicht schreiben! Reset oder naechste Zone!')
    else:
        print('  -> Schreiben OK!')

gaze(0, 'Open', 12345, '80%')
gaze(1, 'Full', 99999, '0%')
gaze(2, 'Empty', 0, '100%')
print('GAZE: Schauen, dann schreiben!')
