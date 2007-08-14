# -*- ruby -*-

require 'rubygems'
require 'hoe'
require './rubicon_testcase.rb'

Hoe.new('bfts', RubiconTestCase::VERSION) do |p|
  p.rubyforge_name = 'bfts'
  p.summary = 'Big "Formal" Test Suite'
  p.description = p.paragraphs_of('README.txt', 2).join("\n\n")
  p.url = p.paragraphs_of('README.txt', 0).first.split(/\n/)[1..-1]
  p.changes = p.paragraphs_of('History.txt', 0..1).join("\n\n")
  p.extra_deps << "miniunit"
  p.test_globs = "test*.rb"
end

# vim: syntax=Ruby
