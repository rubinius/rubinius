#!/usr/bin/env ruby

$:.unshift File.expand_path(File.dirname(__FILE__) + '/../lib')

require 'optparse'
require 'mspec/utils/options'
require 'mspec/utils/script'


class MSpecCI < MSpecScript
  def options(argv=ARGV)
    options = MSpecOptions.new config, "ci", "", 28, "   "

    options.separator " Ask yourself:"
    options.separator "  1. How to run the specs?"
    options.separator "  2. How to display the output?"
    options.separator "  3. What action to perform?"
    options.separator "  4. When to perform it?"

    options.separator "\n How to run the specs"
    options.add_config { |f| load f }
    options.add_name
    options.add_pretend
    options.add_interrupt

    options.separator "\n How to display their output"
    options.add_formatters
    options.add_verbose

    options.separator "\n What action to perform"
    options.add_actions

    options.separator "\n When to perform it"
    options.add_action_filters

    options.separator "\n Help!"
    options.add_version
    options.add_help

    options.separator "\n How might this work in the real world?"
    options.separator "\n   1. To simply run the known good specs"
    options.separator "\n     $ mspec ci"
    options.separator "\n   2. To run a subset of the known good specs"
    options.separator "\n     $ mspec ci path/to/specs"
    options.separator "\n   3. To start the debugger before the spec matching 'this crashes'"
    options.separator "\n     $ mspec ci --spec-debug -S 'this crashes'"
    options.separator ""

    @patterns = options.parse argv
    @patterns = config[:ci_files] if @patterns.empty?
  end

  def run
    files = []
    @patterns.each do |item|
      stat = File.stat(File.expand_path(item))
      files << item if stat.file?
      files.concat(Dir[item+"/**/*_spec.rb"].sort) if stat.directory?
    end

    MSpec.register_tags_patterns config[:tags_patterns]
    MSpec.register_files files
    TagFilter.new(:exclude, "fails").register
    TagFilter.new(:exclude, "unstable").register
    TagFilter.new(:exclude, "incomplete").register

    MSpec.process
    exit MSpec.exit_code
  end
end
