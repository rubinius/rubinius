---
layout: doc_pl
title: How-To - Pisanie testów wydajnościowych
previous: Fix a Failing Spec
previous_url: how-to/fix-a-failing-spec
next: Write a Blog Post
next_url: how-to/write-a-blog-post
---

Dlaczego piszemy testy wydajnościowe?

Testy wydajnościowe są świetnym narzędziem aby porównać Rubiniusa z innymi
implementacjami Rubiego np. MRI, JRuby, IronRuby itd. Celem testów wydajnościowych
nie jest badanie samego Rubiniusa, dlatego upewnij się, że Twoje testy spełniają
poniższe kroki:

  1.  Zapoznaj się z istniejącymi testami w katalogu rubinius/benchmarks,
      np. benchmark/core/hash/bench_merge.rb.
  2.  Każdy plik z testem powinien sprawdzać jedną specyficzną cechę Rubiego, np. różne
      sposoby usuwania kluczy/wartości z Hasha.
  3.  Użyj frameworka do testów wydajnościowych.
  4.  Napisz kod testu zwięźle i prosto.
  5. Testy wydajnościowe nie służą do badania samego Rubiniusa. Dlatego,
     jesli tworzysz testy dla klasy która ma metody z wykrzyknikiem i bez
     wykrzyknika, (tzw. non-bang i bang methods), użyjesz skopiwanej zmiennej
     dla metody z wykrzyknikiem, ale nie musisz kopiować zmiennej dla metody
     bez wykrzyknika.

Jeśli chcesz uruchomić testy możesz uruchomić ich plik lub cały katalog:

    bin/benchmark benchmark/core/string/bench_case.rb
    bin/benchmark benchmark/core
