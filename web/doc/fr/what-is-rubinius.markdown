---
layout: doc_en
next: Démarrer
next_url: getting-started
---

## Qu'est-ce que Rubinius

Rubinius est une implémentation du [langage de programmation Ruby](http://ruby-lang.org).

Rubinius inclue une machine virtuelle, un analyseur de syntaxe, un
compilateur bytecode, un ramasse-miette générationnel, un compilateur de
code en langage natif juste-à-temps (JIT, pour Just-In-Time), et des
librairies Core et Standard Ruby.

Rubinius supporte actuelle Ruby 1.8.7


## Licence

Rubinius est sous licence BSD. Voir le fichier LICENSE présent dans le
source.


## Installer Rubinius


Rubinius tourne sous Max OS X et sous de nombreux systèmes Unix/Linux.
Le support pour Microsot Windows est pour bientôt.

Pour installer Rubinius, procédez comme ci-dessous. Pour plus
d'informations, veuillez vous rendre à la rubrique [Démarrer](/doc/en/getting-started/).

1. `git clone git://github.com/evanphx/rubinius.git`
1. `cd rubinius`
1. `./configure --prefix=/path/to/install/dir`
1. `rake install`


Quand le processus d'installation se termine, suivez les instructions
pour ajouter le dossier des exécutables à votre PATH.

Rubinius est fourni avec les librairies (ou gems) Rubygems, rake, et rdoc
préinstallés. Pour installer le gem Nokogiri, par exemple, exécutez `rbx gem install nokogiri`.
