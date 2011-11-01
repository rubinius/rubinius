---
layout: doc_es
title: Build System
previous: Specs - Compiler
previous_url: specs/compiler
next: Bootstrapping
next_url: bootstrapping
review: true
---

TODO: Faltan muchos detalles.

Rubinius utiliza Rake como sistema de construcción para sus propios archivos.
Sin embargo, Rubinius también incluye el código fuente de varias bibliotecas
externas y éstas utilizan comúnmente archivos para make.


## Construir para desarrollo o para instalación.

Rubinius consiste de los ejecutables y diversos archivos de soporte
como el núcleo y las bibliotecas estándar. Los ejecutables necesitan
saber dónde encontrar estos archivos, aún si se les mueve de
lugar. Para solucionar este problema, Rubinius distingue entre dos
tipos de construcción: desarrollo e instalación. Los ejecutables
almacenan la ruta completa a su directorio base, con respecto al cual
esperan encontrar los archivos que necesitan.

Los ejecutables de desarrollo almacenan la ruta al directorio del
código fuente en que fué construido. Se puede entonces mover el
ejecutable a otro lugar, pero tan pronto como se realicen cambios en
el fuente del núcleo en los directorios kernel/\*\*, el ejecutable
seguirá utilizando esos archivos.

Los ejecutables de instalación almacenan la ruta al directorio de
instalación. De nuevo, aún si los ejecutables se mueven a otro lugar,
seguiran encontrando los archivos instalados.

Esto, por supuesto, tiene consecuencias. Si se construye un ejecutable
para desarrollo y despues se cambia el nombre del directorio fuente,
necesitará reconstruír. De igual forma, si contruye un ejecutable para
instalación y renombra el directorio de instalación, el ejecutable
*no funcionará*,  *aún cuando el ejectuable esté dentro del directorio*.
Los ejecutables utilizan entonces, una sola ruta, almacenada
internamente. Si esto termina siendo un problema por alguna razón,
volveremos a evaluar otras alternativas.


## Instalando Rubinius

Para instalar Rubinius, primero debe configurar con un prefijo de instalación:

    ./configure --prefix=/path/to/install/dir

El proceso de configuración crea un archivo 'config.rb' que especifica
las rutas clave que utiliza Rubinius. Una vez configurado, ejecute
'rake install' para construir e instalar. El procedimiento de
instalación crea todos los archivos, incluyendo el núcleo de Ruby y
los archivos de la biblioteca estándar en el directorio lib /, a continuación, los
copia en el ubicación de instalación mediante el programa 'install'.
Las tareas de instalación se encuentran en rakelib/install.rake.
