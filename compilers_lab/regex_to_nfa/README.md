# P1 — Regex a NFA

Convierte una expresión regular a un NFA y valida cadenas contra él. 

Pipeline: tokenizador con concatenación explícita → Shunting-Yard (infijo a postfijo) → NFA + simulación.

## Uso

```bash
docker build -t regex_validator .

# -r: imprime la regex en postfijo
echo 'a(b|c)*' | docker run --rm -i regex_validator -r
# → abc|*.

# -t (default): 1ª línea = regex, resto = cadenas; imprime 1/0 por cadena
printf '(ab)*\nab\naba\nabab\n' | docker run --rm -i regex_validator
# → 101
```

Una regex inválida imprime el error a stderr y termina con código 1.

Sin Docker:

```bash
cmake -S . -B build && make -C build
echo 'a(b|c)*' | ./build/regex_to_nfa -r
```

## Pruebas

```bash
ctest --test-dir build --output-on-failure
```

Hay una suite por componente: `test_tokenizer`, `test_regex` y `test_nfa`.

## Estructura

| Archivo             | Qué hace                                                          |
| ------------------- | ----------------------------------------------------------------- |
| `src/main.c`        | Punto de entrada (esqueleto del ayudante, no tocar)               |
| `src/tokenizer.c/h` | Tokeniza, valida sintaxis e inserta `.` (concatenación explícita) |
| `src/regex.c/h`     | `parse_regex()`: Shunting-Yard, regresa la regex en postfijo      |
| `src/pila.c/h`      | Pila usada por Shunting-Yard                                      |
| `src/nfa.c/h`       | `regex_to_nfa()` (Thompson) y `match_nfa()` (simulación)          |
| `tests/`            | Pruebas unitarias de cada componente                              |

Sintaxis soportada: literales alfanuméricos, `|`, `*`, `+`, `?` y paréntesis. Los espacios se ignoran.
