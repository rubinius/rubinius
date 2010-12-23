---
layout: doc_pl
title: Rozwiązywanie problemów
previous: Uruchamianie Rubiniusa
previous_url: getting-started/running-rubinius
next: Udział w projekcie
next_url: contributing
---

Poniżej przedstawiliśmy błędy, które możesz napotkać podczas
kompilacji, instalacji czy uruchomienia Rubiniusa wraz z proponowanymi
rozwiązaniami.

Dla każdego błędu, który napotkasz radzimy upewnić się, że pracujesz z
najświeższym kodem Rubiniusa. Zanim rozpoczniesz dalsze poszukiwania
radzimy wykonać poniższe kroki aby pobrać i skompilować najnowszy kod Rubiniusa:

    $ git co master
    $ git reset --hard
    $ git pull
    $ rake distclean
    $ rake


Błąd:

    ERROR: unable to find runtime directory

    Rubinius was configured to find the runtime directory at:

      /Users/brian/devel/rubinius/runtime

    but that directory does not exist.

    Set the environment variable RBX_RUNTIME to the location
    of the directory with the compiled Rubinius kernel files.

    You may have configured Rubinius for a different install
    directory but you have not run 'rake install' yet.

Rozwiązanie:

  Jeśli skonfigurowałeś Rubiniusa z `--prefix`, uruchom `rake install`.

  Jeśli skonfigurowałeś Rubiniusa z `--prefix` i zmieniłeś nazwę
  katalogu instalacyjnego po instalacji Rubiniusa, zrekonfiguruj
  Rubiniusa i zainstaluj ponownie.

  Jeśli zmieniłeś nazwę katalogu źródłowego Rubiniusa, zrekonfiguruj
  go i zainstaluj ponownie.

  Ogólnie rzecz biorąc, staraj się nie zmieniać nazw katalogu
  źródłowego i instalacyjnego po skompilowaniu i instalacji Rubiniusa.
