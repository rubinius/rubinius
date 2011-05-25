---
layout: doc_pt_br
title: Specs
previous: Ruby - Variáveis Globais
previous_url: ruby/global-variables
next: RubySpec
next_url: specs/rubyspec
review: true
---

O projeto Rubinius usa especificações executáveis no estilo TDD/BDD para
impulsionar o desenvolvimento. Diretório 'spec' é conceitualmente dividida
em duas partes:

  1. Todos os arquivos em './spec/ruby' descrevem o comportamento de MatzRuby.
  2. E todos os outros arquivos dentro do diretório './spec' descrevem o comportamento
     de Rubinius.

The specs at ./spec/ruby are a copy of RubySpec at a particular revision. These are regularly imported from the RubySpec project and failing specs are tagged so that the CI process always runs a known good set of specs. This enables easily confirming that changes to Rubinius code do not cause regressions.

Pode encontrar mais informações sobre a organização das specs 
e os guias no [Projeto RubySpec](http://rubyspec.org/).

Use o seguinte workflow ao adicionar specs e código no Rubinius:

  1. Write failing specs for some aspect of Ruby behavior. Commit the specs in
     a separate commit to the appropriate files under ./spec/ruby.
  2. Add Rubinius code to make the specs pass. Again, commit these changes in
     a separate commit from the spec changes.
  3. Run the `rake` command to ensure that all the CI specs pass.

The changes to files in ./spec/ruby are regularly pushed to the RubySpec
project. Also, current changes to RubySpec from committers to other Ruby
implementations are regularly updated in the ./spec/ruby directories. When the
specs are updated from RubySpec, the CI tags are also updated.
