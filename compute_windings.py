import numpy as np

rotations = 600
diameter = 40  # mm
circumference = np.pi * diameter
wire_length = rotations * circumference

print(wire_length)

coil_diameter = 10  # mm
coil_circumference = np.pi * coil_diameter
print(17.9/(coil_circumference*1e-3))
windings = wire_length/coil_circumference

print(f"We have {windings} windings")

18/1.12

mu_rel = 6
coil_length = 24  # mm
num_turns = 600
current = 1  # A

mu0 = 4*np.pi*1e-7
mu = mu_rel*mu0

B = mu * current * num_turns / (coil_length*1e-3)

def fun(x): print(x)

fun(B)

## 300 turns coil with 0.3mm wire
## works well at 0.5A gets too hot
## double the number of turns to get the same magnetic field at half the current
## resistance doubles but heating due to current is reduced to a fourth
## heating in total gets halved, maybe

## https://www.accelinstruments.com/Magnetic/Magnetic-field-calculator.html
