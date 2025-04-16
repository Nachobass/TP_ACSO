from itertools import product
import string

# array.0 extraído del binario
array = [2, 13, 7, 14, 5, 10, 6, 15,
         1, 12, 3, 4, 11, 8, 16, 9]

# caracteres posibles que cumplen que ord(c) & 0x0F == i
char_map = {i: [] for i in range(16)}
for c in string.printable:
    i = ord(c) & 0x0F
    char_map[i].append(c)

target = 44
for indices in product(range(16), repeat=6):
    if sum(array[i] for i in indices) == target:
        candidates = ['']
        for i in indices:
            new_candidates = []
            for prefix in candidates:
                for c in char_map[i]:
                    new_candidates.append(prefix + c)
            candidates = new_candidates

        print("Índices:", indices)
        print("Ejemplo(s) de palabra válida:")
        for word in candidates[:5]:
            print("  →", word)
        break