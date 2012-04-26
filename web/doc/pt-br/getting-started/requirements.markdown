---
layout: doc_pt_br
title: Requisitos
previous: Primeiros Passos
previous_url: getting-started
next: Instalando Rubinius
next_url: getting-started/building
---

Certifique-se que você tem os seguintes programas e bibliotecas instalados. Veja também 
a subsessão abaixo para requerimentos especiais de seu sistema operacional em 
particular.

Abaixo existem sugestões para obter mais informações sobre os programas e bibliotecas 
necessarias para instalar o Rubinius. Seu sistemas operacional ou gerenciador de pacotes 
pode ter outros pacotes disponíveis.

  * [GCC and G++ 4.x](http://gcc.gnu.org/)
  * [GNU Bison](http://www.gnu.org/software/bison/)
  * [MRI Ruby 1.8.7+](http://www.ruby-lang.org/) Se o seu sistema não tiver o 
    Ruby 1.8.7 instalado, considere usar o [RVM](https://rvm.beginrescueend.com/)
    para instala-lo.
  * [Rubygems](http://www.rubygems.org/)
  * [Git](http://git.or.cz/)
  * [ZLib](http://www.zlib.net/)
  * pthread - A biblioteca pthread deve estar instalada no seu sistema operacional
  * [gmake](http://savannah.gnu.org/projects/make/)
  * [rake](http://rake.rubyforge.org/) `[sudo] gem install rake`


### Apple OS X

A maneira mais fácil de configurar o ambiente de instalação no Apple OS X é instalando o 
XCode Tools and Utilities. Uma vez instalado, você pode habilitar o modo de _crash 
reporting_ em: /Developer/Applications/Utilities/CrashReporterPrefs.app


### Debian/Ubuntu

  * ruby-dev or ruby1.8-dev
  * libreadline5-dev
  * zlib1g-dev
  * libssl-dev
