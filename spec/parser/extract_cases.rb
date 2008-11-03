#!/usr/bin/env ruby
#
# Extract the parse tree cases from pt_testcase.rb

dir = ARGV.shift || "spec/parser"

$: << 'lib'
require 'pt_testcase'
require 'pp'

ParseTreeTestCase.testcases.sort.each do |node, hash|
  next if Array === hash['Ruby']

  hash['RubyParser'] = hash.delete 'ParseTree'

  File.open("#{dir}/#{node}_spec.rb", "w") do |f|
    f.puts "def test_case"
    f.puts hash.pretty_inspect
    f.puts "end"
  end
end
