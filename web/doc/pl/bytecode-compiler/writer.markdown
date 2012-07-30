---
layout: doc_pl
title: Etap zapisu
previous: Etap pakowania
previous_url: bytecode-compiler/packager
next: Transformacje
next_url: bytecode-compiler/transformations
---

Gdy zakończył się etap pakowania i utworzony został obiekt
CompiledCode, Rubinius zapisuje tą metodą do pliku w celu
późniejszego użycia. Na przykład, po tym jak pierwszy raz ładujemy
plik, następne ładowanie nie będzie ładować już kodu Ruby i wykonywać
wszystkich poprzednich etapów tylko ładuje CompiledCode bezpośrednio
z pliku.

Etap zapisu jest bardzo prosty. Polega on na dodaniu do nazwy pliku z
kodem źródłowym przyrostka `c` oraz wywołaniu
Rubinius::CompiledFile.dump z obiektem CompiledCode z poprzedniego
etapu jako argumentem oraz nazwą pliku do którego nastąpi zapis.

Kiedy zapisywanie się zakończy etap ten zwraca to co dostał na wejściu
na wyjście (czyli CompiledCode). CompiledCode jest wartością
zwracaną gdy cały etap kompilacji się skończy.

## Pliki

* *lib/compiler/compiled_file.rb*: implementacja CompiledFile.
  do zapisu wywoływana jest metoda `CompiledFile.dump`.

## Dostosowanie zapisu

Tak właściwie ten etap (zapisu) jest opcjonalny, wykonywany tylko
wtedy gdy kompilowany jest plik. Kiedy kompilowany jest String,
(np. przy pomocy eval), ten etap jest pomijany. W takim wypadku
kompilator zatrzymuje się na etapie pakowania i zwraca CompiledCode.

Dzięki architekturze Rubiniusa łatwo jest dodać inne etapy na końcu
procesu kompilacji i jeśli tylko te etapy zwrócą podaną na wejściu
(lub zmienioną) CompiledCode wszystko będzie działać tak jak powinno.

Aby dowiedzieć się więcej o dostosowywaniu
poszczególnych etapów kompilacji do swoich potrzeb przeczytaj
[Modyfikacja procesu kompilacji](/doc/pl/bytecode-compiler/customization/).
