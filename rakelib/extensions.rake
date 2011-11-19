# Tasks for building C extensions used mainly by Rubinius, but also by MRI in
# the case of the Melbourne parser extension. The task names are defined to
# permit running the tasks directly, eg
#
#   rake compile:melbourne_rbx
#
# See rakelib/ext_helper.rb for the helper methods and Rake rules.

desc "Build extensions from lib/ext"
task :extensions

def clean_extension(name)
  rm_f FileList["lib/**/ext/#{name}/*.{o,#{$dlext}}"], :verbose => $verbose
end

namespace :extensions do
  desc "Clean all lib/ext files"
  task :clean do
    clean_extension("**")
    rm_f FileList["lib/tooling/**/*.{o,#{$dlext}}"], :verbose => $verbose
    # TODO: implement per extension cleaning. This hack is for
    # openssl and dl, which use extconf.rb and create Makefile.
    rm_f FileList["lib/ext/**/Makefile"], :verbose => $verbose
    rm_f FileList["lib/tooling/**/Makefile"], :verbose => $verbose
    rm_f FileList["lib/ext/dl/*.func"], :verbose => $verbose
  end
end

def rbx_build
  # rbx-build can run even if prefix is used
  File.expand_path "../bin/rbx-build", File.dirname(__FILE__)
end

def build_extconf(name, opts)
  if opts[:ignore_fail]
    fail_block = lambda { |ok, status|  }
  else
    fail_block = nil
  end

  redirect = $verbose || !opts[:ignore_fail] ? "" : "> /dev/null 2>&1"

  puts "Building #{name}"

  ENV["BUILD_RUBY"] = BUILD_CONFIG[:build_ruby]

  include18_dir = File.expand_path("../../vm/capi/18/include", __FILE__)
  include19_dir = File.expand_path("../../vm/capi/19/include", __FILE__)

  unless File.directory? BUILD_CONFIG[:runtime]
    ENV["CFLAGS"]      = "-I#{include18_dir} -I#{include19_dir}"
  end

  ENV["RBXOPT"] = opts[:env] if opts.key? :env

  unless opts[:deps] and opts[:deps].all? { |n| File.exists? n }
    sh("#{rbx_build} extconf.rb #{redirect}", &fail_block)
  end

  sh("make #{redirect}", &fail_block)

  ENV.delete("RBXOPT")
end

def compile_ext(name, opts={})
  names = name.split ":"
  name = names.last
  ext_dir = opts[:dir] || File.join("lib/ext", names)

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
      ext_helper = File.expand_path "../ext_helper.rb", __FILE__
      dep_grapher = File.expand_path "../dependency_grapher.rb", __FILE__
      build_config = File.expand_path "../../config.rb", __FILE__
      Dir.chdir ext_dir do
        if File.exists? "Rakefile"
          sh "#{BUILD_CONFIG[:build_ruby]} -S #{BUILD_CONFIG[:build_rake]} #{'-t' if $verbose} -r #{build_config} -r #{ext_helper} -r #{dep_grapher} #{ext_task_name}"
        else
          build_extconf name, opts
        end
      end
    end
  end

  Rake::Task[:extensions].prerequisites << "extensions:#{task_name}"
end

# TODO: we must completely rebuild the bootstrap version of Melbourne if the
# configured build ruby changes. We add the version to be extra sure.
build_ruby = "lib/ext/melbourne/.build_ruby"
build_version = "#{BUILD_CONFIG[:build_ruby]}:#{RUBY_VERSION}"

if File.exists?(build_ruby)
  File.open(build_ruby, "rb") do |f|
    clean_extension("melbourne/**") if f.read.chomp != build_version
  end
else
  clean_extension("melbourne/**")
end

File.open(build_ruby, "wb") do |f|
  f.puts build_version
end

compile_ext "melbourne", :task => "rbx", :doc => "for Rubinius"
compile_ext "melbourne", :task => "build", :doc => "for bootstrapping"

compile_ext "digest", :dir => "lib/digest/ext"
compile_ext "digest:md5", :dir => "lib/digest/ext/md5"
compile_ext "digest:rmd160", :dir => "lib/digest/ext/rmd160"
compile_ext "digest:sha1", :dir => "lib/digest/ext/sha1"
compile_ext "digest:sha2", :dir => "lib/digest/ext/sha2"
compile_ext "digest:bubblebabble", :dir => "lib/digest/ext/bubblebabble"

compile_ext "bigdecimal", :dir => "lib/18/bigdecimal/ext", :env => "-X18"
compile_ext "bigdecimal", :dir => "lib/19/bigdecimal/ext", :env => "-X19"

compile_ext "syck", :dir => "lib/18/syck/ext"
compile_ext "nkf", :dir => "lib/18/nkf/ext", :env => "-X18"

if BUILD_CONFIG[:readline] == :c_readline
  compile_ext "readline", :dir => "lib/18/readline/ext"
  compile_ext "readline", :dir => "lib/19/readline/ext",
                          :deps => ["Makefile", "extconf.rb"],
                          :env => "-X19"
end

if BUILD_CONFIG[:libyaml]
  compile_ext "psych", :deps => ["Makefile"], :dir => "lib/19/psych/ext", :env => "-X19"
end

compile_ext "syck", :deps => ["Makefile"], :dir => "lib/19/syck/ext", :env => "-X19"

# rbx must be able to run to build these because they use
# extconf.rb, so they must be after melbourne for Rubinius.
compile_ext "openssl", :deps => ["Makefile", "extconf.h"], :dir => "lib/openssl/ext"
compile_ext "dl", :deps => ["Makefile", "dlconfig.h"],
                  :dir => "lib/18/dl/ext", :env => "-X18"
compile_ext "dbm", :ignore_fail => true, :deps => ["Makefile"], :dir => "lib/dbm/ext"
compile_ext "gdbm", :ignore_fail => true, :deps => ["Makefile"], :dir => "lib/gdbm/ext"
compile_ext "sdbm", :deps => ["Makefile"], :dir => "lib/sdbm/ext"

compile_ext "profiler", :dir => "lib/tooling/profiler",
                        :deps => ["Makefile"]
