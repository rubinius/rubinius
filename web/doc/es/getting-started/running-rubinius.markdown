---
layout: doc_es
title: Running Rubinius
previous: Building
previous_url: getting-started/building
next: Troubleshooting
next_url: getting-started/troubleshooting
translated: true
---

Una vez que usted ha seguido los pasos para la construcción (y posiblemente de
la instalación) Rubinius, puede comprobar que está trabajando:


    rbx -v

Rubinius generalmente funciona como Ruby de la línea de comandos. Por ejemplo:

    rbx -e 'puts "Hello!"'

Para ejecutar un archivo de Ruby llamado 'code.rb':

    rbx code.rb

Para ejecutar IRB:

    rbx

Si ha añadido el directorio bin Rubinius a su PATH, Rubinius debe realizar al
igual que se puede esperar de resonancia magnética. Hay comandos para `ruby`,
`rake`, `gem`, `irb`, `ri`, and `rdoc`.

Usted puede agregar el directorio bin al PATH Rubinius sólo cuando se desea
utilizar Rubinius. De esta manera, no va a interferir con su normal instalado
Ruby cuando no se desea utilizar Rubinius.
