---
layout: doc_de
title: Anleitung - Benchmarks schreiben
previous: Einen fehlgeschlagenen Spec reparieren
previous_url: how-to/fix-a-failing-spec
next: Einen Blogeintrag schreiben
next_url: how-to/write-a-blog-post
---

Warum Benchmarks?

Benchmarks sind ein hervorragendes Hilfsmittel, um Rubinius mit den anderen
Ruby Implementationen, wie z.B. MRI, JRuby, IronRuby und anderen, zu
vergleichen. Dabei geht es weniger darum, Rubinius selber zu messen, sondern
Vergleichswerte zu erzielen. Möchtest du Benchmarks schreiben, dann achte bitte
auf folgende Richtlinien:

  1.  Schau dir bestehende Benchmarks unter rubinius/benchmarks an und versuche
      dem Schema zu folgen.
  2.  Jede Benchmark-Datei sollte sich auf einen besonderen Aspekt von Ruby
      konzentrieren, beispielsweise dem Löschen von Schlüsseln und Werten in 
      Hashes.
  3.  Verwende das interne Benchmark-Framework.
  4.  Benchmarks sollten kurz und prägnant sein.
  5.  Die Benchmarks messen nicht Rubinius. Schreibst du also ein Benchmark für
      eine Klasse, die über sog. Bang-Methoden (also das Objekt verändernde 
      Methoden, meist mit einem Ausrufezeichen versehen) und 
      Nicht-Bang-Methoden verfügt, dann solltest du für die Bang-Methode das
      Objekt duplizieren, nicht jedoch für die Nicht-Bang-Methode.

Zum Testen der Benchmarks kannst einzelne Dateien oder auch ganze Ordner 
übergeben:

    bin/benchmark benchmark/core/string/bench_case.rb
    bin/benchmark benchmark/core
