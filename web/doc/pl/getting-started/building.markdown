---
layout: doc_pl
title: Kompilacja Rubiniusa
previous: Wymagania
previous_url: getting-started/requirements
next: Uruchomienie Rubiniusa
next_url: getting-started/running-rubinius
---

Aby uruchomić Rubiniusa nie jest konieczna jego instalacja, wystarczy
skorzystać z katalogu z kodem źródłowym. Poniższe instrukcje
przedstawiają zarówno sposób instalacji jak i uruchomienia z katalogu
z kodem źródłowym.

Rubinius używa LLVM (Low Level Virtual Machine - Maszyny Wirtualnej
Niskiego Poziomu) jako kompilatora JIT (Just-In-Time). Rubinius polega
na szczególnej wersji LLVM, która została skompilowana z włączonym C++ RTTI
(run-time type information).  Skrypt `configure` automatycznie
zweryfikuje te wymagania szukając zainstalowanej wersji LLVM. Jeśli
posiadasz już LLVM zainstalowaną w systemie i pomimo tego Rubinius nie
chce z nią z jakiegoś powodu współpracować wtedy dobrym rozwiązaniem
jest przekazanie parametu `--skip-system` do skryptu
`configure`. Parametr ten spowoduje, że pominięta zostanie wersja
systemowa LLVM podczas kompilacji, Rubinius użyje swojej wersji LLVM.

### Pobieranie kodu źródłowego

Kod źródłowy Rubiniusa dostępny jest jako archiwum tar oraz jako
projekt na Github'ie. Archiwum tar możesz [pobrać
tutaj](https://github.com/rubinius/rubinius/tarball/master).

Pobieranie źródeł przy pomocy Git'a:

  1. `cd docelowy_katalog`
  2. `git clone git://github.com/rubinius/rubinius.git`


### Instalacja Rubiniusa

Jeśli planujesz używać Rubiniusa do uruchamiania swojej aplikacji to
instalacja jest dobrym wyjściem. Możesz również uruchomić Rubiniusa
bezpośrednio z katalogu ze źródłami (szczegóły poniżej).

Zalecamy instalację w katalogu, który nie wymaga użycia polecenia
`sudo` lub uprawnień administratora (root). Aby zainstalować Rubiniusa
wykonaj poniższe polecenia:

  1. `./configure --prefix=/ścieżka/do/katalogu/instalacyjnego`
  2. `rake install`
  3. postępuj zgodnie ze wskazówkami wyświetlanymi na ekranie aby dodać katalog _bin_
  do zmiennej środowiskowej PATH


### Uruchamianie Rubiniusa z katalogu źródłowego

Jeśli zamierzasz pracować nad rozwojem kodu Rubiniusa ta opcja będzie
dobrym rozwiązaniem.

  1. `./configure`
  2. `rake`

Jeśli tylko zapoznajesz się z Rubiniusem, postępuj zgodnie ze
wskazówkami wyświetlanymi na ekranie aby dodać katalog _bin_ do zmiennej PATH.

Jeśli jednak pracujesz nad rozwojem Rubiniusa, NIE POWINIENEŚ dodawać
katalogu _bin_ do zmiennej PATH ponieważ podczas kompilacji Rubinius
użyje swoich wersji komend `ruby` oraz `rake` - Rubinius wymaga
dostępu do oddzielnej wersji Rubiego podczas kompilacji aby mógł
zostać skompilowany i uruchomiony.
