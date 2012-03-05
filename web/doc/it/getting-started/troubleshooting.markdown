---
layout: doc_it
title: Risoluzione dei problemi
previous: Eseguire Rubinius
previous_url: getting-started/running-rubinius
next: Contribuire a Rubinius
next_url: contributing
---

Quelli che seguono sono errori che potreste riscontrare durante la
compilazione, installazione o esecuzione di Rubinius, con i relativi
suggerimenti per risolverli.

Per ogni errore, il primo passo è assicurarsi di avere una versione recente
e pulita dei sorgenti di Rubinius. Prima di proseguire, provate ad eseguire
questi comandi:

    $ git checkout master
    $ git reset --hard
    $ git pull
    $ rake distclean
    $ rake


### Rubinius non è in grado di trovare la cartella `runtime`

  Dopo la compilazione o l'installazione, tentando di eseguire Rubinius si
  ottiene il seguente errore:

    ERROR: unable to find runtime directory

    Rubinius was configured to find the runtime directory at:

      /Users/brian/devel/rubinius/runtime

    but that directory does not exist.

    Set the environment variable RBX_RUNTIME to the location
    of the directory with the compiled Rubinius kernel files.

    You may have configured Rubinius for a different install
    directory but you have not run 'rake install' yet.

#### Soluzione:

  Se avete configurato Rubinius con l'opzione `--prefix`, eseguite
  rake install.

  Se avete configurato Rubinius con l'opzione `--prefix` e rinominato la
  cartella di installazione dopo aver installato Rubinius, configurate
  Rubinius nuovamente e reinstallatelo.

  Se avete rinominato la cartella dei sorgenti dopo aver compilato Rubinius,
  configuratelo nuovamente e procedete con una nuova compilazione.

  In generale, non rinominate la cartella di compilazione o installazione dopo
  aver compilato o installato Rubinius.


### Rubinius presenta un segmentation fault durante la compilazione su FreeBSD

  Su FreeBSD, compresa la versione stabile 8.1, c'è un problema con execinfo
  che causa un segmentation fault al caricamento di Rubinius.

#### Soluzione:

  Disabilitate execinfo quando configurate Rubinius:

    ./configure --without-execinfo
