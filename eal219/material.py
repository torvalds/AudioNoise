# material.py - OMQ Material-Wahl
wo = input('Wo wird Teil benutzt? (drinnen/draussen/auto/flexibel) ')
mat = {"drinnen":"PLA - 200°C - Einfach!","draussen":"PETG - 230°C - Wasserfest!","auto":"ABS - 240°C - Stark!","flexibel":"TPU - 220°C - Gummi!"}
print(f'NIMM: {mat.get(wo, "PLA - 200°C - Einfach!")}')
