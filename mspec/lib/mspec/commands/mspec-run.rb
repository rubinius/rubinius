#!/usr/bin/env ruby

$:.unshift File.expand_path(File.dirname(__FILE__) + '/../lib')

require 'optparse'
require 'mspec/utils/options'
require 'mspec/utils/script'


class MSpecRun < MSpecScript
  def options(argv=ARGV)
    options = MSpecOptions.new config, "run", "", 28, "   "

    options.separator " Ask yourself:"
    options.separator "  1. What specs to run?"
    options.separator "  2. How to modify the execution?"
    options.separator "  3. How to display the output?"
    options.separator "  4. What action to perform?"
    options.separator "  5. When to perform it?"

    options.separator "\n What specs to run"
    options.add_filters

    options.separator "\n How to modify the execution"
    options.add_config { |f| load f }
    options.add_name
    options.add_randomize
    options.add_pretend
    options.add_interrupt

    options.separator "\n How to display their output"
    options.add_formatters
    options.add_verbose

    options.separator "\n What action to perform"
    options.add_actions
    options.add_verify

    options.separator "\n When to perform it"
    options.add_action_filters

    options.separator "\n Help!"
    options.add_version
    options.add_help

    options.separator "\n How might this work in the real world?"
    options.separator "\n   1. To simply run some specs"
    options.separator "\n     $ mspec path/to/the/specs"
    options.separator "     mspec path/to/the_file_spec.rb"
    options.separator "\n   2. To run specs tagged with 'fails'"
    options.separator "\n     $ mspec -g fails path/to/the_file_spec.rb"
    options.separator "\n   3. To start the debugger before the spec matching 'this crashes'"
    options.separator "\n     $ mspec --spec-debug -S 'this crashes' path/to/the_file_spec.rb"
    options.separator ""

    @patterns = options.parse argv
    if @patterns.empty?
      puts options.parser
      puts "No files specified."
      exit 1
    end
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

    MSpec.process
    exit MSpec.exit_code
  end
end

