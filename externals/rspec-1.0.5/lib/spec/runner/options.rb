module Spec
  module Runner
    class Options
      BUILT_IN_FORMATTERS = {
        'specdoc'  => Formatter::SpecdocFormatter,
        's'        => Formatter::SpecdocFormatter,
        'html'     => Formatter::HtmlFormatter,
        'h'        => Formatter::HtmlFormatter,
        'rdoc'     => Formatter::RdocFormatter,
        'r'        => Formatter::RdocFormatter,
        'progress' => Formatter::ProgressBarFormatter,
        'p'        => Formatter::ProgressBarFormatter,
        'failing_examples' => Formatter::FailingExamplesFormatter,
        'e'        => Formatter::FailingExamplesFormatter,
        'failing_behaviours' => Formatter::FailingBehavioursFormatter,
        'b'        => Formatter::FailingBehavioursFormatter
      }
      
      attr_accessor(
        :backtrace_tweaker,
        :colour,
        :context_lines,
        :diff_format,
        :differ_class,
        :dry_run,
        :examples,
        :failure_file,
        :formatters,
        :generate,
        :heckle_runner,
        :line_number,
        :loadby,
        :reporter,
        :reverse,
        :runner_type,
        :timeout,
        :verbose
      )

      def initialize
        @backtrace_tweaker = QuietBacktraceTweaker.new
        @examples = []
        @formatters = []
        @colour = false
        @dry_run = false
      end

      def create_behaviour_runner
        @formatters.each do |formatter|
          formatter.colour = @colour if formatter.respond_to?(:colour=)
          formatter.dry_run = @dry_run if formatter.respond_to?(:dry_run=)
        end
        @reporter = Reporter.new(@formatters, @backtrace_tweaker)

        # this doesn't really belong here.
        # it should, but the way things are coupled, it doesn't
        if @differ_class
          Spec::Expectations.differ = @differ_class.new(@diff_format, @context_lines, @colour)
        end

        return nil if @generate

        if @runner_type
          @runner_type.new(self)
        else
          BehaviourRunner.new(self)
        end
      end

      def parse_diff(format, out_stream, error_stream)
        @context_lines = 3
        case format
          when :context, 'context', 'c'
            @diff_format  = :context
          when :unified, 'unified', 'u', '', nil
            @diff_format  = :unified
        end

        if [:context,:unified].include? @diff_format
          require 'spec/expectations/differs/default'
          @differ_class = Spec::Expectations::Differs::Default
        else
          begin
            @diff_format  = :custom
            @differ_class = eval(format)
          rescue NameError
            error_stream.puts "Couldn't find differ class #{format}"
            error_stream.puts "Make sure the --require option is specified *before* --diff"
            exit if out_stream == $stdout
          end
        end
      end

      def parse_example(example)
        if(File.file?(example))
          @examples = File.open(example).read.split("\n")
        else
          @examples = [example]
        end
      end

      def parse_format(format, out_stream, error_stream)
        where = out_stream
        # This funky regexp checks whether we have a FILE_NAME or not
        if (format =~ /([a-zA-Z_]+(?:::[a-zA-Z_]+)*):?(.*)/) && ($2 != '')
          format = $1
          where = $2
        else
          raise "When using several --format options only one of them can be without a file" if @out_used
          @out_used = true
        end

        begin
          formatter_type = BUILT_IN_FORMATTERS[format] || eval(format)
          @formatters << formatter_type.new(where)
        rescue NameError
          error_stream.puts "Couldn't find formatter class #{format}"
          error_stream.puts "Make sure the --require option is specified *before* --format"
          exit if out_stream == $stdout
        end
      end

      def parse_require(req)
        req.split(",").each{|file| require file}
      end

      def parse_heckle(heckle)
        heckle_require = [/mswin/, /java/].detect{|p| p =~ RUBY_PLATFORM} ? 'spec/runner/heckle_runner_unsupported' : 'spec/runner/heckle_runner'
        require heckle_require
        @heckle_runner = HeckleRunner.new(heckle)
      end

      def parse_generate_options(options_file, args_copy, out_stream)
        # Remove the --generate-options option and the argument before writing to file
        index = args_copy.index("-G") || args_copy.index("--generate-options")
        args_copy.delete_at(index)
        args_copy.delete_at(index)

        File.open(options_file, 'w') do |io|
          io.puts args_copy.join("\n")
        end
        out_stream.puts "\nOptions written to #{options_file}. You can now use these options with:"
        out_stream.puts "spec --options #{options_file}"
        @generate = true
      end

      def parse_runner(runner, out_stream, error_stream)
        begin
          @runner_type = eval(runner)
        rescue NameError
          error_stream.puts "Couldn't find behaviour runner class #{runner}"
          error_stream.puts "Make sure the --require option is specified."
          exit if out_stream == $stdout
        end
      end
    end
  end
end
