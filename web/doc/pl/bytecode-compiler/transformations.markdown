---
layout: doc_pl
title: Transformacje AST
previous: Etap zapisu
previous_url: bytecode-compiler/writer
next: Modyfikacja procesu kompilacji
next_url: bytecode-compiler/customization
---

Kompilator bajtkodu posiada prosty mechanizm transformacji AST, który
rozpoznaje niektóre węzły AST i je zamienia. Zamienione formy emitują
inny bajtkod niż te oryginalne. Kod źródłowy transformacji znajduje
się w lib/compiler/ast/transforms.rb

TODO: opisz architekturę pluginów kompilatora


### Transformacja Bezpiecznych Operacji Matematycznych

Ponieważ podstawowe biblioteki zbudowane są z tych samych bloków kodu
Rubiego co każdy inny kod i ponieważ Ruby jest bardzo dynamicznym
językiem z otwartymi klasami i mechanizmem późnego przypisania metod
(podczas wykonywania programu, ang. late binding), możliwa jest zmiana
działania podstawowych klas (np. Fixnum) w sposób który zburzy
działanie innych klas opierających się na nich. Dla przykładu,
wyobraźmy sobie poniższy kod:

  class Fixnum
    def +(other)
      (self + other) % 5
    end
  end

Z pewnością jest możliwe przedefiniowanie operacji dodawania aby
zachowywała się inaczej lecz sprawi to, że inne klasy (np. Array) nie
będą w stanie na przykład policzyć odpowiedniej długości. Dynamiczna
natura Rubiego jest kijem z dwoma końcami w tym przypadku.

W bibliotece standardowej 'mathn' przedefiniowuje Fixnum#/ w sposób
niebezpieczny i niekompatybilny. Biblioteka ta przypisuje Fixnum#/ to
Fixnum#quo, która to metoda zwraca domyślnie Float.

Z powyższych powodów specjalny plugin kompilatora emituje inną nazwę
metody gdy napotka metodę #/. Kompilator emituje #divide zamiast
#/. Klasy Fixnum, Bignum, Float oraz Numeric wszystkie definiują tą metodę.

Transformacja bezpiecznych operacji matematycznych jest włączona
podczas kompilacji podstawowych bibliotek. Podcza kompilacji kodu
użytkownika ta transformacja nie jest aktywna. Dzięki temu Rubinius
wspiera bibliotekę mathn bez stosowania niewygodnych praktyk.
