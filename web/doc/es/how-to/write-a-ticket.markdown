---
layout: doc_es
title: Como - Escribir un ticket
previous: How-To
previous_url: how-to
next: Como escribir una especificación en Ruby
next_url: how-to/write-a-ruby-spec
---

El issue tracker de Rubinius es <http://github.com/rubinius/rubinius/issues>.

Para ser útiles, los tickets deben ser concisos, específicos y acciones
concretas. Si no, el ticket va a languidecer y convertirce en desorden. En
consecuencia, los tickets deben caer en una (o más) de las siguientes
categorías:

   1. Una historia precisa de la línea de comandos para demostrar cómo instalar
      e invocar el programa y que muestra el backtrace de una excepción.
   2. Un pequeño fragmento de código que ilustra el problema y la línea de
      comandos que lo invoca.
   3. Un parche, **incluyendo specs, si no existen**, y
      mostrando la spec funcionando antes y después de aplicar el parche.

Si el problema no se ajusta a una de las categorías, no es
válido. Simplemente no es apropiado para un ticket.

   1. Si se trata de una característica, considere discutirla en la lista de
      correo. Además, podría intentar implementarla y demostrar
      cómo su característica es útil.
   2. Si se trata de una biblioteca o gema que no está funcionando, tómese un
      tiempo para investigar y ver si se puede reproducir
      y envíe eso como ticket.


## Procedimiento general para enviar un ticket

1. Verifique dos veces.

      1. Hacer una reconstrucción completa ('rake clean; rake') después de un
         'git pull' o un clone fresco.
      2. Leer [Troubleshooting](/doc/es/getting-started/troubleshooting/)
         ver si algo resuelve el problema.
      3. Leer [Specs](/doc/es/specs/).

   2. Dé a su ticket un título específico, preferentemente corto.

   3. Etiquete correctamente su ticket.

   4. Dar suficientes detalles sobre el tema.

      * La línea de comandos para invocar el programa
      * El backtrace o resultado del programa en comparación con
        resultado esperado.
      * La información de su máquina. `uname -a` está generalmente bien (si hay
        algun "unkown" en cualquiera de los campos, por favor, dar más
        detalles sobre ellos.)


## Instrucciones adicionales para tickets con parches

   * Puede ser sólo un conjunto de specs.
   * Los parches deben ir acompañadas de specs a menos que
     las specs ya existan.
   * La parte relevante de la salida de las specs y la exacta invocación de
     `bin/mspec` de la spec agregada o existente *antes de la corrección*.
   * La salida de las specs y la invocación exacta de `bin/mspec` que muestra el éxito *después* del arreglo.
   * Una descripción adicional de su parche y cómo este soluciona el problema. En
     particular, con nuevas funcionalidades, por favor, indique si ya se ha discutido en #rubinius o rubinius-dev.

A menos que por alguna razón sea imposible, por favor use `git-format-patch` para crear el patchset. Es mucho más fácil de aplicar y conserva la atribución correcta.  De lo contrario, un diff unificado.


## Ejemplo de la presentación de un parche

Supongamos que la siguiente spec existe y está fallando:

    describe "Kernel.format" do
      it "is accessible as a module function" do
        Kernel.format("%s", "hello").should == "hello"
      end
    end

1. Título del ticket:

   "[PATCH] No method 'format' on Kernel (Module)"

2. Tags:

   "patch core spec"

3. Mensaje del ticket:

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

Por último, ponga el parche en un gist y agregue el enlace al gist
de su issue.  A continuación se reproduce el parche en una línea por una cuestión de integridad:


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

