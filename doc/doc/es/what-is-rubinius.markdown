---
layout: doc_es
next: Getting Started
next_url: getting-started
translated: true
---

## ¿Qué es Rubinius?

Rubinius es una aplicación de la [Ruby Programming Language] (http://ruby-lang.org).

Rubinius incluye una máquina virtual de código de bytes, analizador de
sintaxis de Ruby, bytecode compilador, recolector de basura generacional,
justo a tiempo (JIT) nativo de la máquina compilador de código, y Ruby Core y
las bibliotecas estándar.

Rubinius actualmente implementa la versión 1.8.7 de Ruby.


## Licencia

Rubinius utiliza la licencia BSD. Vea el archivo de LICENSE en la fuente.


## Instalación

Rubinius funciona en Mac OS X y muchos sistemas Unix / Linux. Apoyo a la
Microsoft Windows estará disponible muy pronto.

Para instalar Rubinius, utilice los pasos siguientes. Para obtener información
más detallada, ver [Introducción] (/doc/es/getting-started/).

1. `git clone git://github.com/evanphx/rubinius.git`
1. `cd rubinius`
1. `./configure --prefix=/path/to/install/dir`
1. `rake install`

Cuando finalice el proceso de instalación, siga las instrucciones para agregar
la Rubinius ejecutables (bin) directamente a su PATH.

Rubinius viene con RubyGems incorporada y tiene joyas rastrillo y rdoc
pre-instalado. Para instalar la gema Nokogiri, por ejemplo, ejecutar `rbx gem
install Nokogiri`.
