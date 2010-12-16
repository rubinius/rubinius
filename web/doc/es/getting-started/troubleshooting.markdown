---
layout: doc_es
title: Resolviendo problemas
previous: Ejecutando Rubinius
previous_url: getting-started/running-rubinius
next: Contributing
next_url: contributing
translated: true
---

Los siguientes son errores que usted podría encontrar durante la
construcción, instalación o ejecución de Rubinius junto con propuestas de
solución.

Para cualquier error, un primer paso es asegurarse de que tiene una
copia actual, limpia de Rubinius. Antes de seguir adelante, considere ejecutar los
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
  instalación después de instalar Rubinius, vuelva a configurar Rubinius y
  vuelva a instalarlo.

  Si cambió el nombre del directorio de origen después de la
  construcción de Rubinius, vuelva a configurar y a construir.

  En general, no cambie el nombre del directorio fuente donde realizó
  la construcción de Rubinius después de que esta ha terminado.
