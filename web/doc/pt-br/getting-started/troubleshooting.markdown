---
layout: doc_pt_br
title: Solução de Problemas
previous: Executando Rubinius
previous_url: getting-started/running-rubinius
next: Contribuindo
next_url: contributing
---

Abaixo são alguns erros que você pode encontrar enquanto estiver compilando, 
instalando, ou executando o Rubinius e suas referentes sugestões de solução.

Para qualquer um dos erros, o primeiro passo é certificar que você possui a versão 
mais atualizada e sem alterações do repositório do Rubinius. Antes de seguir, 
considere executar os seguintes passos:

    $ git co master
    $ git reset --hard
    $ git pull
    $ rake distclean
    $ rake


### O Rubinius não consegue encontrar o diretório `runtime` 

  Após compilar ou instalar, os seguintes erros aparecem quando tentas executar o 
  Rubinius:

    ERROR: unable to find runtime directory

    Rubinius was configured to find the runtime directory at:

      /Users/brian/devel/rubinius/runtime

    but that directory does not exist.

    Set the environment variable RBX_RUNTIME to the location
    of the directory with the compiled Rubinius kernel files.

    You may have configured Rubinius for a different install
    directory but you have not run 'rake install' yet.

#### Solução:

  Se você configurou o Rubinius com `--prefix`, execute `rake install`.
	
  Se você configurou o Rubinus com `--prefix` e renomeou o diretório de instalação após 
  instalar o Rubinius, configure-o novamente e reinstale-o.
  
  Se você renomeou o diretório do código fonte após compilar o Rubinius, configure-o 
  novamente e recompile-o.

  Em geral, não renomei a pasta onde o Rubiniu foi compilado nem sua pasta de 
  instalação após compila-lo e instala-lo.


### Está ocorrendo erro de segfaults quando compilo o Rubinius no FreeBSD

  No FreeBSD, até a versão _8.1 stable_, existe um problema com o _execinfo_ que causa 
  o erro de _segfault_ no Rubinius quando ele carrega.
  
#### Solução:

  Desabilite o _execinfo_ quando for configurar o Rubinius:

    ./configure --without-execinfo
