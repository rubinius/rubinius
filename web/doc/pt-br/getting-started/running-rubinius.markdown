---
layout: doc_pt_br
title: Executando Rubinius
previous: Instalando Rubinius
previous_url: getting-started/building
next: Solução de Problemas
next_url: getting-started/troubleshooting
---

Depois de seguir os passos para compilar (e possivelmente tendo instalado) o Rubinius, 
você pode verificar se ele está funcionando executando:

    rbx -v

O Rubinius normalmente funciona como o Ruby a partir da linha de comando. Por exemplo:

    rbx -e 'puts "Hello!"'

Para executar um arquivo ruby com nome 'code.rb':

    rbx code.rb

Para executar o IRB:

    rbx

Se você adicionou o diretório _bin_ do Rubinius ao seu PATH, o Rubinius deverá se 
comportar da mesma maneira que o MRI. Existem comando para `ruby`, `rake`, 
`gem`, `irb`, `ri`, e `rdoc`.

Você pode adicionar o diretório _bin_ do Rubinius ao seu PATH apenas quando você quiser 
utilizar o Rubinius. Dessa maneira, ele não irá interferir com outras instalações do 
Ruby, quando você não quizer utiliza-lo.
