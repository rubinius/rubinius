---
layout: doc_pl
title: Kompilator bajtkodu
previous: Maszyna wirtualna - Custom Dispatch Logic
previous_url: virtual-machine/custom-dispatch-logic
next: Etap parsowania
next_url: bytecode-compiler/parser
review: true
---

Kompilator bajtkodu zamienia kod źródłowy Ruby na bajtkod który jest
wykonywany przez maszynę wirtualną. Na kompilację składa się seria etapów, które
transformują kod źródłowy na format zrozumiały dla maszyny wirtualnej.

Każdy z tych etapów jest oddzielny do reszty procesu i odpowiada
jedynie za przyjęcie danych w konkretnej formie na wejściu i wysłaniu
efektu przekształcenia formy na wyjście. W wyniku tego rozdzielenia
poszczególnych etapów proces ten jest wysoce konfigurowalny.

Każdy z tych etapów jak również ich wejścia i wyjścia jest opisany
poniżej.

<div style="text-align: center; width: 100%">
  <img src="/images/compilation_process.png" alt="Compilation process" />
</div>

1. [Etap parsowania](/doc/pl/bytecode-compiler/parser/)
1. [AST](/doc/pl/bytecode-compiler/ast/)
1. [Etap generowania](/doc/pl/bytecode-compiler/generator/)
1. [Etap kodowania](/doc/pl/bytecode-compiler/encoder/)
1. [Etap pakowania](/doc/pl/bytecode-compiler/packager/)
1. [Etap zapisu](/doc/pl/bytecode-compiler/writer/)
1. Printers
1. [Transformacje](/doc/pl/bytecode-compiler/transformations/)
1. [Modyfikacja procesu kompilacji](/doc/pl/bytecode-compiler/customization/)

