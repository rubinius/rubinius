---
layout: doc_pt_br
next: Começando
next_url: getting-started
---

## O que é o Rubinius

Rubinius é uma implementação da [linguagem de programação
Ruby](http://ruby-lang.org).

Rubinius inclui uma máquina virtual de bytecode, um parser da sintaxe Ruby, um
compilador de bytecode, um coletor de lixo baseado em gerações, compilador de código de
maquina nativo just-in-time (JIT) e as bibliotecas padrão do Ruby.

Rubinius implementa atualmente a versão 1.8.7 do Ruby.


## Licença

Rubinius usa a licença BSD. Veja o arquivo LICENSE no código-fonte.


## Instalação

Rubinius executa em Mac OS X e em muitos sistemas Unix/Linux. Microsoft Windows
terá suporte em breve.

Para instalar Rubinius siga os passos abaixo. Para informações mais detalhadas
veja [Começando](/doc/pt-br/getting-started/).

1. `git clone git://github.com/rubinius/rubinius.git`
2. `cd rubinius`
3. `./configure --prefix=/path/to/install/dir`
4. `rake install`

Quando o processo de instalação terminar, adicione o diretório do executável do
Rubinius (bin) ao seu PATH.

Rubinius vem com RubyGems embutido e com as gems rake e rdoc pré-instaladas.
Para instalar a gem nokogiri, por exemplo, rode `rbx gem install nokogiri`
