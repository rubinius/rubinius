---
layout: doc_es
title: Resolviendo problemas
previous: Ejecutando Rubinius
previous_url: getting-started/running-rubinius
next: Aportando a Rubinius
next_url: contributing
---

Los siguientes son errores que usted podría encontrar durante la construcción, 
instalación o ejecución de Rubinius junto con propuestas de solución.

Para cualquier error, un primer paso es asegurarse de que tiene una copia actual, limpia
de Rubinius. Antes de seguir adelante, considere ejecutar los siguientes pasos:

    $ git checkout master
    $ git reset --hard
    $ git pull
    $ rake distclean
    $ rake


### Rubinius no puede encontrar el directorio `runtime`

  Después de construir o instalar Rubinius, ocurre el siguiente error al
  intentar ejecutar Rubinius:

    ERROR: unable to find runtime directory

    Rubinius was configured to find the runtime directory at:

      /Users/brian/devel/rubinius/runtime

    but that directory does not exist.

    Set the environment variable RBX_RUNTIME to the location
    of the directory with the compiled Rubinius kernel files.

    You may have configured Rubinius for a different install
    directory but you have not run 'rake install' yet.

#### Solución

  Si ha configurado Rubinius con --prefix, ejecute `rake install`.

  Si ha configurado Rubinius con --prefix y cambió el nombre del directorio de
  instalación después de instalar Rubinius, vuelva a configurar Rubinius y
  vuelva a instalarlo.

  Si cambió el nombre del directorio de origen después de la
  construcción de Rubinius, vuelva a configurar y a construir.

  En general, no cambie el nombre del directorio de instalación
  después de la instalación de Rubinius o el nombre del directorio
  fuente de si realizo una construcción para desarrollador.

### La construcción de Rubinius termina en error de segmentación en FreeBSD

  En FreeBSD, incluyendo hasta la versión 8.1 estable, existe un problema con
  _execinfo_ que cause un error al iniciar Rubinius.

#### Solución

Deshabilite execinfo al configurar Rubinius:

     ./configure --without-execinfo
