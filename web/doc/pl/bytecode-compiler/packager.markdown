---
layout: doc_pl
title: Etap pakowania
previous: Etap kodowania
previous_url: bytecode-compiler/encoder
next: Etap zapisu
next_url: bytecode-compiler/writer
---

Po tym jak Generator został odpowiednio zakodowany podczas etapu
kodowania, Rubinius pakuje bajtkod jako CompiledCode tworząc nową
instancję klasy CompiledCode i ustawiając na niej kilka atrybutów.

Te atrubuty są dostępne na każdej instancji klasy CompiledCode. Aby
otrzymać instancję CompiledCode z metody Rubiego wywołaj na niej
`executable`.

* *iseq*: Krotka (ang. Tuple) zawierająca sekwencję instrukcji
* *literals*: Krotka zawierająca literały używane w metodzie.
  Rubinius używa literałów wewnętrznie do wartości takich jak String,
  literały używane są przez instrukcje `push_literal` oraz `set_literal`.
* *lines*: Tablica (ang. Array) zawierająca wskaźnik do pierwszej
  instrukcji dla każdej linii reprezentowanej przez bajtkod.
* *required_args*: liczba argumentów wymaganych przez metodę
* *total_args*: całkowita liczba argumentów, zawierająca również argumenty
  opcjonalne
* *splat*: pozycja argumentu splat, jeśli taki istnieje
* *local_count*: ilość zmiennych lokalnych włączając w to argumenty
* *local_names*: Krotka zawierająca listę wszystkich zmiennych
  *lokalnych w kolejności: argumenty wymagane, opcjonalne, splat oraz
  *argumenty blokowe.
* *file*: nazwa pliku, która jest używana przy śledzeniu stosu oraz
  przy innych informacjach do debuggowania
* *name*: nazwa metody
* *primitive*: nazwa podstawowego elementu (ang. primitive) powiązanego z tą metodą
  jeśli taki istnieje
* metadata: jeśli generator został utworzony dla bloku, metadata
  będzie zawierać element `for_block` o wartości `true`. Atrubutu
  `metadata` można używać do przechowywania innych danych dotyczących metody.

Etap pakowania zamienia również wszystkie pomniejsze generatory
(np. generatory dla bloków czy metod) na skompilowane metody
(CompiledCode). Te pomniejsze skompilowane metody są dostępne w
krotce `literals` w nadrzędnej skompilowanej metodzie.

Gdy Generator skończył pakowanie siebie jako CompiledCode, wywołuje
etap zapisywania, przekazując CompiledCode na wejście.

## Pliki

* *kernel/bootstrap/compiled_code.rb*: podstawowa implementacja
  CompiledCode, głównie złożona z odwoływań do elementów
  podstawowych (primitives)
* *kernel/common/compiled_code.rb*: bardziej rozbudowana implementacja
  CompiledCode, połączenie elementów podstawowych oraz metod
  napisanych w Ruby
* *vm/builtin/compiledcode.cpp*: implementacja elementów
  podstawowych w C++
* *lib/compiler/generator.rb*: Implementacja metody `package`, która
  wypełnia obiekt CompiledCode informacjami pochodzącymi z Generatora.

## Dostosowanie pakowania

Ogólnie rzecz biorąc, metoda `package` została zaprojektowana aby
wypełnić obiekt CompiledCode serią zmiennych. Możesz jednak użyć
etapu pakowania aby wypełnić inny obiekt posiadający ten sam interfejs
tymi danymi.
