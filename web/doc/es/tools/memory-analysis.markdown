---
layout: doc_es
title: Análisis de Memoria
previous: Analizador
previous_url: tools/profiler
next: Cómos
next_url: how-to
review: true
---
Rubinius provee una interface que permite escribir el estado del heap actual a
un archivo para hacer análisis posterior. Varios proyectos complementarios
analizan estos dumps del heap y ayudan a descubrir fugas de memoria, colecciones
muy grandes y problemas con el código que podrían generar desperdicio
de memoria en tiempo de ejecución.

## Un Programa de Ejemplo

El siguiente código (que no incluye chequeo de errores) servirá como base
para un tour guiado a través del proceso de buscar una fuga de memoria en
Ruby, y a la vez, para encontrar una fuga en el sistema subyacente
que utiliza FFI.

Este código de ejemplo es un poco artificial pero sirve para ilustrar varios
problemas del mundo real.

    require 'rubygems'
    require 'ffi-rzmq'

    if ARGV.length < 3
      puts "usage: ruby leak.rb <connect-to> <message-size> <roundtrip-count>"
      exit
    end

    link = ARGV[0]
    message_size = ARGV[1].to_i
    roundtrip_count = ARGV[2].to_i

    ctx = ZMQ::Context.new
    request_socket = ctx.socket ZMQ::REQ
    reply_socket = ctx.socket ZMQ::REP

    request_socket.connect link
    reply_socket.bind link

    poller = ZMQ::Poller.new
    poller.register_readable request_socket
    poller.register_readable reply_socket

    start_time = Time.now

    message = ZMQ::Message.new("a" * message_size)
    request_socket.send message, ZMQ::NOBLOCK
    i = roundtrip_count
    messages = []

    until i.zero?
      i -= 1

      poller.poll_nonblock

      poller.readables.each do |socket|
        message = ZMQ::Message.new
        socket.recv message, ZMQ::NOBLOCK
        messages << message
        socket.send ZMQ::Message.new(message.copy_out_string), ZMQ::NOBLOCK
      end
    end

    elapsed_usecs = (Time.now.to_f - start_time.to_f) * 1_000_000
    latency = elapsed_usecs / roundtrip_count / 2

    puts "mean latency: %.3f [us]" % latency
    puts "received #{messages.size} messages in #{elapsed_usecs / 1_000_000} seconds"

Vaya, este programa tiene más fugas que un cedazo. Veamos por qué.

## Guardando un Head Dump

Rubinius provee acceso a la VM mediante una interfaz de agente. El agente abre
un socket en la red y responde a comandos enviados desde el programa de consola.
El agente debe ser iniciado con el programa.

    rbx -Xagent.start <script name>

Ejecute el programa de prueba con el agente activado.

    rbx -Xagent.start leak.rb

Conéctese al agente usando la consola de rbx. Este programa abre una sesión
interactiva con el agente que se está ejecutando dentro de la VM. Los comandos
son enviados al agente. En este caso, vamos a guardar un head dump para hacer un
análisis posterior.

Al iniciar, el agente crea un archivo en $TMPDIR/rubinius-agent.\<pid\> que
contiene algunos detalles importantes para la consola de rbx. Al salir, el agente
automáticamente elimina el archivo. Si existe un problema con el programa es
posible que este archivo no sea eliminado, en este caso debe eliminarse manualmente.

    $ rbx console
    VM: rbx -Xagent.start leak.rb tcp://127.0.0.1:5549 1024 100000000
    Connecting to VM on port 60544
    Connected to localhost:60544, host type: x86_64-apple-darwin10.5.0
    console> set system.memory.dump heap.dump
    console> exit

El comando es `set system.memory.dump <filename>`. El archivo del head dump
es creado en el directorio donde actualmente se está ejecutando el programa que
corre el agente.

## Analizando un Head Dump

El archivo de volcado del head dump es creado usando un formato bien documentado.
A la fecha existen dos herramientas que son capaces de leer e interpretar este
formato. Estos proyectos no forman parte de la distribución de Rubinius.

Puede encontrar head_dump en [la página del proyecto](https://github.com/evanphx/heap_dump).

Esta herramienta lee el archivo de volcado y devuelve información útil en 3
columnas que corresponden a el número de objetos visibles en el heap, la clase
del objeto y el número total de bytes que fueron consumidos por todas las
instancias de esta clase.

Al ejecutar esta herramienta, pasándole como parámetro el archivo de volcado
que obtuvimos en nuestro programa `leak.rb`, podemos ver una pequeña pista de
donde está la fuga de memoria. La siguiente salida ha sido editada para
incluir solamente datos relevantes.

    $ rbx -I /path/to/heap_dump/lib /path/to/heap_dump/bin/histo.rb heap.dump
        169350   Rubinius::CompactLookupTable 21676800
        168983             FFI::MemoryPointer 6759320
        168978                   ZMQ::Message 8110944
        168978                    LibZMQ::Msg 6759120
         27901                Rubinius::Tuple 6361528
         15615                         String 1124280
         13527            Rubinius::ByteArray 882560
          3010                          Array 168560
           825                    Hash::Entry 46200
           787       Rubinius::AccessVariable 62960
            87                           Time 4872
            41                           Hash 3280
            12                   FFI::Pointer 480
             2                    ZMQ::Socket 96

Aunque no se puede ver nada muy escandaloso en este ejemplo, algunos hechos
son notables.

1. La mayor cantidad de bytes es consumida por `Rubinius::CompactLookupTable`
que es una clase que nuestro código nunca instancia directamente (y según los
resultados de head_dump, consume alrededor de 20MB). Podemos ver entonces que
algunas estructuras de Rubinius son reportadas en el archivo de volcado. Es
interesante pero no nos ayuda a encontrar nuestra fuga.

2. La clase `ZMQ::Message` en la línea 3 es la primera clase que nuestro código
instancia directamente. Existen alrededor de 170k instancias, así que
probablemente podremos encontrar nuestra fuga de memoria aquí.

Algunas veces una muestra única no es suficiente para hallar una fuga. En ese
caso deberíamos tomar varias muestras del heap en tiempos diferentes y dejar
que head_dump haga un análisis de *diff*. El *diff* muestra lo que ha cambiado
en el heap en el *antes* y *después*.


    $ rbx -I /path/to/heap_dump/lib /path/to/heap_dump/bin/histo.rb heap.dump heap2.dump
    203110   Rubinius::CompactLookupTable 25998080
    203110                   ZMQ::Message 9749280
    203110                    LibZMQ::Msg 8124400
    203110             FFI::MemoryPointer 8124400

Este diff muestra claramente la fuente del consumo de memoria. El código tiene
200k instancias adicionales de `ZMG::Message` entre el primer y el segundo
volcado del heap así que es allí donde está sucediendo el crecimiento desmedido
de memoria.

Examinando el código podemos ver que una de estas dos líneas es posiblemente
la culpable.

    messages << message
    ...
    puts "received #{messages.size} messages in #{elapsed_usecs / 1_000_000} seconds"

Definitivamente no es necesario guardar cada mensaje para obtener el número total
de mensajes al final de la ejecución. Al revisar el código podemos ver que usando
una variable contadora podemos solucionar la fuga de memoria.

## Herramientas Avanzados - Solamente OS X

Después de modificar el código Ruby para utilizar un contador (dejando así que
el recolector de basura maneje todas las instancias de `ZMQ::Message`) vemos que
el programa todavía tiene una fuga de memoria. Tomamos dos muestras y las
analizamos, pero todavía no sabemos dónde puede estar el problema.

    $ rbx -I /path/to/heap_dump/lib /path/to/heap_dump/bin/histo.rb heap3.dump heap4.dump
      -4                          Array -224
     -90                 Digest::SHA256 -4320
     -90          Rubinius::MethodTable -4320
     -90                   Digest::SHA2 -3600
     -90          Rubinius::LookupTable -4320
     -90                          Class -10080
    -184                Rubinius::Tuple -29192

Este diff nos muestra que el número de algunas estructuras se encogió entre las
dos muestras. Al parecer el problema ya no está en el código de Ruby ya que la
VM es quien está consumiendo la memoria que se está fugando.

Afortunadamente existe una excelente herramienta en Mac OS X llamada `leaks`
 que puede ayudarnos a encontrar el problema. Adicionalmente, la página de man
para `malloc` provee información sobre una variable de entorno
llamada MallockStackLogging que podemos utilizar para incluir una traza de
pila (stack trace) al final del archivo de volcado.

    $ MallocStackLogging=1 rbx leak.rb tcp://127.0.0.1:5549 1024 10000000 &
    $ leaks 36700 > leak.out
    $ vi leak.out
    leaks Report Version:  2.0
    Process:         rbx [36700]
    Path:            /Volumes/calvin/Users/cremes/.rvm/rubies/rbx-head/bin/rbx
    Load Address:    0x100000000
    Identifier:      rbx
    Version:         ??? (???)
    Code Type:       X86-64 (Native)
    Parent Process:  bash [997]

    Date/Time:       2010-12-22 11:34:35.225 -0600
    OS Version:      Mac OS X 10.6.5 (10H574)
    Report Version:  6

    Process 36700: 274490 nodes malloced for 294357 KB
    Process 36700: 171502 leaks for 263427072 total leaked bytes.
    Leak: 0x101bb2400  size=1536  zone: DefaultMallocZone_0x100dea000
            0x01bb2428 0x00000001 0x00000400 0x00000000     ($..............
            0x00000000 0x00000000 0x00000000 0x00000000     ................
            0x00000000 0x00000000 0x61616161 0x61616161     ........aaaaaaaa
            0x61616161 0x61616161 0x61616161 0x61616161     aaaaaaaaaaaaaaaa
            0x61616161 0x61616161 0x61616161 0x61616161     aaaaaaaaaaaaaaaa
            0x61616161 0x61616161 0x61616161 0x61616161     aaaaaaaaaaaaaaaa
            0x61616161 0x61616161 0x61616161 0x61616161     aaaaaaaaaaaaaaaa
            0x61616161 0x61616161 0x61616161 0x61616161     aaaaaaaaaaaaaaaa
            ...
            Call stack: [thread 0x102f81000]: | thread_start | _pthread_start |
            thread_routine | zmq::kqueue_t::loop() | zmq::zmq_engine_t::in_event() |
            zmq::decoder_t::eight_byte_size_ready() | zmq_msg_init_size | malloc |
            malloc_zone_malloc

Podemos ver que en el momento de la muestra tenemos alrededor de 172k fugas.
La salida de la pila de llamadas nos muestra que la fuga ocurre durante la
llamada a `zmq_msg_init_size`, lo que no significa nada a menos de que decidamos
leer el código fuente donde se implementa `ZMQ::Message`. Es aquí donde el
conocimiento sobre el sistema subyacente es critico; sin más información donde
esta llamada es hecha sería mucho más difícil encontrar el problema.

Resulta que la clase `ZMQ::Message` reserva memoria por medio de `malloc` sobre
la cual no se hace seguimiento en el GC (Garbage Collector) de Rubinius. La
de-asignación de esta memoria debe ser hecha manualmente.

Una llamada a `ZMQ::Message#close` resuelve el problema.
