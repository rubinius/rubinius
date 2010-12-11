---
layout: doc_es
title: Troubleshooting
previous: Running Rubinius
previous_url: getting-started/running-rubinius
next: Contributing
next_url: contributing
translated: true
---

Los siguientes errores son que usted puede encontrar mientras que la
construcción, instalación o ejecutando Rubinius junto con propuestas de
solución.

Para cualquier error, un primer paso es asegurarse de que tiene un pago
actual, limpia de Rubinius. Antes de seguir adelante, considere ejecutar los
siguientes pasos:


    $ git co master
    $ git reset --hard
    $ git pull
    $ rake distclean
    $ rake


Error:

    ERROR: unable to find runtime directory

    Rubinius was configured to find the runtime directory at:

      /Users/brian/devel/rubinius/runtime

    but that directory does not exist.

    Set the environment variable RBX_RUNTIME to the location
    of the directory with the compiled Rubinius kernel files.

    You may have configured Rubinius for a different install
    directory but you have not run 'rake install' yet.

Solución:

  Si ha configurado Rubinius con --prefix, ejecute `rake install`.

  Si ha configurado Rubinius con --prefix y cambió el nombre del directorio de
  instalación después de instalar Rubinius, volver a configurar Rubinius y
  volver a instalarlo.

  Si cambió el nombre del directorio de origen después de la construcción
  Rubinius, volver a configurar y la reconstrucción.

  En general, no cambiar el nombre de la fuente o el directorio de
  construcción después de la construcción Rubinius.
