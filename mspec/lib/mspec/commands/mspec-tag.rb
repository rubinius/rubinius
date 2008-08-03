#!/usr/bin/env ruby

require 'mspec/version'
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
    options = MSpecOptions.new "mspec tag [options] (FILE|DIRECTORY|GLOB)+", 30, config

    options.doc " Ask yourself:"
    options.doc "  1. What specs to run?"
    options.doc "  2. How to modify the execution?"
    options.doc "  3. How to display the output?"
    options.doc "  4. What tag action to perform?"
    options.doc "  5. When to perform it?"

    options.doc "\n What specs to run"
    options.filters

    options.doc "\n How to modify the execution"
    options.configure { |f| load f }
    options.name
    options.pretend
    options.interrupt

    options.doc "\n How to display their output"
    options.formatters
    options.verbose

    options.doc "\n What action to perform and when to perform it"
    options.tagging

    options.doc "\n Help!"
    options.version MSpec::VERSION
    options.help

    options.doc "\n How might this work in the real world?"
    options.doc "\n   1. To add the 'fails' tag to failing specs"
    options.doc "\n     $ mspec tag path/to/the_file_spec.rb"
    options.doc "\n   2. To remove the 'fails' tag from passing specs"
    options.doc "\n     $ mspec tag --del fails path/to/the_file_spec.rb"
    options.doc ""

    @patterns = options.parse argv
    if @patterns.empty?
      puts options
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

