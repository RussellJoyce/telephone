# Wiring Connections

References:

- https://www.britishtelephones.com/diagrams/n846.pdf
- https://www.britishtelephones.com/documents/700circuitdescription.pdf


Handset
--

```
4 <--> 5 normally open, closed when handset is lifted
```
This connects T3 and T19 through two PTC thermistors (I measured 32 ohms total) when handset is lifted.


Dial
--

```
  Blue D1 <--> D2 Slate  - normally open, closed when dialling
 Brown D3 <--> D4 Pink   - normally open, closed when dialling
 Brown D3 <--> D5 Orange - normally open, closed when dialling, pulses open for digits
  Pink D4 <--> D5 Orange - normally closed, pulses open for digits
```

Easiest option:

```
D4 Pink   - common (GND)
D3 Brown  - dialling detect (input, pull-up) - goes low when dialling
D5 Orange - pulse detect (input, pull-up) - pulses high for each digit
```