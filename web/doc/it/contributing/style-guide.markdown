---
layout: doc_it
title: Linee guida per la scrittura del codice
previous: Comunicazione
previous_url: contributing/communication
next: Ruby
next_url: ruby
---

Le seguenti linee guida puntano a mantenere il codice di Rubinius
manutenibile. Quando in dubbio su una delle linee guida, fate le vostre
domande sul canale IRC #rubinius su irc.freenode.net.


## Tutto il codice

  * Configurate il vostro editor per l'inserimento di spazi al posto dei tab
  * I tab vanno sostituiti con 2 spazi
  * Aggiungete un carattere di fine riga alla fine di ogni file


## Codice C++

  * Nessuno spazio tra le istruzioni condizionali e le parentesi
      Usate `if(1)` NON `if (1)`

  * Inserite la parentesi graffa sulla stessa riga della dichiarazione di 
    funzione o dell'istruzione condizionale.

  * Utilizzate sempre le parentesi graffe, anche quando potrebbero essere
    omesse.

  * Preferite le parentesi alle regole di precedenza implicite (nei limiti del
    possibile).

  * Versioni alternative di una funzione devono avere nomi che rispecchino il
    motivo per cui differiscono dalla funzione principale. Se esiste una
    funzione 'person()' e volete una versione che riceva come parametro il
    nome della persona, questa va chiamata 'person_with_name(char \*name)' o
    'person_with_details(char \*name, ...)'. NON 'person1(char \*name).


## Codice Ruby

  * Metodi: Cercate di mantenere i metodi brevi--una schermata e cercate di
    rispettare il principio DRY (Don't Repeat Yourself, Non Ripetetevi) nei
    limiti del possibile. In genere le funzionalità di uso comune vanno
    astratte all'interno di metodi helper (che possono essere resi privati) ma
    in alcuni casi, in particolare lavorando con il Core, cercare di non
    ripetersi è solo un ostacolo se dovete, ad esempio, aggirare diverse
    condizioni d'errore.

  * Nomi dei metodi: dovrebbero essere chiari, espressivi e significativi.
    Evitate l'utilizzo di trattini bassi (underscore) per 'proteggere' i
    metodi ('\_\_send\_\_'), a parte delle eccezioni.

  * I nomi dei metodi in stile Smalltalk vanno bene, nel senso che è possibile
    avere dei metodi come `SomeClass.make_from` da invocare in questo modo:
    `SomeClass.make_from file` or `SomeClass.make_from :file => name`.
    In questi casi il nome del parametro _completa_ il nome del metodo e rende
    la lettura del codice più naturale.

  * Nomi delle variabili: devono essere chiari e significativi (con qualche
    nota eccezione come l'utilizzo di 'i' come contatore). Cercate di evitare
    nomi di variabili uguali a nomi di metodo. Ad esempio, all'interno di
    Array utilizzate 'idx' anziché 'index' in quanto questo è già un nome di
    metodo.

  * Postcondizioni: utilizzate postcondizioni solo *se* l'espressione occupa
    una sola riga *e* non ci sono troppe condizioni.

  * Blocchi: Utilizzate `do ... end` o `{...}`, con degli spazi tra i
    delimitatori e le istruzioni (`foo { |arg| code }`). Dividete le
    espressioni lunghe o complesse su più righe, così:

        mapped = foo.map do |elem|
          do_something_with elem
        end

  * Definizioni di Moduli/Classi con qualificatori di visibilità:

        module Rubinius::Profiler
          class Sampler
          end
        end

## Codice del kernel

Le linee guida principali per il codice del kernel sono la semplicità e
l'efficienza. Il codice semplice è spesso più efficiente e generalmente più
comprensibile. Non andrebbe usata la metaprogrammazione nel bootstrap.
Utilizzate i metodi #attr_xxx per tutto il codice del kernel. Inoltre, le
chiamate al metodo #alias_method per la creazione di metodi alias vanno poste
subito dopo la definizione del metodo. I metodi privati vanno specificati
chiamando il metodo `private :sym` subito dopo la definizione del metodo.
Ricordate che le versioni dei metodi sopra citati nello stage alpha ricevono
un singolo simbolo come parametro.

## Documentazione

  * Utilizzate RDoc per la documentazione del codice Ruby.

  * Utilizzate Doxygen per la documentazione del codice C++.

  * Utilizzate Markdown per la documentazione nella cartella doc/. Si veda
    [La sintassi Markdown](http://daringfireball.net/projects/markdown/syntax)
    Impostate la lunghezza delle righe a 78 caratteri e andate a capo
    manualmente.
