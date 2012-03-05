---
layout: doc_it
title: Eseguire Rubinius
previous: Compilare Rubinius
previous_url: getting-started/building
next: Risoluzione dei problemi
next_url: getting-started/troubleshooting
---

Una volta seguite le istruzioni per la compilazione (ed eventualmente per 
l'installazione) di Rubinius, potete verificarne il funzionamento:

    rbx -v

Rubinius funziona generalmente come Ruby dalla riga di comando. Ad esempio:

    rbx -e 'puts "Hello!"'

Per eseguire un file Ruby chiamato 'code.rb':

    rbx code.rb

Per eseguire IRB:

    rbx

Se avete aggiunto la cartella bin di Rubinius al vostro PATH, Rubinius
dovrebbe comportarsi esattamente come vi aspettereste che facesse MRI.
Sono disponibili i comandi `ruby`, `rake`, `gem`, `irb`, `ri` e `rdoc`.

Potete aggiungere la cartella bin di Rubinius al vostro PATH solo quando
avete intenzione di utilizzarlo. In questo modo Rubinius non interferirà con
la vostra installazione di Ruby quando non volete utilizzarlo.
