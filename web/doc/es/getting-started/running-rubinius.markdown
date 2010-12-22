---
layout: doc_es
title: Ejecutando Rubinius
previous: Construyendo Rubinius
previous_url: getting-started/building
next: Resolviendo problemas
next_url: getting-started/troubleshooting
---

Una vez que usted ha seguido los pasos para la construcción (y
posiblemente la instalación) de Rubinius, puede comprobar que funciona
correctamente:


    rbx -v

Rubinius generalmente se comporta como Ruby en la línea de comandos. Por ejemplo:

    rbx -e 'puts "Hello!"'

Para ejecutar un archivo de Ruby llamado 'code.rb':

    rbx code.rb

Para ejecutar IRB:

    rbx

Si ha añadido el directorio bin de Rubinius a su PATH, Rubinius debe
comportarse tal como usted lo esperaría de MRI. Hay comandos para `ruby`,
`rake`, `gem`, `irb`, `ri`, y `rdoc`.

Usted puede agregar el directorio bin de Rubinius al PATH sólo en el
momento en que desea utilizar Rubinius. De esta manera, no va a
interferir con su instalación normal de Ruby cuando no deseé utilizar Rubinius.
