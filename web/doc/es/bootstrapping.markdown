---
layout: doc_es
title: Bootstrapping
previous: Build System
previous_url: build-system
next: Virtual Machine
next_url: virtual-machine
---

Bootstrapping es el proceso de construcción de la funcionalidad del sistema
hasta todo el código de Ruby que se puede ejecutar. Hay siete etapas en el proceso
de arranque:

  1. VM: La máquina virtual es capaz de cargar y ejecutar bytecode,
     enviar mensajes (por ej. buscar y ejecutar métodos), y todas las
     funciones primitivas están disponibles, pero aún no conectadas como
     métodos de Ruby.

     La clase Class tiene que configurar manualmente esto en el proceso
     estableciendo su clase  a sí misma (n. t. Class) y su superclase a Module. Además
     de Class y Module, se crean un par de clases base, incluyendo Object,
     Tuple, LookupTable, y MethodTable.

     Ahora que las clases se pueden definir, algo así como 35
     clases son llamadas para ser inicializadas a sí mismas,
     se crean símbolos para los métodos de alto nivel :object_id, :call,
     :protected, etc), se definen las excepciones básicas, y
     se registran las primitivas. Por último se conecta IO. Además en
     esta etapa se unen varios métodos fundamentales de Ruby

     En este punto hay bastante comportamiento definido como para empezar a
     cargar el resto del runtime kernel que está todo definido en
     ruby. Esto ha de ser hecho en varios pasos a medida que el lenguaje crece.


  2. alpha: Se inicia la carga del código Ruby. La capacidad de abrir las
     clases y los módulos y definir métodos existe. La funcionalidad mínima
     para soportear los métodos siguientez se implementa en kernel/alpha.rb:

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

  3. bootstrap: Esta etapa continúa agregando la funcionalidad mínima a
     poder cargar platform y common. Las funciones primitivas se añaden
     para la mayoría de las clases de Kernel.

  4. platform: La FFI (interfaz para función externa) del sistema se
     implementa y las interfaces de los método para plataformas-específicas
     se crean. Una vez establecido esto, se adjuntan las cosas
     específicas de la plataforma, tales como indicadores, el acceso a
     archivos, matemáticas, y comandos POSIX.

  5. common: La gran mayoría de las clases fundamentales de Ruby están
     implementadas. Las clases fundamentales de Ruby se mantienen lo
     más neutral posible de la implementación. Además, se agregan
     la mayoría la funcionalidad para las clases específicas de Rubinius.

  6. delta: Se agrega las versiones finales de los métodos como # attr_reader.
     Por otra parte, se agregan versiones específicas de la implementación
     de los métodos que sobreescriben las versiones provistas en common.

  7. loader: La versión compilada del kernel/loader.rb se ejecuta.

     La etapa final configura el ciclo de vida de un proceso de Ruby.
     Comienza por conectar la máquina virtual al sistema, crea rutas de carga,
     y lee las secuencias de personalización desde el directorio home.
     Parsea señales y procesa argumentos de la línea de comandos.

     Después de eso, o se ejecuta el script que se le pasa desde la
     línea de comandos o bootea el intérprete interactivo de Ruby. Cuando
     eso termina, se ejecuta cualquier bloque at_exit registrado, finaliza
     todos los objetos, y sale..


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
