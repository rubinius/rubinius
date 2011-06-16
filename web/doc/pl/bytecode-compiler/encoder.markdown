---
layout: doc_pl
title: Etap kodowania
previous: Etap generowania
previous_url: bytecode-compiler/generator
next: Etap pakowania
next_url: bytecode-compiler/packager
---

Po tym jak generator wygenerował bajtkod, musi odpowiednio go
zakodować. Ten etap jest bardzo prosty i służy głównie ewidencjonowaniu.

Etap kodowania jest odpowiedzialny za dwie rzeczy:

* zamianę strumienia instrukcji utworzonego w poprzednim etapie (gdy
  węzły AST wywoływały `bytecode) na sekwencję instrukcji.
* sprawdzenie czy stos nie przepełnia się lub w jakikolwiek inny
  sposób znajduje się w niewłaściwym stanie.

Etap ten wykonywany jest na głównym obiekcie `Generatora` jak również
na pomniejszych obiektach `Generatora` (tworzonych dla bloków, metod
oraz innych struktur sterujących).

Po wykonaniu powyższego, obiekt `Generatora` przekazywany jest do
następnego etapu czyli pakowania.

## Pliki

* *lib/compiler/generator.rb*: Metoda `encode` w tym pliku wykonuje
  większą część czynności wymienionych w tym etapie.

## Dostosowanie pakowania

Ponieważ ten etap jest bardzo prosty, nie jest konieczne jego
dostosowanie. Może wystąpić konieczność zmiany jego działania (np. w
celu profilowania lub wyświetlania na ekranie). Aby dowiedzieć się
więcej o dostosowywaniu poszczególnych etapów kompilacji do swoich potrzeb przeczytaj
[Modyfikacja procesu kompilacji](/doc/pl/bytecode-compiler/customization/).
