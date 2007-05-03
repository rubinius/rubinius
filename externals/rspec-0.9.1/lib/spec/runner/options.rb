module Spec
  module Runner
    class Options
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
    end
  end
end
