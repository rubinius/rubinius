---
layout: doc_en
title: Kompilacja Rubiniusa
previous: Wymagania
previous_url: getting-started/requirements
next: Uruchomienie Rubiniusa
next_url: getting-started/running-rubinius
---

Aby uruchomić Rubiniusa nie jest konieczna jego iinstalacja, wystarczy
skorzystać z katalogu z kodem źródłowym. Poninższe instrukcje
przedstawiają zarówno sposób instalacji jak i uruchomienia z katalogu
z kodem źródłowym.x

Rubinius używa LLVM (Low Level Virtual Machine - Maszyny Wirtualnej
Niskiego Poziomu) jako kompilatora JIT (Just-In-Time). Rubinius polega
na szczególnej wersji LLVM, która została skompilowana z włączonym C++ RTTI
(run-time type information).  Skrypt `configure` automatycznie
zweryfikuje te wymagania szukając zainstalowanej wersji LLVM. Jeśli
posiadasz LLVM zainstalowaną w systemie i pomimo tego Rubinius nie
chce z nią z jakiegoś powodu współpracować wtedy dobrym rozwiązaniem
jest przekazanie parametu `--skip-system` do skryptu
`configure`. Parametr ten spowoduje, że pominięta zostanie wersja
systemowa LLVM podczas kompilacji, Rubinius użyje swojej wersji LLVM.

### Pobieranie kodu źródłowego

Kod źródłowy Rubiniusa dostępny jest jako archiwum tar oraz jako
projekt na Github'ie. Archiwum tar możesz [pobrać tutaj](http://rubini.us/download/latest).

Pobieranie źródeł przy pomocy Git'a:

  1. `cd docelowy_katalog`
  2. `git clone git://github.com/evanphx/rubinius.git`


### Instalacja Rubiniusa

Jeśli planujesz używać Rubiniusa do uruchamiania swojej aplikacja to
instalacja jest dobrym wyjściem. Możesz również uruchomić Rubiniusa
bezpośrednio z katalogu ze źródłami (szczegóły w następnej sekcji).

Zalecamy instalację w katalogu, który nie wymaga użycia polecenia
`sudo` lub uprawnień administratora (root). Aby zainstalować Rubiniusa:

  1. `./configure --prefix=/ścieżka/do/katalogu/instalacyjnego`
  2. `rake install`
  3. Postępuj zgodnie ze wskazówkami poniżej aby dodać katalog _bin_
  do zmiennej środowiskowej PATH


### Uruchamianie Rubiniusa ze katalogu źródłowego

Jeśli zamierzać pracować nad rozwojem kodu Rubiniusa ta opcja będzie
dobrym rozwiązaniem.

  1. `./configure`
  2. `rake`

Jeśli tylko zapoznajesz się z Rubiniusem, postępuj zgodnie ze
wskazówkami wyświetlanymi na ekranie aby dodać katalog _bin_ do zmiennej PATH.

Jeśli jednak pracujesz nad rozwojem Rubiniusa, NIE POWINIENEŚ dodawać
katalogu _bin_ do zmiennej PATH ponieważ podczas kompilacji Rubinius
użyje swoich wersji komend `ruby` oraz `rake` - Rubinius wymaga
dostępu do oddzielnej wersji Rubiego podczas kompilacji aby Rubinius
mógł się sam "zbudować" (skompilować i uruchomić).
