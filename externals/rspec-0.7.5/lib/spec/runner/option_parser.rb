require 'ostruct'
require 'optparse'

module Spec
  module Runner
    class OptionParser
      def initialize
        @spec_parser = SpecParser.new
        @file_factory = File
      end

      def create_context_runner(args, err, out, warn_if_no_files)
        options = parse(args, err, out, warn_if_no_files)

        formatter = options.formatter_type.new(options.out, options.dry_run, options.colour)
        options.reporter = Reporter.new(formatter, options.backtrace_tweaker) 

        # this doesn't really belong here.
        # it should, but the way things are coupled, it doesn't
        if options.differ_class
          Spec::Expectations::Should::Base.differ = options.differ_class.new(options.diff_format, options.context_lines, options.colour)
        end

        ContextRunner.new(options)
      end

      def parse(args, err, out, warn_if_no_files)
        options = OpenStruct.new
        options.out = out == STDOUT ? Kernel : out
        options.formatter_type = Formatter::ProgressBarFormatter
        options.backtrace_tweaker = QuietBacktraceTweaker.new
        options.spec_name = nil

        opts = ::OptionParser.new do |opts|
          opts.banner = "Usage: spec [options] (FILE|DIRECTORY|GLOB)+"
          opts.separator ""

          opts.on("-D", "--diff [FORMAT]", "Show diff of objects that are expected to be equal when they are not",
                                           "Builtin formats: unified|u|context|c",
                                           "You can also specify a custom differ class",
                                           "(in which case you should also specify --require)") do |format|
            require 'spec/expectations/diff'  # patches in diff extension

            # TODO make context_lines settable
            options.context_lines = 3

            case format
              when 'context', 'c'
                options.diff_format  = :context
              when 'unified', 'u', '', nil
                options.diff_format  = :unified
            end

            if [:context,:unified].include? options.diff_format
              require 'spec/expectations/differs/default'
              options.differ_class = Spec::Expectations::Differs::Default
            else
              begin
                options.diff_format  = :custom
                options.differ_class = eval(format)
              rescue NameError
                err.puts "Couldn't find differ class #{format}"
                err.puts "Make sure the --require option is specified *before* --diff"
                exit if out == $stdout
              end
            end

          end
          
          opts.on("-c", "--colour", "--color", "Show coloured (red/green) output") do
            options.colour = true
          end
          
          opts.on("-s", "--spec SPECIFICATION_NAME", "Execute context or specification with matching name") do |spec_name|
            options.spec_name = spec_name
          end
          
          opts.on("-l", "--line LINE_NUMBER", Integer, "Execute context or specification at given line") do |line_number|
            options.line_number = line_number.to_i
          end

          opts.on("-f", "--format FORMAT", "Builtin formats: specdoc|s|rdoc|r|html|h", 
                                           "You can also specify a custom formatter class",
                                           "(in which case you should also specify --require)") do |format|
            case format
              when 'specdoc', 's'
                options.formatter_type = Formatter::SpecdocFormatter
              when 'html', 'h'
                options.formatter_type = Formatter::HtmlFormatter
              when 'rdoc', 'r'
                options.formatter_type = Formatter::RdocFormatter
                options.dry_run = true
            else
              begin
                options.formatter_type = eval(format)
              rescue NameError
                err.puts "Couldn't find formatter class #{format}"
                err.puts "Make sure the --require option is specified *before* --format"
                exit if out == $stdout
              end
            end
          end

          opts.on("-r", "--require FILE", "Require FILE before running specs",
                                          "Useful for loading custom formatters or other extensions",
                                          "If this option is used it must come before the others") do |req|
            require req
          end
          
          opts.on("-b", "--backtrace", "Output full backtrace") do
            options.backtrace_tweaker = NoisyBacktraceTweaker.new
          end

          opts.on("-H", "--heckle CODE", "If all specs pass, this will run your specs many times, mutating",
                                         "the specced code a little each time. The intent is that specs",
                                         "*should* fail, and RSpec will tell you if they don't.",
                                         "CODE should be either Some::Module, Some::Class or Some::Fabulous#method}") do |heckle|
            require 'spec/runner/heckle_runner'
            options.heckle_runner = HeckleRunner.new(heckle)
          end
          
          opts.on("-d", "--dry-run", "Don't execute specs") do
            options.dry_run = true
          end
          
          opts.on("-o", "--out OUTPUT_FILE", "Path to output file (defaults to STDOUT)") do |outfile|
            options.out = File.new(outfile, 'w')
          end
          
          opts.on("-v", "--version", "Show version") do
            out.puts ::Spec::VERSION::DESCRIPTION
            exit if out == $stdout
          end

          opts.on_tail("-h", "--help", "You're looking at it") do
            out.puts opts
            exit if out == $stdout
          end
          
        end
        opts.parse!(args)

        if args.empty? && warn_if_no_files
          err.puts opts
          exit(1) if err == $stderr
        end

        if options.line_number
          set_spec_from_line_number(options, args, err)
        end

        options
      end
      
      def set_spec_from_line_number(options, args, err)
        unless options.spec_name
          if args.length == 1
            if @file_factory.file?(args[0])
              source = @file_factory.open(args[0])
              options.spec_name = @spec_parser.spec_name_for(source, options.line_number)
            elsif @file_factory.directory?(args[0])
              err.puts "You must specify one file, not a directory when using the --line option"
              exit(1) if err == $stderr
            else
              err.puts "#{args[0]} does not exist"
              exit(1) if err == $stderr
            end
          else
            err.puts "Only one file can be specified when using the --line option"
            exit(1) if err == $stderr
          end
        else
          err.puts "You cannot use both --line and --spec"
          exit(1) if err == $stderr
        end
      end
    end
  end
end
