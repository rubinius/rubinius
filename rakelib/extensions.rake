# Tasks for building C extensions used mainly by Rubinius, but also by MRI in
# the case of the Melbourne parser extension. The task names are defined to
# permit running the tasks directly, eg
#
#   rake compile:melbourne_rbx
#
# See rakelib/ext_helper.rb for the helper methods and Rake rules.

desc "Build extensions from lib/ext"
task :extensions

namespace :extensions do
  desc "Clean all lib/ext files"
  task :clean do
    rm_f FileList["lib/ext/**/*.{o,#{$dlext}}"], :verbose => $verbose
    # TODO: implement per extension cleaning. This hack is for
    # openssl and dl, which use extconf.rb and create Makefile.
    rm_f FileList["lib/ext/**/Makefile"], :verbose => $verbose
    rm_f FileList["lib/ext/dl/*.func"], :verbose => $verbose
  end
end

def compile_ext(name, opts={})
  names = name.split ":"
  name = names.last
  ext_dir = File.join "lib/ext", names

  if t = opts[:task]
    ext_task_name = "build:#{t}"
    names << t
  else
    ext_task_name = "build"
  end

  task_name = names.join "_"

  namespace :extensions do
    desc "Build #{name.capitalize} extension #{opts[:doc]}"
    task task_name do
      # rbx-build can run even if prefix is used
      rbx = File.expand_path "../bin/rbx-build", File.dirname(__FILE__)

      ext_helper = File.expand_path "../ext_helper.rb", __FILE__
      dep_grapher = File.expand_path "../dependency_grapher.rb", __FILE__
      Dir.chdir ext_dir do
        if File.exists? "Rakefile"
          ruby "-S rake #{'-t' if $verbose} -r #{ext_helper} -r #{dep_grapher} #{ext_task_name}"
        else
          unless File.directory? BUILD_CONFIG[:runtime]
            ENV["CFLAGS"]      = "-Ivm/capi/include"
          end

          unless File.exists?("Makefile") and File.exists?("extconf.h")
            sh "#{rbx} extconf.rb"
          end

          sh "make"
        end
      end
    end
  end

  Rake::Task[:extensions].prerequisites << "extensions:#{task_name}"
end

compile_ext "bigdecimal"
compile_ext "readline"
compile_ext "digest"
compile_ext "digest:md5"
compile_ext "digest:rmd160"
compile_ext "digest:sha1"
compile_ext "digest:sha2"
compile_ext "digest:bubblebabble"
compile_ext "syck"
compile_ext "melbourne", :task => "rbx", :doc => "for Rubinius"
compile_ext "melbourne", :task => "mri", :doc => "for MRI"
compile_ext "nkf"

# rbx must be able to run to build these because they use
# extconf.rb, so they must be after melbourne for Rubinius.
compile_ext "openssl"
compile_ext "dl"
