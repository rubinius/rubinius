---
layout: doc_es
next: Primeros pasos
next_url: getting-started
---

## ¿Qué es Rubinius?

Rubinius es una implementación del [lenguaje de programación Ruby](http://ruby-lang.org/es/).

Rubinius incluye una máquina virtual para código intermedio (bytecode), analizador de
sintaxis de Ruby, compilador a código intermedio, recolector de basura generacional,
compilador JIT a código máquina nativo, el núcleo de Ruby y sus bibliotecas estándar.

Actualmente, Rubinius implementa la versión 1.8.7 de Ruby.


## Licencia

Rubinius utiliza la licencia BSD. Vea el archivo LICENSE en la
distribución del código fuente.


## Instalación

Rubinius funciona en Mac OS X y muchos sistemas Unix / Linux. Muy
pronto estará disponible tambien en plataformas Microsoft Windows.

Para instalar Rubinius, realize los siguientes pasos. Para obtener información
más detallada, vea [Primeros pasos](/doc/es/getting-started/).

1. `git clone git://github.com/rubinius/rubinius.git`
1. `cd rubinius`
1. `./configure --prefix=/path/to/install/dir`
1. `rake install`

Cuando finalice el proceso de instalación, siga las instrucciones para agregar
los ejecutables de Rubinius (bin) directamente a su variable de
ambiente PATH.

Rubinius incluye el manejador de paquetes RubyGems y viene con algunas
gemas pre-instaladas como rake y rdoc. Para instalar la gema Nokogiri, por
ejemplo, usted debe ejecutar `rbx gem install Nokogiri`.
