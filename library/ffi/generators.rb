# We need to load during build when the Rubinius lib dir cannot be in
# $LOAD_PATH because the bootstrap ruby will load other lib contents.
base = File.expand_path "../", __FILE__

require base + '/generators/structures'
require base + '/generators/constants'
require base + '/generators/types'

module Rubinius
module FFI
  module Generators

    ##
    # BodyGuard does your file system dirty work and cleans up after any
    # fallout.

    class BodyGuardError < Exception; end

    class BodyGuard
      def initialize(subject, label, platform)
        @subject = subject
        @label = label.gsub(/\W/, '-')
        @platform = platform
      end

      ##
      # Orchestrates a workflow by querying the subject at each stage and cleans
      # up if problems arise before raising an exception.
      #
      # Expects the subject to respond to the following methods:
      #
      #   #source io
      #     Where io is an IO instance used to create the source for later
      #     stages.
      #
      #   #prepare name, target
      #     Where name is the source file name and target is the file that would
      #     be created by the prepare process. The method should return the
      #     command to run.
      #
      #   #prepare_failed
      #     The method should return the error message for #raise to which will
      #     be appended the output of running the command returned by #prepare.
      #
      #   #process target
      #     Where target is the same as passed to #prepare. The method should
      #     return the command to run. If no further options or changes are
      #     needed, #process should just return target.
      #
      #   #process_failed
      #     The method should return the error message for #raise to which will
      #     be appended the output of running the command returned by #process.
      #
      # The #perform method returns the result of running the command returned
      # by the #process method.

      def perform
        begin
          name = "rbx-ffi-generators-#{@label}"
          source = File.expand_path name + @platform.source_ext
          target = File.expand_path name + @platform.executable_ext

          File.open source, "wb" do |f|
            @subject.source f
          end

          if preparer = @subject.prepare(source, target)
            handle preparer, :prepare_failed
          else
            target = source
          end

          processor = @subject.process target
          return handle(processor, :process_failed)
        ensure
          remove source, target
        end
      end

      def handle(command, failure)
        result = `#{command}`
        Process.waitpid $?.pid rescue nil

        unless $?.success?
          result = result.split("\n").map { |l| "\t#{l}" }.join "\n"
          msg = "#{@subject.send failure}:\n#{result}"
          raise BodyGuardError, msg
        end

        result
      end

      def remove(*names)
        names.each do |name|
          File.delete name if File.exist? name
        end
      end
    end

    class Platform
      # TODO: Make these configurable to enable cross-compiling

      def initialize(kind=:c)
        @kind = kind
      end

      def source_ext
        case @kind
        when :c
          ".c"
        when :cpp
          ".cpp"
        end
      end

      def executable_ext
        windows? ? ".exe" : ""
      end

      def defines
        Rubinius::BUILD_CONFIG[:defines].map{|f| "-D#{f}" }.join(" ")
      end

      def windows?
        RUBY_PLATFORM =~ /mswin|mingw/
      end

      def compiler
        case @kind
        when :c
          Rubinius::BUILD_CONFIG[:cc]
        when :cpp, :cxx
          Rubinius::BUILD_CONFIG[:cxx]
        else
          Rubinius::BUILD_CONFIG[:cc]
        end
      end

      def language
        case @kind
        when :c
          "c"
        when :cpp, :cxx
          "c++"
        else
          "c"
        end
      end

      def compile(include_dirs, source, target)
        includes = include_dirs.map { |i| "-I#{i}" }.join(" ")
        compile_options = "#{defines} -x #{language} #{includes} -Wall -Werror"

        "#{compiler} #{compile_options} #{source} -o #{target} 2>&1"
      end
    end
  end
end
end
