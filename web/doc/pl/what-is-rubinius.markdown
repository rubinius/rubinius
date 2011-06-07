---
layout: doc_pl
next: Pierwsze kroki
next_url: getting-started
---

## Czym jest Rubinius ?

Rubinius jest implementacją [języka programowania Ruby](http://ruby-lang.org).

Rubinius składa się z maszyny wirtualnej opartej na bajtkodzie,
parsera składni, kompilatora bajtkodu, pokoleniowego odśmiecacza
pamięci, kompilatora JIT oraz z podstawowych i rozszerzonych bibliotek
języka Ruby.

Rubinius obecnie implementuje Ruby w wersji 1.8.7.


## Licencja

Rubinius jest na licencji BSD. Więcej na ten temat znajdziesz w pliku
LICENSE w źródłach Rubiniusa.


## Instalacja

Rubinius działa na systemach Mac OS X oraz wielu systemach
Unix/Linux. Wsparcie dla systemu Microsoft Windows jest planowane wkrótce.

Aby zainstalować Rubiniusa wykonaj poniższe kroki. Aby dowiedzieć się
więcej przeczytaj [Pierwsze kroki](/doc/pl/getting-started/).

1. `git clone git://github.com/rubinius/rubinius.git`
1. `cd rubinius`
1. `./configure --prefix=/path/to/install/dir`
1. `rake install`

Gdy proces instalacji zakończy się, przeczytaj instrukcje wyjaśniające
w jaki sposób dodać katalog bin do zmiennej PATH w systemie.

Rubinius zawiera już w sobie bibliotekę RubyGems, rake oraz rdoc. Aby
zainstalować inną bibliotekę (np. nokogiri) wpisz `rbx gem install nokogiri`.
