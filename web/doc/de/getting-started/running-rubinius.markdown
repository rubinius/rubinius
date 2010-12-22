---
layout: doc_de
title: Rubinius ausführen
previous: Rubinius bauen
previous_url: getting-started/building
next: Problemlösungen
next_url: getting-started/troubleshooting
---

Sobald die angegebenen Schritte zum Bauen (und ggf. Installieren) von
Rubinius befolgt wurden, kann überprüft werden, ob alles richtig
funktioniert:

    rbx -v

Rubinius funktioniert generell wie Ruby von der Kommandozeile. Zum Beispiel:

    rbx -e 'puts "Hello!"'

Um eine Ruby Datei 'code.rb' auszuführen:

    rbx code.rb

Um IRB auszuführen:

    rbx

Falls du das _bin_ Verzeichnis von Rubinius deinem PATH hinzugefügt
hast sollte es so funktionieren wie man es auch bei MRI erwarten
würde. Es gibt Befehle für `ruby`, `rake`, `gem`, `irb`, `ri` und
`rdoc`.

Du kannst das _bin_ Verzeichnis deinem PATH auch nur dann hinzufügen,
wenn du Rubinius benutzen möchtest. So wird es nicht mit dem normal
installierten Ruby in Konflikte geraten, wenn man Rubinius gerade
nicht nutzen möchte.