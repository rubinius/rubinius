---
layout: doc_pl
title: Udział w projekcie
previous: Rozwiązywanie problemów
previous_url: getting-started/troubleshooting
next: Komunikacja
next_url: contributing/communication
---

Zachęcamy do udziału w projekcie, wszelkie poprawki są mile
widziane. Jest wiele rzeczy od których można zacząć, najważniejsze to
zrobić coś co Ciebie osobiście interesuje. To najlepszy sposób na
entuzjastyczne i pełne energii podejście do pracy.

Jeśli masz pytania odnośnie Rubiniusa, najlepszym sposobem na
uzyskanie odpowiedzi jest rozmowa z nami na kanale IRC #rubinius na irc.freenode.net.

Poniżej przedstawiamy kilka pomysłów dotyczących udziału w projekcie i
jego rozwoju.


## Uruchom swój kod

Twój kod jest często bardziej podstępny niż specyfikacje. Spróbuj
uruchomić swój ulubiony projekt w Rubiniusie i zgłoś problemy jeśli
takowe wystąpią. Zobacz [Jak zgłosić problem](/doc/pl/how-to/write-a-ticket).


## Poproś o pomoc

Zrobimy wszystko co w naszej mocy aby pomóc. Jeśli to możliwe
przeprowadź samemo wstępną analizę problemu. Rubinius stara się być
projektem łatwym do poznania, nauki oraz rozwoju.

Na pewno zaakceptujemy proste raporty błędów, jednak lepiej będzie
jeśli zamieścisz w swoim zgłoszeniu listę czynności, które należy
wykonać aby zaobserwować dany problem. Jeszcze lepsze od tego są
zgłoszenia, które zawierają specyfikację RubySpecs pokazującą problem
oraz łatę rozwiązującą go.


## Napisz specyfikację RubySpecs

  1. Wpisz `bin/mspec tag --list incomplete <dir>` aby zobaczyć
     niekompletne specyfikacje. Te specyfikacje mogą wymagać poprawek
     lub może brakować specyfikacji dla danej klasy.

        Ważne: Zamiast podania nazwy katalogu <dir> możesz użyć
        pseudo-katalogu ':files', który pokaże wszystkie niekompletne
        specyfikacje, które powinny działać na Rubiniusie. Możesz również
        jako <dir> podać dowolny podkatalog z katalogu spec/

  2. Znajdź funkcjonalność dla której nie powstały jeszcze
     specyfikacje. Zobacz [Jak napisać specyfikację Ruby Spec](/doc/en/how-to/write-a-ruby-spec).


## Popraw niedziałające specyfikacje

  1. Wpisz `bin/mspec tag --list fails <dir>` aby zobaczyć
  niedziałające specyfikacje.

        Ważne: Zamiast podania nazwy katalogu <dir> możesz użyć
        pseudo-katalogu ':files', który pokaże wszystkie niekompletne
        specyfikacje, które powinny działać na Rubiniusie. Możesz również
        jako <dir> podać dowolny podkatalog z katalogu spec/

  2. Wybierz interesującą Cię specyfikację i spróbuj napisać łatę,
     która ją poprawi.


## Napisz dokumentację

Spróbuj dowiedzieć się jak działa Rubinius i udokumentuj jego
działanie. To pomoże innym poznać szczegóły implementacji Rubiniusa.


## Przejrzyć zgłoszone problemy

  * Zamknij lub otwórz stare problemy (w zależności od tego czego wymagają).
  * Spróbuj opisać proste kroki wymagane do zaobserwowania danego
    problemu. Sprawdź czy istnieją już dla niego specyfikacje, jeśli
    nie rozważ możliwość ich napisania.
