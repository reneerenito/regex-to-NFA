# 🤖 P1 — Regex a NFA

Convertimos una expresión regular en un NFA y validamos cadenas contra él, en tres etapas:

```
  regex cruda          PARTE 1 ✅              PARTE 2 ⬜               PARTE 3 ⬜
 ┌───────────┐   ┌──────────────────┐   ┌───────────────────┐   ┌──────────────────┐
 │ a(b|c)*   │ → │ Tokenizador +    │ → │ Shunting-Yard     │ → │ NFA (Thompson) + │ → 1 / 0
 │           │   │ concat explícita │   │ (infijo→postfijo) │   │ simulación       │
 └───────────┘   │    a.(b|c)*      │   │     abc|*.        │   └──────────────────┘
                 └──────────────────┘   └───────────────────┘
```

| Etapa | Estado | Archivos |
|---|---|---|
| **1. Tokenizador + validación + concatenación explícita** | ✅ Hecho (Luis) | `src/tokenizer.c/h` |
| **2. Shunting-Yard** | ⬜ Pendiente | `src/regex.c` (el `TODO` en `parse_regex`) |
| **3. NFA: construcción + simulación** | ⬜ Pendiente | `src/nfa.c/h` |

---

## 🚀 Quickstart

```bash
# 1. Build de la imagen (repetir cada vez que cambien código)
docker build -t regex_validator .

# 2. Modo -r: regex procesada (hoy: tokens de la parte 1; con la parte 2: postfijo)
echo 'a(b|c)*' | docker run --rm -i regex_validator -r
# → a.(b|c)*

# 3. Modo -t (default): 1ª línea = regex, resto = cadenas a validar
printf '(ab)*\nab\naba\nabab\n' | docker run --rm -i regex_validator
# → 000 hoy · 101 cuando la parte 3 esté lista

# 4. Regex inválida → mensaje a stderr y exit code 1
echo '*ab' | docker run --rm -i regex_validator -r
# → Error: operador sin operando
```

⚡ Sin Docker (más rápido mientras desarrollas):

```bash
gcc -std=c11 -Wall -Wextra src/*.c -o regex_to_nfa
echo 'a(b|c)*' | ./regex_to_nfa -r
```

---

## 📁 Qué hace cada archivo

| Archivo | Estado | Qué hace |
|---|---|---|
| `src/main.c` | 🏛️ Esqueleto del ayudante — **no tocar** | Punto de entrada. `-r`: lee una regex de stdin y la imprime procesada. `-t`: lee una regex (1ª línea) y cadenas (líneas siguientes), imprime `1`/`0` por cadena. |
| `src/tokenizer.h` | ✅ Parte 1 | API: tipos `token` (`{type, value}`), `token_list` y errores `tokenize_status`. |
| `src/tokenizer.c` | ✅ Parte 1 | Tokenizador + validación de sintaxis + inserción de `.` en una sola pasada. |
| `src/regex.h` | 🔗 Interfaz compartida | Struct `regex` (`items[i].value`, `size`) que `main.c` consume. |
| `src/regex.c` | ⬜ Parte 2 | `parse_regex()` ya llama a `tokenize()` y maneja errores. **El `TODO` marca dónde va Shunting-Yard.** Hoy regresa los tokens en infijo con `.` explícito. |
| `src/nfa.h` / `src/nfa.c` | ⬜ Parte 3 | Stubs de `regex_to_nfa()` (Thompson) y `match_nfa()` (hoy siempre regresa 0). El struct `nfa` es placeholder: **cámbienlo por lo que necesiten**. |
| `tests/test_tokenizer.c` | 🧪 Pruebas | Pruebas unitarias del tokenizador: clasificación, validación e inserción del `.`. |
| `tests/test_regex.c` | 🧪 Pruebas | Pruebas de `parse_regex` con el contrato actual (actualizar los esperados cuando exista Shunting-Yard). |
| `CMakeLists.txt` | 🏛️ Esqueleto (+ tests) | Build. Si agregan archivos `.c`, hay que listarlos aquí. Define también los targets de prueba para `ctest`. |
| `Dockerfile` | 🏛️ Esqueleto (CMD ajustado) | Compila y ejecuta en el contenedor. Default: `-t`; acepta `-r` como argumento. |

---

## 🔍 Parte 1: cómo funciona el tokenizador

`tokenize(str, &out)` hace tres cosas en una sola pasada:

**1. Clasifica** cada carácter:

| Carácter | Token |
|---|---|
| alfanumérico (`a`, `b`, `0`…) | `TOKEN_LITERAL` |
| `\|` `*` `+` `?` (y el `.` insertado) | `TOKEN_OPERATOR` |
| `(` / `)` | `TOKEN_LPAREN` / `TOKEN_RPAREN` |
| espacios y tabs | se ignoran (el enunciado usa `printf ' %s\n'`, que mete un espacio inicial) |
| cualquier otro | ❌ `TOKENIZE_ERR_INVALID_CHAR` |

**2. Valida sintaxis** — regresa un `tokenize_status` descriptivo:

| Error | Ejemplos |
|---|---|
| `TOKENIZE_ERR_UNBALANCED_PARENS` | `a(` · `a)b` |
| `TOKENIZE_ERR_EMPTY_PARENS` | `()` |
| `TOKENIZE_ERR_MISPLACED_OPERATOR` | unario sin operando: `*ab`, `(*a`, `a\|*b` · `\|` suelto: `\|ab`, `a\|\|b`, `ab\|`, `(a\|)` |
| `TOKENIZE_ERR_EMPTY` | regex vacía |

**3. Inserta `.` (concatenación explícita)** entre dos tokens consecutivos cuando el anterior **termina** una expresión (literal, `)`, unario `*`/`+`/`?`) y el actual **empieza** una (literal, `(`):

```
ab → a.b      a( → a.(      )b → ).b      *a → *.a
```

La salida es `out->items`: arreglo de `{type, value}` con tamaño `out->size`, listo para Shunting-Yard. Se libera con `free_token_list()`.

> 📝 `a**` se acepta (hay algo que repetir: `a*`), igual que en los motores de regex reales.

### 🤝 Para quien haga la parte 2 (Shunting-Yard)

Trabajen en el `TODO` de `parse_regex()` (`src/regex.c`). La `token_list` que reciben ya viene validada y balanceada — no necesitan re-checar errores. El `.` insertado es un operador binario más. Precedencia sugerida: `*` `+` `?` > `.` > `|`.

---

## 🧪 Pruebas unitarias

Cada componente tiene su propio binario de pruebas en `tests/`, compilado únicamente con los archivos que necesita, para poder validarlo de forma independiente. Se corren con `ctest`:

```bash
# Con Docker
docker build -t regex_validator .
docker run --rm --entrypoint sh regex_validator -c 'cmake . >/dev/null && make >/dev/null && ctest --output-on-failure'

# Local (build fuera del repo para no ensuciar)
cmake -S . -B build && make -C build && ctest --test-dir build --output-on-failure
```

- `test_tokenizer` — 24 casos: inserción del `.`, manejo de espacios, clasificación de tipos de token y todos los errores de sintaxis.
- `test_regex` — verifica el contrato actual de `parse_regex` (tokens en infijo con `.` explícito). Quien haga la parte 2 solo actualiza los valores esperados a postfijo.
- Cuando exista la parte 3, agreguen `tests/test_nfa.c` con el mismo patrón y su `add_test` en `CMakeLists.txt`.

---

## ✅ Casos probados (parte 1)

| Entrada | Salida |
|---|---|
| `a(b\|c)*` | `a.(b\|c)*` |
| `(ab)*` | `(a.b)*` |
| `(ab)*c+d?` | `(a.b)*.c+.d?` |
| `a*b` | `a*.b` |
| `a\|b\|c` | `a\|b\|c` (sin inserciones) |
| ` a (b)` | `a.(b)` (espacios ignorados) |
| `*ab` · `a\|\|b` · `ab\|` · `(a\|)` | `Error: operador sin operando` |
| `a(` · `a)b` | `Error: parentesis desbalanceados` |
| `()` | `Error: parentesis vacios '()'` |
| `a-b` | `Error: caracter invalido en la regex` |
| (vacía) | `Error: regex vacia` |

---

## ⚠️ Pendiente de aclarar con el profe

El enunciado pide postfijo **con concatenación explícita**, pero su ejemplo muestra `(ab)*` → `ab*` (sin el `.`; con `.` sería `ab.*`). Afecta a la parte 2: hay que confirmar si la salida de `-r` lleva el `.` o no. 🙋
