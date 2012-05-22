---
layout: doc_pt_br
title: Contribuindo
previous: Solução de Problemas
previous_url: getting-started/troubleshooting
next: Comunicação
next_url: contributing/communication
---

Contribuidores para o projeto Rubinius são bem vindos. Existem diversas coisas que 
você pode ajudar. O mais imporante, você deve fazer alguma coisa que te interesse. Essa
é a melhor maneira de se manter entusiasmado.

Se você tiver perguntas sobre o Rubinius, a melhor maneira de conversar com a gente é 
no canal do IRC #rubinius no irc.freenode.net.

Abaixo são algumas ideias de coisas que você pode fazer para contribuir para o
Rubinius.


## Execute seu código

O seu código é normalmente mais agressivo que apenas nossos testes (specs). Execute 
seus projetos com o Rubinius e reporte problemas encontrados. Veja [Como Reportar uma
Falha](/doc/pt-br/how-to/write-a-ticket).


## Procure ajuda

Qualquer coisa que possamos fazer para te ajudar, nos iremos fazer. Certifique-se de 
fazer a sua própria pesquisa também, se possível. O Rubinius procura se um projeto que 
você pode facilmente estudar, aprender, e possivelmente ajudar.

Nos aceitamos e agredecemos que você reporte falhas, mas nos podemos dar mais 
prioridade aos reportes que incluem passo-a-passos simples que nos permite reproduzir 
o problema. Melhor ainda são reportes que incluem RubySpecs que demonstram o bug e 
códigos de correção (_patch_) que corrigem o erro.


## Escreva Specs

  1. Execute `bin/mspec tag --list incomplete <dir>` para exibir os specs que foram
     marcados como incompletos. Esses specs podemos simplesmente precisar de revisão,
     ou pode não existir nenhum spec para uma determinada classe.

     NOTA: Você pode especificar o pseudo-diretório ':file' para \<dir\>, o qual irá 
     exibir tag para todos os specs que devem executar no Rubinius. Ou você pode
     especificar qualquer subdiretório do diretório spec/ para listar as tags dos specs
     naquele subdiretório.

  2. Encontre comportamentos não especificados. Veja [Como Escrever um RubySpec](/doc/pt-br/how-to/write-a-ruby-spec).


## Corrigir Specs que estão falhando

  1. Execute `bin/mspec tag --list fails <dir>` para exibir os specs que estão 
  	 falhando.

     NOTA: Você pode especificar o pseudo-diretório ':file' para \<dir\>, o qual irá 
     exibir tag para todos os specs que devem executar no Rubinius. Ou você pode
     especificar qualquer subdiretório do diretório spec/ para listar as tags dos specs
     naquele subdiretório.

  2. Escolha um spec que seja interessante e veja se você consegue escrever um código
     de correção (_patch_) que faça o teste passar.


## Escreva Documentação

Estude como o Rubinius funciona e escreva documentação de alto nível que irá ajudar 
outros a aprender sobre os detalhes de implementação do Rubinius.


## Triagem de Reportes de Erros (tickets)

  * Reviva ou feche tickets antigos.
  * Construa casos de testes pequenos que reproduza os erros apresentados. E veja se
  	já não existe um RubySpec para aquele problema. Se não, considere escrever um (ou
  	mais de um, caso seja necessário).
