---
layout: doc_es
title: Analizador
previous: Depurador
previous_url: tools/debugger
next: Análisis de Memoria
next_url: tools/memory-analysis
translated: false
---

Rubinius tiene un analizador de perfiles que proporciona los tiempos
de ejecución exactos para todos los métodos invocados. El analizador
está implementado a nivel de la VM (máquina virtual) y los datos son
entregados al código Ruby para hacer uso de ellos.

## Analizador de la VM

El elenco de personajes involucrados en la creación y el mantenimiento del
analizar incluye a VM, SharedState, ProfilerCollection, y Profiler.
La clase VM es  una estructura de datos local a los hilos. Cada instancia
de la VM tiene una instancia diferente de Profiler. La instancia de SharedState
 tiene una instancia de ProfilerCollection que mapea VMs a Profilers y genera
los resultados compuestos de todas las instancias de Profiler.

El Profiler vive y muere en su propio mundo. El analizador recibe una instancia
de VM cuando es creado ya que este necesita acceso a ella mientras está obteniendo
la información. El argumento STATE podría ser pasado a todos los métodos del analizador pero
es muy simple, así que se pasa el analizador cuando este es inicializado.

La instancia de VM crea su instancia de Profiler de manera perezosa (solo cuando
se necesita). La VM registra el Profiler con SharedState.

SharedState tiene una instancia de ProfilerCollection y envía las llamadas para
registrar o remover analizadores (profilers).

La instancia ProfilerCollection se encarga de hacer la petición para que la VM
mapeada a un analizador remueva el analizador cuando este vaya a ser eliminado.

## Analizador de Ruby

En el campo de Ruby, la instancia de Rubinius::Profiler::Instrumenter no expone
nada sobre la naturaleza multi-hilo de la VM. Las instancias individuales del
analizador en C++ no son expuestas a Ruby. En Ruby, una instancia del analizador
es creada. Esta instancia puede iniciar o detener el analizador de la VM. Cuando
el analizador es detenido, la información sobre el análisis es retornada en una
LookupTable. El analizador de Ruby puede mostrar esa información con una llamada
al método #show.

Rubinius provee dos bibliotecas compatibles con la biblioteca estándar de MRI
llamadas profile.rb y profiler.rb. Puede encontrar más información sobre el uso
de estas en la documentación de MRI.

Se puede observar los pasos básicos para usar el analizador desde el código Ruby en
lib/profiler.rb.


    # crea una instancia del analizador
    profiler = Rubinius::Profiler::Instrumenter.new

    # inicia el analizador
    profiler.start

    # detiene el analizador
    profiler.stop

    # obtiene la información del analizador
    data = profiler.info

    # o se imprime la información del analizador
    profiler.show  # recibe un objeto IO, por defecto usa STDOUT


Puede utilizar un método más conveniente para analizar el trabajo realizado
en un bloque.

    # crea una instancia del analizador
    profiler = Rubinius::Profiler::Instrumenter.new

    # analiza algo
    profiler.profile do
      # algún trabajo
    end

El método #profile inicia el analizador, hace la llamada a yield, detiene el
analizador e imprime por defecto los datos del análisis. Puede pasar la opción
'false' al método #profile para que no se impriman los datos. De cualquier
manera, los datos son el valor retornado por la llamada a #profile.

## Cómo Leer la Salida del Analizador

La salida del analizador tiene las siguientes columnas:

### % time

La cantidad de tiempo dedicado a este método. Se presenta como una relación
al tiempo de ejecución de todos los métodos.


### cumulative seconds

Si se considera este método como la raíz de un árbol, el resultado de
'cumulative seconds' sería el tiempo acumulado de ejecución de este
método y de todos los métodos que el mismo ha llamado (hojas del árbol).


### self seconds

El tiempo total empleado en este método menos el tiempo total invertido en
todos los métodos que fueron llamados por este.

### calls

El número de veces que fue llamado este método.


### self ms/call

El tiempo definido en 'self seconds' dividido por el número total de llamadas.


### total ms/call

El tiempo total de este método y de todos los métodos que este llamó dividido
por el número total de llamadas.


### Ejemplo de la Salida

El siguiente script es la base para los dos análisis que se muestran a
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


Al ejecutar el script con 'bin/rbx script.rb' se obtiene una salida como:


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


## Cómo Leer la Salida Gráfica

La salida gráfica se puede activar pasándole la siguiente opción al interprete:

    -Xprofiler.graph

Usando el script anteriormente mencionado se obtiene el resultado que se muestra
al final del documento. Cada entrada en este gráfico tiene tres secciones: 1)
una línea que describe el método para esta entrada, llamada línea primaria;
2) quienes llamaron al método primario; y 3) los métodos que el método
primario llamó. Los campos tienen diferentes significados según en la posición
en la que se encuentran para cada entrada.

Para la línea de primaria, los campos significan:

### index

Un índice asignado a cada método en la gráfica para facilitar las referencias
cruzadas entre las entradas.


### % time

La cantidad de tiempo dedicado a este método como un porcentaje del tiempo
total dedicado a todos los métodos. Igual a la salida plana.


### self

El tiempo total empleado en este método menos el tiempo total invertido en
todo esto método llamados por el mismo. Igual a la salida plana.


### children

El tiempo total empleado en todos los métodos llamados por este método.


### called

El número total de veces que fue llamado este método.


### name

El nombre del método seguido por el número de índice.


Las líneas por encima de la línea principal son los métodos que llaman al
método principal.

Los campos que llaman al método principal son interpretados así:


### self

El tiempo total empleado en este método menos el tiempo total invertido en
todo esto método que el mismo llama. Igual a 'self seconds' en la salida plana.


### children

El tiempo dedicado en las llamadas al método principal.


### called

El campo 'called' tiene dos partes separadas por una barra diagonal. El número
a la izquierda es el número de veces que este método llamó al método primario.
El número a la derecha es el número total de llamadas que este método realizó.
En otras palabras, estos números representan la relación entre el número de
llamadas al método primario vs. el número total de llamadas.

### name

El nombre de quien llamó al método seguido de su número de índice. Si el índice
es [0], el método no aparece en el gráfico.

Las líneas por debajo de la línea principal son los métodos que el método
principal llamó. Los campos de los métodos llamados son los siguientes:


### self

El tiempo total empleado en este método menos el tiempo total invertido en
todos los métodos que este método llamó. Este valor es equivalente a 'self
seconds' en la salida plana.


### children
Este es un estimado del tempo que los métodos llamados por este método se
demoraron cuando este método fue llamado por el método primario. El estimado
se basa en la relación de tiempo de ejecución de este método cuando fue llamado
por el método primario vs. el tiempo total utilizado en el método.

### called

El campo "called" tiene dos partes separadas por una barra diagonal. La izquierda
es el número de veces que este método fue llamado por el método principal. La
derecha es el número total de veces en que este método fue llamado.


### name

El nombre del método llamado seguido de su número de índice [N]. Si no hay
un índice presente, no hay ninguna entrada principal para el método en el
gráfico. Utilice el opción --Xprofiler.full_report para imprimir el gráfico
completo si es necesario ver la entrada.


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

