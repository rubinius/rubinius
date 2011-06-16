---
layout: doc_pl
title: Etap parsowania
previous: Kompilator bajtkodu
previous_url: bytecode-compiler
next: AST
next_url: bytecode-compiler/ast
review: true
---

Pierwszym etapem kompilacji jest Ruby Parser. Parser dostaje na wejściu
String lub plik z kodem źródłowym i przekazuje AST (Abstract Syntax
Tree czyli Drzewo AST) do następnego etapu czyli do generatora.

Sam parser (zwany Melbourne) składa się z części napisanej w C (jest
to ten sam parser, który jest używany w MRI Matz Ruby Interpreter - czyli implementacji
referencyjnej Ruby napisanej przez Matz'a) oraz z części napisanej w
Ruby, która jest odpowiedzialna za utworzenie Drzewa AST w
Ruby. Parser w C komunikuje się z Ruby wywołując metodę dla każdego
węzła w drzewie AST.

Każda z tych metod posiada sygnaturę zawierającą wszystkie informacje
o przetwarzanej właśnie części drzewa. Na przykład jeśli kod zawiera
`if` C-parser wywoła metodę `process_if` przekazując numer linii,
warunek, sekcję kodu jeśli warunek jest spełniony oraz sekcję dla
niespełnionego warunku (jeśli taka istnieje).

    def process_if(line, cond, body, else_body)
      AST::If.new line, cond, body, else_body
    end

Listę wszystkich metod `process_` znajdzie w
`lib/melbourne/processor.rb` w kodzie Rubiniusa.

Zauważ, że w wielu przypadkach parser przekazuje wynik wywołania
poprzedniej metody `process_` jako argument do następnej metody
`process_`. Na przykład `true if 1` - parser najpierw wywoła
`process_lit(line 1)` oraz `process_true(line)`. Wywoła również
`process_nil(line)`, ponieważ drzewo zawiera `nil` jako sekcję
`else`. Następnie parser wywoła `process_if` z tym samym numerem
linii, wynikiem wykonania `process_lit`, wynikiem wykonania
`process_true` oraz wynikiem wykonania `process_nil`.

Ten proces w sposób rekurencyjny buduje strukturę drzewa, którą
Rubinius przekazuje do następnego etapu czyli do generowania.

## Pliki

* *lib/melbourne/processor.rb*: Interfejs Ruby do parsera napisanego w
  C. Ten plik zawiera metody zaczynające się od `process_`, które
  parser C wywołuje dla każdego węzła w drzewie AST.
* *lib/compiler/ast/\**: definicje dla każdego węzła drzewa AST
  używane przez powyższy processor Melbourne.

## Dostosowanie kompilacji

Istnieją dwa sposoby na dostosowanie tego etapu w procesie
kompilacji do własnych potrzeb. Najprostszym sposobem jest utworzenie
[Transformacji AST](/doc/pl/bytecode-compiler/transformations/).

Oprócz tego, możesz również utworzyć klasę która będzie dziedziczyć z
klasy Melbourne processor i zdefiniować własne handlery dla metod
`process_`. To jest zaawansowany temat, który nie został jeszcze udokumentowany.

