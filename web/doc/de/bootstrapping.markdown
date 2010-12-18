---
layout: doc_de
title: Bootstrapping
previous: Bausystem
previous_url: build-system
next: Virtuelle Maschine
next_url: virtual-machine
---

Bootstrapping nennt man den Prozess bei dem sämtliche Funktionalität
aufgebaut wird, die benötigt wird, sodass das System sämtlichen Ruby
code ausführen kann. Es gibt sieben Phasen beim Bootstrapping Prozess:

  1. VM: Die Virtuelle Maschine kann Bytecode laden und ausführen
     sowie Nachrichten versenden (d.h. Methoden finden und
     aufrufen). Alle primitiven Funktionen sind verfügbar aber noch
     nicht als Ruby methoden eingetragen.

     Die Klasse Class muss manuell eingerichtet werden. Dazu wird
     deren +class+ Eintrag auf sie selbst und deren +superclass+
     Eintrag auf Module gesetzt. Neben Class und Module werden
     desweiteren einige weitere Basisklassen erstellt, darunter
     Object, Tuple, LookupTable und MethodTable.

     Von dem Zeitpunkt an können Klassen definiert werden. Ca. 35
     eingebaute Klassen (built in classes) werden veranlasst sich
     selbst zu initialisieren, Symbole für die top level Methoden
     (:object_id, :call, :protected etc.) werden erstellt und
     grundlegende Exceptions werden definiert. Außerdem werden alle
     Primitiven Funktionen registriert. Schließlich wird das IO System
     eingerichtet und einige fundamentale Ruby methoden mit den
     entsprechenden Primitiven Funktionen verknüpft.

     Zu diesem Zeitpunkt ist genügend Funktionalität im System
     definiert um den Rest des Laufzeitsystems (Kernel) zu laden,
     welche komplett in Ruby geschrieben ist. Das Laden des Kernels
     geschieht in mehreren Pässen, bei der die Sprache wächst.

  2. alpha: Hier started das Laden des Ruby Codes. Das Öffnen von
     Klassen und Modulen sowie Definieren von Methoden
     funktioniert. Die Mindestfunktionalität um die folgenden Methoden
     zu unterstützen ist in kernel/alpha.rb implementiert:

       attr_reader :sym
       attr_writer :sym
       attr_accessor :sym
       private :sym
       protected :sym
       module_function :sym
       include mod

     Außerdem ist es möglich Exceptions zu werfen und den laufenden
     Prozess abzubrechen (beenden). Diese Phase stellt das Fundament
     für die beiden folgenden Phasen dar.

  3. bootstrap: Diese Phase fährt damit fort die Mindestfunktionalität
     einzurichten, die benötigt wird um platform und common zu
     laden. Zusätzlich werden die Primitiven Funktionen der meisten
     Kernel Klassen hinzugefügt.

  4. platform: Das FFI (foreign function interface) System wird
     eingerichtet und Ruby Methodenschnittstellen zu
     plattformabhängigen Funktionen werden erstellt. Sobald dies getan
     ist werden weitere plattformabhängige Dinge wie Pointer,
     Dateizugriff, die math Funktionen und POSIX Befehle eingehängt.

  5. common: Der Großteil der Ruby core Bibliothek und deren Klassen
     werden implementiert. Die Ruby core Klassen werden so
     Implementationsneutral wie möglich gehalten. Außerdem wird die
     meiste Funktionalität der Rubinius-spezifischen Klassen
     hinzugefügt.

  6. delta: Entgültige Versionen von Methoden wie #attr_reader
     etc. werden hinzugefügt. Implementationsspezifische Versionen von
     Methoden welche die in common bereitgestellten Versionen
     überschreiben werden ebenfalls in das System übernommen.

  7. loader: Die kompilierte Version von kernel/loader.rb wird ausgeführt.

     Die finale Phase richtet den Lebenszyklus eines Rubyprozesses
     ein. Dazu wird die VM mit dem System verbunden, Loadpaths werden
     gesetzt, Anpassungsskripte aus dem Heimverzeichnis werden gelesen
     und ausgeführt, System Signale werden gefangen und
     Kommandozeilenargumente verarbeitet.

     Danach wird entweder das übergebene Skript (Ruby Programmdatei)
     ausgeführt oder die interaktive Ruby Shell ausgeführt (IRB).
     Sobald dies beendet ist werden alle registrierten at_exit Blöcke
     ausgeführt, alle Objekte aufgeräumt (finalized) und der Prozess
     wird beendet.

## Ladereihenfolge

Die Dateien in den Kernel Verzeichnissen bootstrap, platform, common
und delta implementieren die entsprechenden bootstrapping Phasen, die
oben beschrieben wurden. Die Reihenfolge in der diese Verzeichnisse
geladen werden ist in runtime/index angegeben.

Wenn eine rbc Datei geladen ist wird Code im Script Level und in
Klassen- und Modulrümpfen ausgeführt. Zum Beispiel, beim Laden von

    class SomeClass
      attr_accessor :value
    end

wird der Aufruf zu #attr_accessor ausgeführt. Das setzt voraus, dass
jeglicher Code der in Script-, Klassen- oder Modulrümpfen definiert
ist vor der Datei geladen werden muss, die diesen ausführen/benutzen
möchte. Die Datei kernel/alpha.rb definiert den Großteil des Codes der
in Script- oder Modulrümpfen benötigt wird. Jedoch gibt es auch andere
Ladereihenfolgeabhängigkeiten zwischen einigen Dateien in platform,
common, delta und compiler.

Diese Ladereihenfolgeabhängigkeiten sind in den load_order.txt Dateien
beschrieben, die sich jeweils in jedem der kernel/\*\* Verzeichnissen
befinden. Falls Code verändert oder hinzugefügt wird, wodurch weitere
Ladereihenfolgeabhängigkeiten entstehen, müssen die entsprechenden
load_order.txt Dateien angepasst werden. Dabei muss eine Datei von der
eine andere Abhängt über dieser platziert werden. Außerdem muss bei
Hinzufügen einer neuen Datei in den Kernel Verzeichnissen diese
entsprechend in die load_order.txt des Ordners eingetragen werden. Die
Dateien werden dann beim Bauprozess in die entsprechenden runtime/\*\*
Verzeichnisse kopiert. Während jedem der oben genannten bootstrap
Prozesse lädt die VM die Dateien in den load_order.txt Dateien gemäß
ihrer Anordnung.
