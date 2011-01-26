---
layout: doc_de
title: Rubinius ausführen
previous: Rubinius kompilieren
previous_url: getting-started/building
next: Problemlösungen
next_url: getting-started/troubleshooting
---

Sobald die angegebenen Schritte zum Kompilieren (und ggf. Installieren) von
Rubinius befolgt wurden, kann überprüft werden, ob alles richtig funktioniert:

    rbx -v

Rubinius funktioniert generell wie Ruby von der Kommandozeile. Zum Beispiel:

    rbx -e 'puts "Hello!"'

Um eine Ruby Datei 'code.rb' auszuführen:

    rbx code.rb

Um IRB auszuführen:

    rbx

Falls du das _bin_ Verzeichnis von Rubinius deinem PATH hinzugefügt hast, 
sollte es so funktionieren, wie man es auch bei MRI erwarten würde. Es gibt 
Befehle für `ruby`, `rake`, `gem`, `irb`, `ri` und `rdoc`.

Das _bin_ Verzeichnis solltest du nur dann deinem PATH hinzufügen, wenn du 
Rubinius aktiv einsetzen möchtest. Ansonsten kann es zu Konflikten mit anderen,
bereits installierten Rubies kommen.
