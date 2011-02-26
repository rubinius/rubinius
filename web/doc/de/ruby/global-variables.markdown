---
layout: doc_de
title: Globale Variablen
previous: Klassenvariablen
previous_url: ruby/class-variables
next: Specs
next_url: specs
review: true
---

Syntaktisch gesehen ist eine globale Variable eine Variable, die mit einem `$`
beginnt. Globale Variablen sollen in einem Ruby Programm aus allen Kontexten
heraus verfügbar sein. Dennoch unterscheidet man drei Arten von globalen
Variablen: echte Globale, thread-lokale Globale und pseudo-Globale.

Echte Globale assoziieren einen Wert mit einem Global-Namen, wie z.B. 
`$LOAD_PATH`.

Thread-lokale Globale haben den gleichen Syntax wie globale Variablen, aber
es gibt unterschiedliche Versionen der Globalen für jeden Thread im laufenden
Ruby Prozess. Beispiele für thread-lokale Globale sind `$SAFE` und `$!`. Um zu
zeigen, dass diese Werte von dem aktuellen Thread abhängen, beachte folgendes
Codebeispiel:

    puts $SAFE

    Thread.new do
      $SAFE = 2
      puts $SAFE
    end

    puts $SAFE

Pseudo-Globale sind eine festgelegte Teilmenge von Namen, die nicht auf globale
Werte verweisen, sondern auf Werte im aktuellen Geltungsbereich, ähnlich der 
lokalen Variablen. Man bezeichnet sie deshalb als globale Variablen, weil sie 
mit einem Dollarzeichen beginnen; die Bezeichnung stiftet jedoch unnötig 
Verwirrung.

Alle pseudo-Globalen bauen auf eine primäre pseudo-Globale auf: `$~`. D.h. alle 
pseudo-Globalen greifen auf Teile von `$~` zu und wenn `$~` sich 
ändert, dann ändern sich alle pseudo-Globalen mit.

Abhängige pseudo-Globalen sind diese hier: `$&`, <code>$`</code> (backtick), 
`$'` (einfache Anführungszeichen), `$+` und `$1`, `$2`, `$3`, etc.

Ein kniffeliger Aspekt bei diesen Werten ist, dass sie strikt an den 
aktuellen Geltungsbereich gebunden sind. Ruby erlaubt jedoch, dass sie mit 
einem Alias versehen werden können, wie man in English.rb nachsehen kann.

Diese neuen Aliase fügen im Prinzip neue lokale Variablen in allen 
Geltungsbereichen ein, auch in denen, die bereits aktiv sind. Aus diesem Grund 
unterstützt Rubinius sie nicht vollständig. Anstelle dessen bieten wir nur die
Aliase, die es schon in English.rb gibt. Z.B. kann `$MATCH` anstelle von `$~`
verwendet werden, egal ob English.rb eingebunden ist oder nicht.
