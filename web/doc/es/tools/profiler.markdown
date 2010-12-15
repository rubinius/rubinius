---
layout: doc_es
title: Profiler
previous: Debugger
previous_url: tools/debugger
next: Memory Analysis
next_url: tools/memory-analysis
translated: true
---

Rubinius tiene una instrumentación de perfiles que proporciona la
sincronización exacta para todos métodos que se ejecutan. El generador de
perfiles se implementa en el nivel de máquina virtual y los datos Se
proporciona el código Ruby a proceso.


## VM Profiler

El elenco de personajes involucrados en la creación y el mantenimiento del
generador de perfiles incluyen VM, SharedState, ProfilerCollection, y de
perfiles. La clase virtual es un hilo de estructura de datos locales. Cada
instancia de VM obtiene un analizador por separado instancia. La instancia
SharedState tiene una instancia ProfilerCollection que los mapas Máquinas
virtuales con perfiladores y genera los resultados globales de todos los
perfiladores.

El Profiler vive y muere en su propio mundo. El generador de perfiles se pasa
a una máquina virtual ejemplo, cuando se creó porque el generador de perfiles
necesita tener acceso a ella mientras está reuniendo información. El argumento
de Estado podría ser aprobada en todos los perfiles métodos, pero es lo
suficientemente simple para que pase cuando el generador de perfiles se crea.
El generador de perfiles no manipula la instancia de VM. Es importante
mantener esta separación.

La instancia de VM perezosamente crea su instancia Profiler cuando sea
necesario. El VM registra los perfiles con los SharedState.

El SharedState mantiene la instancia ProfilerCollection y pide delanteros para
registrar o quitar perfiladores.

La instancia solicita ProfilerCollection que la instancia de máquina virtual
asignado a un perfiles elimina el generador de perfiles cuando el generador de
perfiles serán eliminados.


## Ruby Profiler

En la tierra de Ruby, el Rubinius::Profiler::Instrumenter ejemplo expone nada
sobre la realidad multi-hilo en la máquina virtual. El individuo C++ Perfil
casos no están expuestos a Ruby. En Ruby, una instancia de perfiles se crea.
Esa instancia sólo puede iniciar y parar la máquina virtual de perfiles.
Cuando el generador de perfiles se detenido, información sobre la descripción
del perfil se devuelve en un LookupTable. El Ruby código de perfiles que puede
mostrar información utilizando el método #show.

Rubinius proporciona una profile.rb compatible con la biblioteca estándar y
profiler.rb.  Consulte la documentación de la RM sobre el uso de ellos.

En cuanto a lib/profiler.rb, puede ver los pasos básicos para utilizar el
generador de perfiles de código Ruby.

    # create a profiler instance
    profiler = Rubinius::Profiler::Instrumenter.new

    # start the profiler
    profiler.start

    # stop the profiler
    profiler.stop

    # get the profile data
    data = profiler.info

    # or print out the profiler info
    profiler.show  # takes on IO object, defaults to STDOUT

También puede utilizar un método de conveniencia para el perfil de trabajo en
un bloque.

    # create a profiler instance
    profiler = Rubinius::Profiler::Instrumenter.new

    # profile some work
    profiler.profile do
      # some work
    end

El método #profile se inicia el generador de perfiles, los rendimientos, se
detiene el generador de perfiles y grabados los datos del perfil por defecto.
Pase 'false' a #profile para no imprimir los datos.  De cualquier manera, los
datos del perfil se devuelve #profile.


## Cómo Leer la Salida Plana de Perfiles

La salida de perfiles piso tiene las siguientes columnas:


### % time

La cantidad de tiempo dedicado a este método como un porcentaje del tiempo
total dedicado a todos los métodos.


### cumulative seconds

La cantidad total de tiempo dedicado a este método y todos sus callees directa
y su callees hasta el final a todos los métodos de la hoja llamada a lo largo
de una ruta de acceso de este método. Considere la posibilidad de este método
como la raíz de un árbol de llamadas. La suma de todos los tiempo en los
métodos en este árbol de llamadas es el segundo acumulada para este método.


### self seconds

El tiempo total empleado en este método menos el tiempo total invertido en
todo esto método callees.


### calls

El número total de veces que este método fue llamado.


### self ms/call

El segundo auto como milisegundos dividido por el número total de llamadas.


### total ms/call

El segundo acumulado milisegundos dividido por el número total de llamadas.


### Ejemplo de Salida Plana

La siguiente secuencia de comandos es la base del perfil de los dos ejemplos a
continuación.

    class F
      def foo(a)
        1 + a
      end

      def bar(a, b)
        foo(a) ** b
      end

      def work(a, b, n, m)
        n.times { |i| i + bar(a, b) }
        m.times { |i| foo(i) }
      end
    end

    p = Rubinius::Profiler::Instrumenter.new :sort => :self_seconds
    p.profile {
      F.new.work 2, 3, 10, 5
    }


Al ejecutar el script con 'bin/rbx script.rb' debe dar el siguiente plano
de salida.


      %   cumulative   self                self     total
     time   seconds   seconds      calls  ms/call  ms/call  name
    ------------------------------------------------------------
      9.64     0.00      0.00         10     0.00     0.00  Fixnum#**
     23.33     0.00      0.00          1     0.01     0.08  #toplevel
     12.64     0.00      0.00         10     0.00     0.00  F#bar
     15.38     0.00      0.00         15     0.00     0.00  F#work {}
     17.74     0.00      0.00          2     0.00     0.03  Integer#times
     19.29     0.00      0.00          1     0.01     0.06  F#work
      1.12     0.00      0.00          1     0.00     0.00  Class#new
      0.68     0.00      0.00         15     0.00     0.00  F#foo
      0.13     0.00      0.00          1     0.00     0.00  Class#allocate
      0.06     0.00      0.00          1     0.00     0.00  Object#initialize

    10 methods called a total of 57 times


## Cómo Leer la Salida Gráfico

La salida gráfica es activado con la opción de configuración:

    -Xprofiler.graph

Determinado por encima del mismo guión, la producción gráfica se muestra a
continuación. Cada "entrada" en el gráfico tiene tres secciones: 1) el método
para la entrada, llama la _primary_ línea, 2) las personas que llaman del
método principal, y 3) los métodos que el principal método llamado. Los campos
tienen significados diferentes en función del parte de la entrada.

Para la línea principal, los campos son los siguientes:


### index

Un índice asignado a cada método en el gráfico para facilitar las referencias
cruzadas las entradas.


### % time

La cantidad de tiempo dedicado a este método como un porcentaje del tiempo
total dedicado a todos los métodos. Esta es la misma que la salida plana.


### self

El tiempo total empleado en este método menos el tiempo total invertido en
todo esto método Callees. Este es el mismo segundo auto en la salida plana.


### children

El tiempo total empleado en todos los métodos llamados por este método.


### called

El número total de veces que este método fue llamado.


### name

El nombre del método seguido por el número de índice.


Las líneas por encima de la línea principal son los métodos que llaman el
método principal. La campos que llaman 'tienen la siguiente interpretación:


### self

El tiempo total empleado en este método menos el tiempo total invertido en
todo esto método Callees. Este es el mismo segundo auto en la salida plana.


### children

El tiempo dedicado a llamar al método para el método principal.


### called

El campo tiene dos partes separadas por una barra diagonal. La izquierda es la
número de veces en que este método denominado el método principal. El derecho
es el total número de llamadas a este método hizo. En otras palabras, los dos
números juntos muestran una relación de las llamadas al método primario versus
todas las llamadas realizadas por el persona que llama.


### name

El nombre de la persona que llama seguido de su número de índice. Si el índice
es [0], el método no aparece en el gráfico.


Las líneas por debajo de la línea principal son los métodos que el principal
método llamado.  Los campos de los métodos llamados son los siguientes:


### self

El tiempo total empleado en este método menos el tiempo total invertido en
todo esto método callees. Este es el mismo segundo auto en la salida plana.


### children

Esta es una estimación de la cantidad de tiempo Callees este método ha pasado
al este método fue llamado por el método principal. La estimación se basa en
la ración del tiempo dedicado a este método cuando es llamado por el método
principal para la tiempo total empleado en este método.


### called

El campo "tiene dos partes separadas por una barra diagonal. La izquierda es
la número de veces en que este método fue llamado por el método principal. El
derecho es el número total de veces en que este método fue llamado.


### name

El nombre del método llamado seguida de su número de índice [N]. Si no hay
presente índice, no hay ninguna entrada principal para el método en el
gráfico. Utilice el opción --Xprofiler.full_report para imprimir el gráfico
entero si es necesario para ver de la entrada.


    index  % time     self  children         called       name
    ----------------------------------------------------------
                      0.00      0.00         10/20             F#bar [3]
    [1]       9.6     0.00      0.00         10           Fixnum#** [1]
    -------------------------------------------------------
    [2]      23.2     0.00      0.00          1           #toplevel [2]
                      0.00      0.00          1/1              Class#new [7]
                      0.00      0.00          1/1              F#work [6]
    -------------------------------------------------------
                      0.00      0.00         10/15             F#work {} [4]
    [3]      12.7     0.00      0.00         10           F#bar [3]
                      0.00      0.00         10/15             F#foo [8]
                      0.00      0.00         10/10             Fixnum#** [1]
    -------------------------------------------------------
                      0.00      0.00         15/15             Integer#times [5]
    [4]      15.4     0.00      0.00         15           F#work {} [4]
                      0.00      0.00         10/10             F#bar [3]
                      0.00      0.00          5/15             F#foo [8]
    -------------------------------------------------------
                      0.00      0.00          2/2              F#work [6]
    [5]      17.8     0.00      0.00          2           Integer#times [5]
                      0.00      0.00         15/15             F#work {} [4]
    -------------------------------------------------------
                      0.00      0.00          1/2              #toplevel [2]
    [6]      19.3     0.00      0.00          1           F#work [6]
                      0.00      0.00          2/2              Integer#times [5]
    -------------------------------------------------------
                      0.00      0.00          1/2              #toplevel [2]
    [7]       1.1     0.00      0.00          1           Class#new [7]
                      0.00      0.00          1/1              Object#initialize [10]
                      0.00      0.00          1/1              Class#allocate [9]
    -------------------------------------------------------
                      0.00      0.00         10/20             F#bar [3]
                      0.00      0.00          5/15             F#work {} [4]
    [8]       0.7     0.00      0.00         15           F#foo [8]
    -------------------------------------------------------
                      0.00      0.00          1/2              Class#new [7]
    [9]       0.1     0.00      0.00          1           Class#allocate [9]
    -------------------------------------------------------
                      0.00      0.00          1/2              Class#new [7]
    [10]      0.1     0.00      0.00          1           Object#initialize [10]
    -------------------------------------------------------

    10 methods called a total of 57 times

