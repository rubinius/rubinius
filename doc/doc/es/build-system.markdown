---
layout: doc_es
title: Build System
previous: Specs - Compiler
previous_url: specs/compiler
next: Bootstrapping
next_url: bootstrapping
review: true
translated: true
---

TODO: Many details are missing.

Rubinius uniforme utiliza rastrillo como el sistema de compilación para sus
propios archivos. Sin embargo Rubinius también incluye la fuente de varias
bibliotecas externas y estos suelen utilizar los makefiles.


## Desarrollo contra Construye Instalar

Rubinius se compone de los archivos de soporte ejecutable y diversos como el
núcleo y las bibliotecas estándar. Las necesidades ejecutable para saber dónde
encontrar estos archivos incluso si se trasladan a otro lugar. Para manejar
este problema, Rubinius distingue dos tipos de generación: el desarrollo y la
instalación. Las tiendas ejecutable la ruta completa a su directorio base, en
relación a los que se espera encontrar el archivos que necesita.

Las tiendas de desarrollo ejecutable de la ruta de acceso al directorio de
origen en el que fue construido. A continuación, puede mover el archivo
ejecutable a una ubicación diferente, pero a medida que realizar cambios en
los archivos de base de la biblioteca en el kernel/\*\* directorios, la
ejecutable seguirá utilizando los archivos.

La instalación de tiendas ejecutable de la ruta de acceso al directorio de
instalación. Una vez más, incluso si el ejecutable se mueve a una nueva
ubicación, que seguirá para encontrar el instalado los archivos.

Esto tiene consecuencias, por supuesto. Si se construye un desarrollo
ejecutable y, a continuación cambiar el nombre de su directorio de origen,
tendrá que reconstruir. Del mismo modo, si usted construye un instalar el
ejecutable y cambiar el nombre del directorio de instalación, el ejecutable *
no * función, * incluso si el ejecutable está en el directorio *. Los
controles de un ejecutable único, en el código camino. Si esto termina siendo
muy doloroso por alguna razón, vamos a revisar.


## Instalación Rubinius

Para instalar Rubinius, primero debe configurar con un prefijo de instalación:

    ./configure --prefix=/path/to/install/dir

El proceso de configuración crea un archivo 'config.rb "que especifica el
archivo de clave caminos que Rubinius usos. Una vez configurado, ejecute
'instalar rastrillo "para construir y instalar. El procedimiento de
instalación crea todos los archivos, incluyendo la constitución del Ruby
archivos de la biblioteca estándar en el directorio lib /, a continuación, los
copia en el ubicación de instalación mediante el programa de la 'instalar'.
Las tareas de instalación se encuentran en rakelib/install.rake.
