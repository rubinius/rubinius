---
layout: doc_es
title: Requirements
previous: Getting Started
previous_url: getting-started
next: Building
next_url: getting-started/building
translated: true
---

Asegúrese de que tiene los siguientes programas y bibliotecas instaladas.
Véase también la subsecciones a continuación para los requisitos especiales
para su funcionamiento particular del sistema.

Las siguientes son sugerencias para obtener más información acerca de los
programas y las bibliotecas necesarias para construir Rubinius. Su sistema
operativo o paquete gerente puede tener otros paquetes disponibles.


  * [GCC and G++ 4.x](http://gcc.gnu.org/)
  * [GNU Bison](http://www.gnu.org/software/bison/)
  * [MRI Ruby 1.8.7+](http://www.ruby-lang.org/) If your system does not have
    Ruby 1.8.7 installed, consider using [RVM](http://rvm.beginrescueend.com/)
    to install it.
  * [Rubygems](http://www.rubygems.org/)
  * [Git](http://git.or.cz/)
  * [ZLib](http://www.zlib.net/)
  * pthread - The pthread library should be installed by your operating system
  * [gmake](http://savannah.gnu.org/projects/make/)
  * [rake](http://rake.rubyforge.org/) `[sudo] gem install rake`


### Apple OS X

La forma más fácil de conseguir un entorno de generación de Apple OS X es para
instalar el XCode de herramientas y utilidades. Una vez instalado, puede
habilitar el modo de desarrollador accidente presentación de informes en:
/Developer/Applications/Utilities/CrashReporterPrefs.app



### Debian/Ubuntu

  * ruby-dev or ruby1.8-dev
  * libreadline5-dev
  * zlib1g-dev
  * libssl-dev
