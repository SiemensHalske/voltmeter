import matplotlib
matplotlib.use('Agg')

import schemdraw
import schemdraw.elements as elm

with schemdraw.Drawing() as d:
    d.config(unit=3)

    d += elm.SourceV().label('Vin', loc='left')
    d += elm.Resistor().label('R', loc='top')
    d += elm.Dot(open=True)
    d.push()
    d += elm.Line().down()
    d += elm.Capacitor().label('C', loc='right')
    d += elm.Ground()
    d.pop()
    d += elm.Line().right()
    d += elm.Dot(open=True)
    d += elm.Line().right().length(1)
    d += elm.Dot(open=True)
    d += elm.Label().label('→ µC / ADC Pin', loc='right', align='center')

    d.save('rc_lowpass_filter_fixed.png')
