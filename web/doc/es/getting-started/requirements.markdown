---
layout: doc_es
title: Requisitos
previous: Primeros pasos
previous_url: getting-started
next: Construyendo Rubinius
next_url: getting-started/building
---

Asegúrese de que tiene los siguientes programas y bibliotecas instaladas.
Véase también la subsecciones siguientes para instrucciones
específicas para su sistema operativo.

Las siguientes son sugerencias para obtener más información acerca de los
programas y bibliotecas necesarias para construir Rubinius. Su sistema
operativo o su manejador de paquetes podría tener otros paquetes disponibles.

  * [GCC y G++ 4.x](http://gcc.gnu.org/)
  * [GNU Bison](http://www.gnu.org/software/bison/)
  * [MRI Ruby 1.8.7+](http://www.ruby-lang.org/) Si su sistema no
    tiene Ruby 1.8.7 instalado, considere utilizar [RVM](https://rvm.beginrescueend.com/)
    para instalarlo.
  * [Rubygems](http://www.rubygems.org/)
  * [Git](http://git.or.cz/)
  * [ZLib](http://www.zlib.net/)
  * pthread - La libreria pthread debe encontrarse instalada como parte de su sistema operativo.
  * [gmake](http://savannah.gnu.org/projects/make/)
  * [rake](http://rake.rubyforge.org/) `[sudo] gem install rake`


### Apple OS X

La forma más fácil de conseguir un entorno de construcción para Apple OS X es instalar
XCode Tools and Utilities. Una vez instalado, puede habilitar el envio de errores para
desarrolladores en: /Developer/Applications/Utilities/CrashReporterPrefs.app


### Debian/Ubuntu

  * ruby-dev o ruby1.8-dev
  * libreadline5-dev
  * zlib1g-dev
  * libssl-dev

### FreeBSD

Rubinius tiene un port en el árbol de ports de FreeBSD. Y se llama `lang/rubinius`. Puede
encontrar más información acerca de este port en <http://freshports.org>. Una vez
instalado el port, automáticamente se incluirán todas las dependencias.
