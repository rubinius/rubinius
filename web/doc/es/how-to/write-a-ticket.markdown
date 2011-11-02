---
layout: doc_es
title: How-To - Write a Ticket
previous: How-To
previous_url: how-to
next: Escribir una especificación en Ruby
next_url: how-to/write-a-ruby-spec
---

The Rubinius issue tracker is <http://github.com/rubinius/rubinius/issues>.

Para ser útiles, los boletos deben ser concisas, específicas y acciones
concretas. Si no, el boleto languidecen y se convierten en el desorden. En
consecuencia, los boletos deben caer en uno (o más) de las siguientes
categorías:

   1. Una historia de línea de comandos precisos para demostrar cómo instalar
      e invocar el programa y que muestra el seguimiento hacia atrás de una
      excepción.
   2. Un pequeño fragmento de código que ilustra el problema y la línea de
      comandos lo invocan.
   3. Un parche, **incluyendo especificaciones técnicas, si no existen**, y
      mostrando la especificación de funcionamiento antes y después de aplicar
      el parche.

Si el problema no se ajusta a una de las categorías, no es válido. Es
simplemente no es apropiado para un boleto.

   1. Si se trata de una función, consideran que trata sobre la lista de
      correo. Además, podría tomar una grieta en su aplicación y demostrar
      cómo su función es útil.
   2. Si se trata de una biblioteca o joya que no está funcionando, tómese un
      tiempo para cavar en y ver si se puede crear una reproducción de un tema
      y después de que, como Venta de entradas.


## Procedimiento general para la presentación de un billete

1. Vuelva a comprobar.

      1. Hacer una reconstrucción completa ('rake clean; rake') después de un
         'git pull' o clonar fresco.
      2. Leer [Troubleshooting](/doc/es/getting-started/troubleshooting/)
         ver si algo se resuelve el problema.
      3. Leer [Specs](/doc/es/specs/).

   2. Dé su billete un título específico, preferentemente corto.

   3. Dé a sus etiquetas apropiadas billete.

   4. Dar suficientes detalles sobre el tema.

      * La línea de comandos para invocar el programa
      * El seguimiento hacia atrás o resultado del programa en comparación con
        resultado esperado.
      * La información de su máquina. `uname-a` es generalmente bueno (si hay
        alguna "desconocido" en campos que, por favor, más detalles sobre
        ellos.)


## Instrucciones adicionales para las entradas con parches

   * Puede ser sólo un conjunto de especificaciones técnicas.
   * Los parches deben ir acompañadas de especificaciones técnicas a menos que
     las especificaciones técnicas ya existentes.
   * La parte pertinente de la producción de especificaciones y la exacta
     `bin/mspec` la invocación de la existentes o agregar especificaciones
     *antes de la corrección*.
   * La salida de especificaciones y `bin/mspec` exactamente la invocación
     que muestra el éxito *después* de la revisión.
   * Descripción adicional de su parche y cómo se soluciona el problema. En
     particular, con nuevas funcionalidades por favor, indique si ya estaba
     discutido en #rubinius o rubinius-dev.

A menos que por alguna razón imposible, por favor use `git-format-patch` para
crear la patchset. Es mucho más fácil de aplicar y que conserva la atribución
correcta.  De lo contrario, un diff unificado.


## Ejemplo de la presentación de un parche

Supongamos que la especificación siguiente existe y no es:

    describe "Kernel.format" do
      it "is accessible as a module function" do
        Kernel.format("%s", "hello").should == "hello"
      end
    end

1. Ticket Title:

   "[PATCH] No method 'format' on Kernel (Module)"

2. Tags:

   "patch core spec"

3. Ticket Message:

   The method 'format' is not available as a module function of Kernel.

       $ bin/mspec spec/ruby/core/kernel/format_spec.rb
       Started
       .E

       1)
       Kernel.format is accessible as a module function ERROR
       No method 'format' on Kernel (Module):

   The method 'format' already exists but has not been set as a module
   function.  This patch does so.

   After the patch is applied:

       $ bin/mspec spec/ruby/core/kernel/format_spec.rb
       Started
       ..

       Finished in 0.016031 seconds

       2 examples, 2 expectations, 0 failures, 0 errors

4. Attachment:

Por último, poner el parche en una esencia y añadir el enlace a la esencia de
su problema.  A continuación se reproduce el parche en línea para la integridad:


    From c61cecce6442347ebbdf1ded7a5c0832c97582c1 Mon Sep 17 00:00:00 2001
    From: Brian Ford <bford@engineyard.com>
    Date: Sat, 19 Jan 2008 17:48:19 -0800
    Subject: [PATCH] Set Kernel#format as a module function.


    diff --git a/kernel/core/kernel.rb b/kernel/core/kernel.rb
    index 2d2e508..f2a382e 100644
    --- a/kernel/core/kernel.rb
    +++ b/kernel/core/kernel.rb
    @@ -150,6 +150,7 @@ module Kernel
       end
       alias_method :format, :sprintf
       module_function :sprintf
    +  module_function :format
       module_function :abort

       def puts(*a)

