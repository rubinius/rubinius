---
layout: doc_fr
next: Démarrage
next_url: getting-started
---

## Qu'est-ce que Rubinius ?

Rubinius est une implémentation du [langage de programmation Ruby](http://www.ruby-lang.org/fr/).

Rubinius inclut une machine virtuelle pour code intermédiaire, un analyseur de syntaxe
Ruby, un compilateur de code intermédiaire, un ramasse-miettes générationnel, un compilateur
JIT (en temps réel) de code machine natif, et les bibliothèques du coeur Ruby et standard.

Rubinius implémente actuellement la version 1.8.7 de Ruby.


## Licence

Rubinius utilise la licence BSD. Voir le fichier `LICENSE` dans les sources.


## Installer

Rubinius fonctionne sur Max OS X et de nombreux systèmes d'exploitation Unix/Linux.
Le support de Microsoft Windows est à venir prochainement.

Pour installer Rubinius, suivez les instructions suivantes. Pour des informations plus détaillées,
lisez le chapitre [Démarrage](/doc/fr/getting-started/).

1. `git clone git://github.com/evanphx/rubinius.git`
1. `cd rubinius`
1. `./configure --prefix=/path/to/install/dir`
1. `rake install`

Quand le processus d'installation se termine, suivez les indications pour ajouter
le répertoire des exécutables Rubinius (`bin`) à votre variable d'environnement `PATH`.

Rubinius est fourni avec RubyGems intégré, et les gems `rake` et `rdoc` pré-installées.
Pour installer la gem `nokogiri` par exemple, exécutez `rbx gem install nokogiri`.

