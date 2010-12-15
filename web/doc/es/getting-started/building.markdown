---
layout: doc_es
title: Building Rubinius
previous: Requirements
previous_url: getting-started/requirements
next: Running Rubinius
next_url: getting-started/running-rubinius
translated: true
---

Usted puede crear y ejecutar Rubinius del directorio de origen. Usted no
necesita instalar Rubinius para ejecutarlo. Las instrucciones a continuación
se detallarán tanto en la instalación Rubinius y lo ejecute desde el
directorio de origen.

Rubinius LLVM utiliza para el compilador JIT. Rubinius depende de un
determinado versión de LLVM. Si ha instalado LLVM en su sistema, pasar el
`--skip-system` de la bandera para configurar la secuencia de comandos en las
instrucciones a continuación.

### Obtención de la Fuente

El código fuente Rubinius disponible como un archivo comprimido y como un
proyecto en Github.  Puede [descargar el tarball aquí](http://rubini.us/download/latest).

Para usar Git:


  1. Cambie su directorio de trabajo actual en el directorio de desarrollo
  2. `git clone git://github.com/evanphx/rubinius.git`


### Instalación Rubinius

Si usted está planeando sobre el uso de Rubinius para ejecutar la aplicación,
ésta es una buena opción. Sin embargo, también puede ejecutar Rubinius
directamente desde el directorio de origen.  Vea la sección siguiente para
obtener más detalles al respecto.

Le recomendamos que instale Rubinius a un lugar que no requiere `sudo` o
privilegios de superusuario. Para instalar Rubinius:


  1. `./configure --prefix=/path/to/install/dir`
  2. `rake install`
  3. Siga las instrucciones para agregar el directorio _bin_ Rubinius a su
     PATH

### Se ejecuta desde el directorio de origen

Si usted planea trabajar en Rubinius sí mismo, usted debe utilizar esta
opción.


  1. `./configure`
  2. `rake`

Si se acaba de probar Rubinius, siga las instrucciones para agregar la _bin_
directorio a su PATH.

Sin embargo, si está desarrollando Rubinius, usted NO debe agregar la _bin_
directorio a su PATH Rubinius porque el sistema de construcción recogerá el
`ruby` y `rake` vínculos con el ejecutable Rubinius. Rubinius necesita
una por separado ejecutable Ruby a sí mismo de arranque durante el proceso de
generación.

