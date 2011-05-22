---
layout: doc_pt_br
next: Começando
next_url: getting-started
---

## O que é Rubinius

Rubinius é uma implementação da [linguagem de programação Ruby]
(http://ruby-lang.org).


Rubinius inclue uma máquina virtual de bytecodes, parser de sintax Ruby,
compilador de bytecode, gerenciador de garbage collector, compilador de
de código nativo just-in-time (JIT), e o Ruby Core e Bibliotecas padrão.
Rubinius implementa atualmente a versão 1.8.7 do Ruby.


## Licença

Rubinius usa a licença BDS. Veja o arquivo de LICENCA no código-fonte.

## Instalação
Rubinius executa em Mac OS X e muitos sistemas Unix/Linux. Microsoft Windows
terá suporte em breve.

Para instalar Rubinius, siga os passos abaixo. Para informação mais detalhada
veja [Começando](/doc/pt-br/getting-started/).


1. `git clone git://github.com/evanphx/rubinius.git`
1. `cd rubinius`
1. `./configure --prefix=/path/to/install/dir`
1. `rake install`

Quando o processo de instalação terminar,  adicione o diretório executável
do Ruby (bin) ao seu PATH.

Rubinius vem com RubyGems e tem as gems rake e rdoc pré-instaladas.
Para instalar a gem nokogiri, por exemplo, rode `rbx gem install nokogiri`

