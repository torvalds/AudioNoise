# gcode.py - OMQ G-Code-Schreiber
print('M3 S1000 - Spindel an!')
print('G0 X0 Y0 - Schnell hin!')
for x,y in [(50,0),(50,50),(0,50),(0,0)]:
    print(f'G1 X{x} Y{y} - Fraest nach {x},{y}!')
print('M5 - Spindel aus!')
print('M30 - Programm Ende! Quadrat fertig!')
