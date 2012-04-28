---
layout: doc_it
title: Compilare Rubinius
previous: Requisiti
previous_url: getting-started/requirements
next: Eseguire Rubinius
next_url: getting-started/running-rubinius
---

Potete compilare ed eseguire Rubinious dalla cartella dei sorgenti. Non è
necessario installare Rubinius per eseguirlo. Le istruzioni seguenti
illustrano sia l'installazione di Rubinius sia la sua esecuzione dalla
cartella dei sorgenti.

Rubinius usa LLVM per il compilatore JIT. Rubinius dipende da una particolare
versione di LLVM e LLVM deve essere compilato con l'identificazione run-time 
dei tipi (RTTI) di C++ abilitata. Lo script `configure` verificherà
automaticamente questi requisiti al momento della ricerca di una versione
installata di LLVM. Se avete installato LLVM ma Rubinius non riesce a 
rilevarlo per qualche motivo, passate l'opzione `--skip-system` allo script
`configure`.

### Ottenere i sorgenti

I sorgenti di Rubinius sono disponibili come tarball e come progetto su
Github. Potete [scaricare il tarball
qui](https://github.com/rubinius/rubinius/tarball/master).

Per usare Git:

  1. Spostatevi nella vostra cartella di sviluppo
  2. `git clone git://github.com/evanphx/rubinius.git`


### Installare Rubinius

Se prevedete di utilizzare Rubinius per eseguire la vostra applicazione,
questa è la soluzione migliore. È possibile altrimenti eseguire Rubinius
direttamente dalla cartella dei sorgenti. Si veda il prossimo paragrafo per
maggiori dettagli.

Raccomandiamo l'installazione di Rubinius in una cartella che non richieda 
`sudo` o privilegi di amministratore. Per installare Rubinius:

  1. `./configure --prefix=/path/to/install/dir`
  2. `rake install`
  3. Seguite le istruzioni per aggiungere la cartella _bin_ di Rubinius al 
     vostro PATH


### Eseguire Rubinius dalla cartella dei sorgenti

Se intendete lavorare su Rubinius stesso, scegliete questa soluzione.

  1. `./configure`
  2. `rake`

Se volete semplicemente provare Rubinius, seguite le istruzioni per aggiungere
la cartella _bin_ di Rubinius al vostro PATH.

Se invece state sviluppando Rubinius, NON aggiungete la cartella _bin_
al vostro PATH, altrimenti il sistema di compilazione di Rubinius utilizzerà i
collegamenti agli eseguibili `ruby` e `rake` di Rubinius stesso.
Rubinius necessita di un eseguibile Ruby separato per il suo processo di
compilazione.
