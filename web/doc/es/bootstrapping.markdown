---
layout: doc_es
title: Bootstrapping
previous: Build System
previous_url: build-system
next: Virtual Machine
next_url: virtual-machine
translated: true
---

Bootstrapping es el proceso de construcción de la funcionalidad del sistema
hasta todo el código de Ruby se puede ejecutar. Hay siete etapas en el proceso
de arranque:

  1. VM: La máquina virtual es capaz de cargar y ejecutar código de bytes,
     envíe Mensajes (es decir, buscar y ejecutar métodos), y todas las
     funciones primitivas están disponibles, pero aún no conectados como los
     métodos de Ruby.

     La clase de la clase tiene que configurar manualmente tan temprano en el
     proceso por el establecimiento de su clase para ser sí mismo y su
     superclase para módulo. En Además de la clase y el módulo, un par de
     clases base se crean otros aquí, incluyendo objetos, tupla, LOOKUPTABLE
     y MethodTable.

     Ahora que las clases se pueden definir, de 35 años o menos integrado en
     las clases se les dice que inicializar sí mismos, símbolos de los
     métodos de nivel superior (: object_id,: llamar, : protección, etc) se
     crean excepciones básicas se definen, y primitivas están registrados.
     Por último IO se engancha. También en esta etapa, varios métodos
     fundamentales de Ruby están vinculados a los primitivos.

     En este punto hay bastante definido el comportamiento de comenzar a
     cargar el resto del kernel en tiempo de ejecución que es todo lo
     definido en el rubí. Esto tiene que ser a cabo en varios pasos como el
     idioma crece.

  2. alpha: Se inicia la carga de código Ruby. La capacidad de abrir las
     clases y los módulos y definir métodos existe. La funcionalidad mínima
     admiten los siguientes métodos se implementa en el kernel / alpha.rb:

       attr_reader :sym
       attr_writer :sym
       attr_accessor :sym
       private :sym
       protected :sym
       module_function :sym
       include mod

     Además, es posible plantear las excepciones y hacer que el proceso que se
     ejecuta a salida. Esta etapa se sientan las bases para los próximos dos
     etapas

  3. bootstrap: Esta etapa continúa agregando la funcionalidad mínima a
     support loading platform and common. Las funciones primitivas se añaden
     para la mayoría de las clases del núcleo.

  4. platform: La FFI (interfaz de la función externa) del sistema se
     implementa y Interfaces de Ruby método para funciones específicas de la
     plataforma se crean. Una vez se trata de establecer, las cosas
     específicas de la plataforma, tales como indicadores, el acceso a
     archivos, opciones matemáticas, y POSIX se adjuntan.

  5. common: La gran mayoría de las clases fundamentales son la colección de
     Ruby implementado. Las clases fundamentales de Ruby se mantienen como la
     aplicación-neutral posible. Además, la mayoría de la funcionalidad para
     las clases específicas Rubinius se añade.

  6. delta: Las versiones finales de los métodos como # attr_reader, etc
     agregó. Por otra parte, versiones específicas de la implementación de los
     métodos que anulan las versiones previstas en común se añade.

  7. loader: La versión compilada del kernel / loader.rb se ejecuta.

     La etapa final configuraciones del ciclo de vida de un proceso de rubí.
     Comienza por conectar la máquina virtual al sistema, crea rutas de carga,
     y lee las secuencias de personalización desde el directorio de origen.
     Señala trampas, y procesos de argumentos de línea de comandos.

     Después de eso, o se ejecuta la secuencia de comandos que se le pasa la
     línea de comandos o botas hasta la cáscara de rubíes interactivo. Cuando
     eso termina, se ejecuta cualquier at_exit bloques que se habían
     registrado, finaliza todos los objetos, y salidas.


## Orden de Carga

Los archivos en el arranque del núcleo directorios, la plataforma común, y el
delta, poner en práctica las etapas bootstrapping respectiva arriba. El orden
en el que estos directorios se cargan se especifica en runtime/index.

Cuando un archivo de RBC se carga, el código a nivel de guión y en la clase o
módulo órganos que se ejecuta. Por ejemplo, cuando se carga

    class SomeClass
      attr_accessor :value
    end

la llamada al #attr_accessor se llevará a cabo. Esto requiere que cualquier
código de llamada en la escritura, la clase, o de los órganos del módulo debe
ser cargado antes de que el archivo que llama a el código es cargado. El
kernel / alpha.rb define la mayor parte del código que se necesarios en el
guión o el nivel de módulo. Sin embargo, otras dependencias para la carga
existen entre algunos de la plataforma, el delta común, y los archivos de
compilador.

Estas dependencias de orden de carga se abordan en el archivo ubicado
load_order.txt en cada uno de los kernel/\*\* directorios. Si modifica el
código que añade una carga Para la dependencia, debe editar los archivos
load_order.txt para colocar el dependían en el archivo sobre el archivo que
depende de él. Además, si se agrega un nuevo archivo a un de los directorios
del kernel, debe agregar el nombre del archivo a la load_order.txt archivo en
ese directorio. Estos archivos se copian los directorios runtime/\*\* durante
la creación. Durante cada una de las etapas de arranque anterior, la máquina
virtual las cargas de los archivos enumerados en load_order.txt en orden.
