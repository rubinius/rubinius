---
layout: doc_it
title: Ruby
previous: Linee guida per la scrittura del codice
previous_url: contributing/style-guide
next: Script
next_url: ruby/scripts
review: true
---

Per capire come Rubinius implementa Ruby è necessario comprendere diversi
concetti del linguaggio. Lo scopo di questo capitolo è introdurre le
funzionalità di Rubinius affrontando concetti del linguaggio Ruby che
dovrebbero essere già noti. La documentazione di Rubinius assume che abbiate
familiarità con Ruby o con i concetti relativi a macchine virtuali e
compilatori.

Il concetto di _visibilità (scope)_ ha un ruolo centrale nella comprensione
degli argomenti che seguono. Nella sintassi di Ruby, la visibilità è in genere
un concetto derivato. In altre parole, non vi sono elementi della sintassi il
cui scopo principale sia delimitare o riferirsi allo scope. Per questo motivo,
parlare di visibilità può generare confusione. Ne è un esempio una normale
definizione di metodo:

    a = 5

    def diligent(a)
      puts a * 2
    end

In questo caso, il metodo `#diligent` ci fornisce un nome con cui riferirci
all'istruzione `puts a * 2`, ma definisce anche uno scope lessicale chiuso
per la variabile `a`. Lo scope è chiuso perché l'istruzione `a = 5` che
precede il metodo non ha nessuna relazione con la variabile `a` in
`#diligent`.

Spesso si afferma che tutto in Ruby è un oggetto. Non è del tutto vero.
_Quasi_ tutto in Ruby è un oggetto, ma alcuni elementi assolutamente 
essenziali per l'esecuzione di codice Ruby non sono necessariamente oggetti
su cui si possono mettere le mani. Quali tra questi "elementi dell'ambiente
di esecuzione" siano oggetti in Ruby dipende molto dall'implementazione. Lo
scope è uno di questi.

Essenzialmente, lo _scope_ come concetto è un contesto che consente di
rispondere a domande quali: Qual è il valore di `self` qui? Quali variabili
locali esistono qui? Quale valore avrà la costante `APPLE` in questo punto del
codice?

I seguenti elementi di Ruby sono trattati con lo scopo di comprendere come
vengono implementati da Rubinius e come il concetto di _scope_ sia coinvolto
in ciascuno.

1. [Script](/doc/it/ruby/scripts/)
1. [Metodi](/doc/it/ruby/methods/)
1. [Costanti](/doc/it/ruby/constants/)
1. [Classi e Moduli](/doc/it/ruby/classes-and-modules/)
1. [Blocchi e Proc](/doc/it/ruby/blocks-and-procs/)
1. [Variabili Locali](/doc/it/ruby/local-variables/)
1. [Variabili di Istanza](/doc/it/ruby/instance-variables/)
1. [Variabili di Classe](/doc/it/ruby/class-variables/)
1. [Variabili Globali](/doc/it/ruby/global-variables/)

