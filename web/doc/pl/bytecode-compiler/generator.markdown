---
layout: doc_pl
title: Etap generowania
previous: AST
previous_url: bytecode-compiler/ast
next: Etap kodowania
next_url: bytecode-compiler/encoder
review: true
---

Po utworzeniu Drzewa AST przez parser Melbourne jest ono przekazywane
do generatora na wejście.

Etap generowania polega na utworzeniu instancji `Rubinius::Generator`
i wywołaniu na głównym węźle drzewa AST metody, która wygeneruje
bajtkod na obiekcie `Generator`.

Klasa `Generator` zapewnia mini język DSL (napisany w "czystym" Ruby)
służący do generowania bajtkodu Rubiniusa. `Generator` udostępnia
metody, które przekładają się bezpośrednio na [instrukcje dla maszyny
wirtualnej](/doc/pl/virtual-machine/instructions/). Dla przykładu aby
utworzyć instrukcję, która przekaże nil na stos możesz wywołać metodę
`push_nil` na instancji `Generatora`.

Klasa `Generator` zapewnia również kilka pomocnych metod, które
umożliwiają generowanie często występujących układów bajtkodu bez
zagłębiania się w szczęgóły implementacji niektórych instrukcji
maszyny wirtualnej.

Na przykład, aby wywołać metodę bezpośrednio przy użyciu bajtkodu
Rubiniusa musiałbyś utworzyć literał reprezentujący nazwę metody i
wywołać `send_stack` aby wywołać metodę. Dodatkowo jeśli chciałbyś
wywołać metodę prywatną najpierw musiałbyś utworzyć bajtkod, który to
umożliwi. Jeśli chciałbyś wywołać metodę `puts` bez argumentów,
zezwalając na wywołanie prywatne musiałbyś wywołać:

    # tutaj, g jest instancją Generatora
    g.allow_private
    name = find_literal(:puts)
    g.send_stack name, 0

Wykorzystując metodę pomocniczą `send`, możesz to zrobić prościej:

    g.send :puts, 0, true

Generując bajtkod dla Drzewa AST Rubinius wywołuje metodę `bytecode`
na każdym węźle drzewa, przekazując obecną instancję `Generatora` jako
argument. Poniżej metoda `bytecode` dla węzła `if`:

    def bytecode(g)
      pos(g)

      done = g.new_label
      else_label = g.new_label

      @condition.bytecode(g)
      g.gif else_label

      @body.bytecode(g)
      g.goto done

      else_label.set!
      @else.bytecode(g)

      done.set!
    end

Najpierw, metoda `bytecode` wywołuje metodę `pos`, która jest
zdefiniowana w podstawowej klasie `Node`. `pos` wywołuje `g.set_line
@line`. Jest to wykorzystywane przez maszynę wirtualną aby zapewnić
informacje odnośnie działającego kodu służące do debuggowania.

Następnie, kod korzysta z metod pomocniczych `label` klasy `Generator.
Podstawowy kod Rubiniusa nie ma żadnych struktur sterujących oprócz
kilku instrukcji goto (czyste `goto`, `goto_if_true` oraz
`goto_if_false`). Możesz używać formy uproszczonej `git` dla
`goto_if_true` oraz `gif` dla `goto_if_false`. W tym przypadku
tworzymy dwie etykiety (labels), jedną dla oznaczenia końca warunku
`if` a drugą dla oznaczenia początku bloku `else`.

Po utworzeniu tych dwóch etykiet, węzeł `if` wywołuje metodę
`bytecode` na swoim węźle warunku (@condition), przekazując aktualną
instancję generatora jako argument. To wywołanie spowoduje utworzenie
bajtkodu dla warunku.

Ten proces powinien umieścić wartość wykonania warunku na stosie, więc
węzeł `if` tworzy instrukcję `goto_if_false`, która spowoduje przeskok
do `else_label` jeśli warunek nie został spełniony. Następnie używamy
podobnego wzorca co poprzednio - generator wywołuje metodę `bytecode`
na węźle `@body`, co powoduje utworzenie bajtkodu dla @body. Po tym
następuje umieszczenie bezwarunkowego `goto`, które przenosi
wykonywanie programu na koniec bajtkodu całej instrukcji `if`.

Następnie, umieszczamy oznaczenie `else_label`. Dzięki rozdzieleniu tworzenia
oznaczenia od jego użycia możemy przekazać to oznaczenie do
`goto` zanim go wykorzystamy do oznaczenia lokalizacji. To bardzo
ważna możliwość, która ma podstawowe znaczenie dla wielu struktur kontrolnych.

Następnie prosimy węzeł `@else` o jego bajtkod oraz oznaczamy
lokalizację `done`.

Ten proces wykonuje się rekurencyjnie począwszy od głównego węzła
drzewa, poprzez wszystkie węzły. Wynikiem tego jest utworzenie w
obiekcie `Generator` bajtkodu reprezentującego całe drzewo.

Dobrym miejscem do sprawdzenia definicji wszystkich węzłow AST i ich
bajtkodu jest katalog `lib/compiler/ast`. Znajdziesz tam również
dobre, praktyczne przykłady wykorzystania API `Generatora`.

Po utworzeniu całego bajtkodu dla AST, `Generator` wywołuje następny
etap czyli kodowanie.

## Pliki

* *lib/compiler/generator_methods.rb*: Generowany plik zawierający metody
  odnoszące się do [instrukcji Rubiniusa](/doc/pl/virtual-machine-instructions/)
* *lib/compiler/generator.rb*: Definicja obiektu `Generator`.
  Klasa ta definiuje podstawowe metody generatora oraz metodą
  pomocnicze ułatwiające generowanie bajtkodu dla często występujących
  wzorców.
* *lib/compiler/ast*: Definicje wszystkich węzłów AST utworzone przez parser.

## Dostosowanie generowania

Naprostszym sposobem zmiany działania generatora jest utworzenie
dodatkowych metod które będą korzystać z podstawowych metod
dostarczanych przez klasę `Generator`.

Oprócz tego możesz zmienić klasę Generatora, która jest używana w
procesie kompilacji. Aby dowiedzieć się więcej o dostosowywaniu
poszczególnych etapów kompilacji do swoich potrzeb przeczytaj
[Modyfikacja procesu kompilacji](/doc/pl/bytecode-compiler/customization/).
