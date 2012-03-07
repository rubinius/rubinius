---
layout: doc_it
title: Variabili Globali
previous: Variabili di Classe
previous_url: ruby/class-variables
next: Specifiche
next_url: specs
review: true
---

Dal punto di vista sintattico, una variabile globale è una variabile il cui
nome inizia con `$`. Le variabili globali sono visibili da ogni contesto di un
programma Ruby. Vi sono in realtà tre tipi diversi di variabili globali:
globali reali, globali thread-local, e pseudo globali.

Le variabili globali reali associano un valore a un nome globale, come
`$LOAD_PATH`.

Le variabili globali thread-local hanno la stessa sintassi delle globali
reali, ma ne esiste una versione diversa per ogni thread all'interno del
processo Ruby in esecuzione.
Esempi di variabili globali thread-locali sono `$SAFE` and `$!`. Per
verificare che questi valori dipendono dal thread, provate il codice seguente:

    puts $SAFE

    Thread.new do
      $SAFE = 2
      puts $SAFE
    end

    puts $SAFE

Le variabili pseudo globali sono un sottoinsieme di nomi che non fanno
riferimento a valori globali ma a valori nello scope corrente, come le
variabili locali. Ci si riferisce comunque ad esse come variabili globali
perché il loro nome inizia con il simbolo di dollaro; questa è fonte di
confusione per gli utenti.

Tutte le variabili pseudo globali dipendono dalla variabile pseuso globale
principale `$~` e quindi quando `$~` cambia, tutte le variabili pseudo
globali cambiano.

Le restanti variabili pseudo globali sono `$&`, <code>$`</code> (backtick),
`$'` (virgoletta singola), `$+`, and `$1`, `$2`, `$3`, ecc.

Un aspetto complicato di questi valori è che sono strettamente legati allo
scope corrente ma Ruby consente di riferirsi ad essi con degli alias, come
succede in English.rb.

Questi nuovi alias praticamente aggiungono nuove variabili locali a tutti gli
scope, anche a quelli già in esecuzione. Di conseguenza, non sono del tutto
supportati da Rubinius. Ci limitiamo a fornire gli alias presenti in
English.rb di default. Per esempio, si può utilizzare `$MATCH` al posto di
`$~`, a prescindere dall'avere incluso English.rb o no.
