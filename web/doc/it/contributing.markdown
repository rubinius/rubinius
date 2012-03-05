---
layout: doc_it
title: Contribuire a Rubinius
previous: Risoluzione dei problemi
previous_url: getting-started/troubleshooting
next: Comunicazione
next_url: contributing/communication
---

Il tuo contributo al progetto Rubinius è benvenuto. Ci sono molte cose da fare
per aiutarci. La cosa più importante è che facciate qualcosa che sia di vostro
interesse: è il modo migliore per trovare l'entusiasmo e l'energia necessari.

Se avete domande su Rubinius, il modo migliore per ottenere delle risposte è
contattarci sul canale IRC #rubinius su irc.freenode.net.

Qui di seguito trovate diverse idee per contribuire a Rubinius.


## Eseguite il vostro codice

Il vostro codice è spesso più ostico delle specifiche. Eseguite i vostri
progetti personali su Rubinius e segnalate eventuali problemi. Si veda 
[Come scrivere un ticket](/doc/it/how-to/write-a-ticket).


## Chiedete aiuto

Faremo tutto il possibile per aiutarvi, ma prima assicuratevi di aver fatto le
dovute ricerche, se possibile. Rubinius cerca di essere un progetto facile da
studiare, imparare, e, si spera, estendere.

Accetteremo e apprezzeremo sicuramente anche semplici segnalazioni di
malfunzionamenti, ma daremo la priorità ai ticket che contengono semplici
istruzioni per riprodurre il problema. Ancora meglio sono i ticket che
comprendono specifiche RubySpec che dimostrino il malfunzionamento e una patch
che lo risolva.


## Scrivete delle specifiche

  1. Eseguite `bin/mspec tag --list incomplete <dir>` per mostrare le 
     specifiche che sono state marcate come incomplete. Queste specifiche 
     potrebbero semplicemente necessitare di una revisione, o ci potrebbero
     essere specifiche mancanti per una classe particolare.

     NOTA: Potete specificare la pseudo-directory ':files' come \<dir\>, e
     verranno mostrati i tag per tutte le specifiche che si devono eseguire
     su Rubinius. Altrimenti potete specificare una qualsiasi sottocartella
     della cartella spec/ per elencare i tag per le specifiche contenute in
     quella sottocartella.

  2. Trovate dei comportamenti non specificati. Si veda [Come scrivere una
     specifica RubySpec](/doc/it/how-to/write-a-ruby-spec).


## Correggete specifiche errate

  1. Eseguite `bin/mspec tag --list fails <dir>` per mostrare le specifiche
     marcate come errate.

     NOTA: Potete specificare la pseudo-directory ':files' come \<dir\>, e
     verranno mostrati i tag per tutte le specifiche che si devono eseguire
     su Rubinius. Altrimenti potete specificare una qualsiasi sottocartella
     della cartella spec/ per elencare i tag per le specifiche contenute in
     quella sottocartella.

  2. Scegliete una specifica che vi sembra interessante e cercate di scrivere
     una patch che la faccia passare.


## Scrivete documentazione

Studiate il funzionamento di Rubinius e scrivete documentazione di alto
livello che aiuti gli altri ad imparare i dettagli implementativi.


## Pulite il codice

Fate riferimento alle [Linee guida per la scrittura del
codice](/doc/it/contributing/style-guide/).


## Fate manutenzione dei ticket

  * Rilanciate o chiudete vecchi ticket.
  * Scrivete piccoli casi di test che riproducano i malfunzionamenti.
    Verificate che esistano già specifiche RubySpec per il problema. Se non ne
    esistono, considerate di scriverne una.
