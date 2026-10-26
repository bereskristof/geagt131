# Számítógépi grafika bradandó

A Ludo társasjáték 3D-s interaktív szimulációja.

## Irányítás

- **WASD, Arrow keys, Mouse:** Kamera mozgatása
- **Lshift, Lctrl, Mouse scroll:** Kamera bezoomolása
- **1, 2, 3, 4:** Bábu kiválasztása
- **Space:** Kocka dobás/Kiválasztott bábu mozgatása
- **'-', '=':** Napidő változtatása
- **0:** Wireframe nézet
- **B, N, M:** Előre beállított kamera-pontok

## CPU játék

A terminálból egy négyjegyű bináris sorozattal beállítható, hogy egy játékos ember, vagy gép legyen. A `0` embernek, az `1` CPU-nak felel meg. Példa:

```
$ make windows && main.exe 0110 // 1 & 4 ember, 2 & 3 CPU
```

## Kiemelt funkciók

- Arcball kamera implementáció
- Animált mozgatás, dobókocka
- Hamis árnyékhatás (Áttetsző textúrával)
- LOD implementácó (Annak ellenére, hogy nincs sok haszna ezesetben)
