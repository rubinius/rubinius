---
layout: doc_pt_br
title: Instalando Rubinius
previous: Requisitos
previous_url: getting-started/requirements
next: Executando Rubinius
next_url: getting-started/running-rubinius
---

Você pode compilar e executar o Rubinius a partir do código fonte. Você não precisa 
instalar o Rubinius para executa-lo. Abaixo você encontrará detalhes de como instalar e 
executa-lo a partir do código fonte.

O Rubinus usa o LLVM como o compilador JIT. O Rubinius depende de uma versão particular 
do LLVM e o LLVM precisa estar compilado com o C++ RTTI (_run-time type information_) 
habilitado. O script `configure` irá verificar automaticamente esse requerimento quando 
este buscar a versão instalada do LLVM. Se você já tiver o LLVM instalado em sua máquina 
e o Rubinius falhar em linkar com o LLVM por qualquer motivo, passe `--skip-system` como 
argumento do script `configure` nas instruções abaixo.

### Obtendo o código fonte

O código fonte do Rubinius está disponível como tarball e como projeto no Github.
Você pode [fazer o download do tarball
aqui](https://github.com/rubinius/rubinius/tarball/master).

Se preferir, utilize o Git:

  1. Altere o diretório atual para seu diretório de desenvolvimento.
  2. `git clone git://github.com/rubinius/rubinius.git`


### Instalando o Rubinius

Se você planeja utilizar o Rubinius para executar sua aplicação, essa é melhor opção. 
Contudo, você também pode rodar o Rubinius diretamente do codigo fonte. Veja a próxima 
sessão para maiores detalhes.

Nos recomendamos instalar o Rubinius em um local que não exija o uso de `sudo` or outros 
previlegio de _superuser_. Para instalar o Rubinius:

  1. `./configure --prefix=/path/to/install/dir`
  2. `rake install`
  3. Siga as instruções para adicionar a pasta _bin_ do Rubinius no seu PATH


### Executando a partir da pasta do Código Fonte

Se você planeja trabalhar diretamente no Rubinus, você deve fazer o seguinte:

  1. `./configure`
  2. `rake`

Se você estiver interessado em experimentar o Rubinius, siga as instruções de como adicionar a pasta _bin_ ao seu PATH.

Contudo, se você estiver desenvolvendo o Rubinius, você NÃO DEVE adicionar a pasta _bin_ 
ao seu PATH por que o próprio Rubinius irá fazer os link do `ruby` e `rake` ao 
executavel do Rubinius. O Rubinius precisa de uma outra versão do executavel do Ruby 
para fazer a inicialização do seu processo de compilação (_bootstrap_).
