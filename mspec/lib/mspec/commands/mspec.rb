#!/usr/bin/env ruby

MSPEC_HOME = File.expand_path(File.dirname(__FILE__) + '/../../..')

require 'optparse'
require 'mspec/utils/options'
require 'mspec/utils/script'
require 'mspec/helpers/tmp'
require 'mspec/runner/actions/timer'


class MSpecMain < MSpecScript
  def initialize
    config[:includes] = []
    config[:requires] = []
    config[:target]   = ENV['RUBY'] || 'ruby'
    config[:flags]    = []
    config[:command]  = nil
    config[:options]  = []
  end

  def options(argv=ARGV)
    if ["ci", "run", "tag"].include? argv[0]
      config[:command] = argv.shift
      config[:options] << "-h" if argv.delete("-h") || argv.delete("--help")
      config[:options] << "-v" if argv.delete("-v") || argv.delete("--version")
    end

    options = MSpecOptions.new config, "[COMMAND]", "", 28, "   "

    options.separator ""
    options.separator "  The mspec command sets up and invokes the sub-commands"
    options.separator "  (see below) to enable, for instance, running the specs"
    options.separator "  with different implementations like ruby, jruby, rbx, etc.\n"

    options.add_config do |f|
      config[:options] << '-B' << f
    end

    options.add_targets

    options.on("-D", "--gdb", "Run under gdb") do
      config[:flags] << '--gdb'
    end
    options.on("-A", "--valgrind", "Run under valgrind") do
      config[:flags] << '--valgrind'
    end
    options.on("--warnings", "Don't supress warnings") do
      config[:flags] << '-w'
      ENV['OUTPUT_WARNINGS'] = '1'
    end
    options.on("-j", "--multi", "Run multiple (possibly parallel) subprocesses") do
      config[:multi] = true
      config[:options] << "-fy"
    end
    options.add_version
    options.on("-h", "--help", "Show this message") do
      puts options.parser
      exit
    end

    # The rest of the help output
    options.separator "\n  where COMMAND is one of:\n"
    options.separator "    run - Run the specified specs (default)"
    options.separator "    ci  - Run the known good specs"
    options.separator "    tag - Add or remove tags\n"
    options.separator "  mspec COMMAND -h for more options\n"

    config[:options] += options.parser.filter! argv
    options.parse argv
  end

  def register; end

  def parallel
    @parallel ||= !(Object.const_defined?(:JRUBY_VERSION) ||
                  /(mswin|mingw)/ =~ RUBY_PLATFORM)
  end

  def fork(&block)
    parallel ? Kernel.fork(&block) : block.call
  end

  def report(files, timer)
    require 'yaml'

    exceptions = []
    tally = Tally.new

    files.each do |file|
      d = File.open(file, "r") { |f| YAML.load f }
      File.delete file

      exceptions += Array(d['exceptions'])
      tally.files!        d['files']
      tally.examples!     d['examples']
      tally.expectations! d['expectations']
      tally.errors!       d['errors']
      tally.failures!     d['failures']
    end

    print "\n"
    exceptions.each_with_index do |exc, index|
      print "\n#{index+1})\n", exc, "\n"
    end
    print "\n#{timer.format}\n\n#{tally.format}\n"
  end

  def multi_exec(argv)
    timer = TimerAction.new
    timer.start

    files = config[:ci_files].inject([]) do |list, item|
      name = tmp "mspec-ci-multi-#{list.size}"

      rest = argv + ["-o", name, item]
      fork { system [config[:target], *rest].join(" ") }

      list << name
    end

    Process.waitall
    timer.finish
    report files, timer
  end

  def run
    ENV['MSPEC_RUNNER'] = '1'

    argv = config[:flags]
    argv.concat config[:includes]
    argv.concat config[:requires]
    argv << "#{MSPEC_HOME}/bin/mspec-#{ config[:command] || "run" }"
    argv.concat config[:options]

    if config[:multi] and config[:command] == "ci"
      multi_exec argv
    else
      exec config[:target], *argv
    end
  end
end

