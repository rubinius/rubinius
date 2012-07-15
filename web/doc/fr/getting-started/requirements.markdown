---
layout: doc_fr
title: Pré-requis
previous: Démarrage
previous_url: getting-started
next: Compilation
next_url: getting-started/building
---

Assurez vous d'avoir les programmes et bibliothèques suivants installés.
Consultez aussi les sous-sections ci-dessous pour les pré-requis spécifiques
à votre système d'exploitation.

La liste suivante est un ensemble de suggestions pour avoir plus d'information
à propos des programmes et bibliothèques nécessaires pour compiler Rubinius.
Votre système d'exploitation ou gestionnaire de paquets peut avoir
d'autres paquets disponibles.

  * [GCC et G++ 4.x](http://gcc.gnu.org/)
  * [GNU Bison](http://www.gnu.org/software/bison/)
  * [MRI Ruby 1.8.7+](http://www.ruby-lang.org/) Si votre système n'a pas
    Ruby 1.8.7 installé, pensez à utiliser [RVM](https://rvm.beginrescueend.com/)
    pour l'installer.
  * [Rubygems](http://www.rubygems.org/)
  * [Git](http://git.or.cz/)
  * [ZLib](http://www.zlib.net/)
  * pthread - La bibliothèque pthread devrait être installée par votre système d'exploitation
  * [gmake](http://savannah.gnu.org/projects/make/)
  * [rake](http://rake.rubyforge.org/) `[sudo] gem install rake`


### Apple OS X

Le moyen le plus facile d'obtenir un environnement de build sur Apple OS X est d'installer les
Outils et utilitaires XCode. Une fois installés, vous pouvez activer le mode de rapport de plantages
développeur ici :

`/Developer/Applications/Utilities/CrashReporterPrefs.app`

### Debian/Ubuntu

  * `ruby-dev` ou `ruby1.8-dev`
  * `libreadline5-dev`
  * `zlib1g-dev`
  * `libssl-dev`
 
### Fedora/CentOS

  * `ruby-devel`
  * `readline-devel`
  * `zlib-devel`
  * `openssl-devel`

### FreeBSD

Rubinius a un portage dans l'arbre des portages FreeBSD. Il s'appelle `lang/rubinius`.
Vous pouvez trouver des informations à propos de ce portage sur [FreshPorts](http://freshports.org/lang/rubinius/).
Lors de son installation, le portage installe toutes les dépendances automagiquement.

