#!/usr/bin/env ruby

require 'optparse'
require 'mspec/utils/options'
require 'mspec/utils/script'


class MSpecTag < MSpecScript
  def initialize
    super

    config[:tagger]  = :add
    config[:tag]     = 'fails:'
    config[:outcome] = :fail
  end

  def options(argv=ARGV)
    options = MSpecOptions.new config, "tag", "", 28, "   "

    options.separator " Ask yourself:"
    options.separator "  1. What specs to run?"
    options.separator "  2. How to modify the execution?"
    options.separator "  3. How to display the output?"
    options.separator "  4. What tag action to perform?"
    options.separator "  5. When to perform it?"

    options.separator "\n What specs to run"
    options.add_filters

    options.separator "\n How to modify the execution"
    options.add_config { |f| load f }
    options.add_name
    options.add_pretend
    options.add_interrupt

    options.separator "\n How to display their output"
    options.add_formatters
    options.add_verbose

    options.separator "\n What action to perform and when to perform it"
    options.add_tagging

    options.separator "\n Help!"
    options.add_version
    options.add_help

    options.separator "\n How might this work in the real world?"
    options.separator "\n   1. To add the 'fails' tag to failing specs"
    options.separator "\n     $ mspec tag path/to/the_file_spec.rb"
    options.separator "\n   2. To remove the 'fails' tag from passing specs"
    options.separator "\n     $ mspec tag --del fails path/to/the_file_spec.rb"
    options.separator ""

    @patterns = options.parse argv
    if @patterns.empty?
      puts options.parser
      puts "No files specified."
      exit 1
    end
  end

  def register
    super

    tag = SpecTag.new config[:tag]
    tagger = TagAction.new(config[:tagger], config[:outcome], tag.tag, tag.comment,
                           config[:atags], config[:astrings])
    tagger.register
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

