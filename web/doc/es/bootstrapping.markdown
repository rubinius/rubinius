---
layout: doc_es
title: Bootstrapping
previous: Sistema de Construcción
previous_url: build-system
next: La Máquina Virtual
next_url: virtual-machine
---

Bootstrapping es el proceso de evolución de la funcionalidad del sistema
hasta que todo el código Ruby se puede ejecutar. Hay siete etapas en
el proceso de arranque:

  1. VM: La máquina virtual es capaz de cargar y ejecutar bytecode,
     enviar mensajes (por ej. buscar y ejecutar métodos), en esta
     etapa todas las funciones primitivas están disponibles, pero aún
     no se encuentran disponibles como métodos de Ruby.

     La clase Class es configurada manualmente en esta estapa del proceso
     estableciendo su clase a sí misma (n. t. Class) y su superclase a
     Module. Además de Class y Module, se crean un par de clases base,
     incluyendo Object, Tuple, LookupTable, y MethodTable.

     Ahora que las clases se pueden definir, al rededor de 35 clases
     inicializadas, se crean símbolos para los métodos esenciales
     :object_id, :call, :protected, etc), se definen las excepciones
     básicas, y se registran las instrucciones primitivas. Por último se conecta
     el sistema de IO. Además en esta etapa varios métodos
     fundamentales de Ruby son ligados a las instrucciones primitivas
     que los implementan.

     En este punto hay bastante comportamiento definido como para empezar a
     cargar el resto del núcleo de ejecución que está completamente definido en
     ruby. Esto ha de ser hecho en varias etapas a medida que el lenguaje crece.


  2. alpha: Se inicia la carga del código Ruby. Se obtiene la capacidad de abrir
     clases, módulos y definir métodos. Se implementa la funcionalidad mínima
     para soportar los métodos siguientes en kernel/alpha.rb:

       attr_reader :sym
       attr_writer :sym
       attr_accessor :sym
       private :sym
       protected :sym
       module_function :sym
       include mod

     Además, es posible arrojar excepciones y hacer que el proceso que
     está corriendo termine. Esta etapa sienta las bases para las próximos dos
     etapas.

  3. bootstrap: Esta etapa continúa agregando la funcionalidad mínima para
     poder cargar platform y common. Las funciones primitivas se añaden
     para la mayoría de las clases del núcleo.

  4. platform: La FFI (interfaz para función externa) del sistema se
     implementa y las interfaces de los métodos para funciones
     específicas-de-la-plataforma se crean. Una vez establecido esto,
     se adjuntan las cosas específicas-de-la-plataforma, tales como punteros,
     el acceso a archivos, matemáticas, y comandos POSIX.

  5. common: Se implementa la gran mayoría de las clases fundamentales
     de Ruby. Las clases fundamentales de Ruby se mantienen lo
     más neutral posible de la implementación. Además, se agrega
     la mayoría la funcionalidad para las clases específicas de Rubinius.

  6. delta: Se agregan las versiones finales de los métodos como #attr_reader.
     Por otra parte, se agregan versiones específicas-de-la-implementación
     de los métodos que sobreescriben las versiones provistas en common.

  7. loader: La versión compilada de kernel/loader.rb se ejecuta.

     La etapa final configura el ciclo de vida de un proceso de Ruby.
     Comienza por conectar la máquina virtual al sistema, crea rutas de carga,
     y lee las secuencias de personalización desde el directorio home.
     Atrapa señales del sistema y procesa argumentos de la línea de comandos.

     Después de eso, o se ejecuta el script que se le pasa desde la
     línea de comandos o se inicia el intérprete interactivo de Ruby. Cuando
     eso termina, se ejecuta cualquier bloque at_exit registrado, finaliza
     todos los objetos, y termina la ejecución.


## Orden de Carga

Los archivos en el directorio kernel, bootstrap, platform, common, and delta,
implementan las etapas de carga respectivamente descritas arriba. El orden
en el cual estos directorios son cargados está especificado en runtime/index.

Cuando un archivo rbc se carga, el código a nivel de script y
en el cuerpo de clases y módulos, se ejecuta. Por ejemplo, cuando se carga

    class SomeClass
      attr_accessor :value
    end

la llamada a #attr_accessor se va a ejecutar. Esto require que cualquier
código llamado en los cuerpos de scripts, clases o módulos sea cargado
antes del archivo que llama al código.
kernel/alpha.rb define la mayor parte del código que será necesario a nivel
de script o módulo. Sin embargo, otras dependencias de orden de carga
existen entre algunos de los archivos de platform, common, delta, y los archivos del compilador.

Estas dependencias de orden de carga se abordan en el archivo
load_order.txt ubicado en cada uno de los directorios kernel/\*\* . Si modifica
código que añade una dependencia al orden de carga, debe editar los archivos
load_order.txt para colocar el archivo arriba del archivo que depende
de él. Además, si se agrega un nuevo archivo a uno de los directorios
del kernel, debe agregar el nombre del archivo al archivo load_order.txt en
ese directorio. Estos archivos se copian a los directorios runtime/\*\* durante
la construcción. Durante cada una de las etapas de arranque anteriores, la máquina
virtual carga los archivos enumerados dentro de load_order.txt en orden.
