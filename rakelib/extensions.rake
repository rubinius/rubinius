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
    rm_f FileList["lib/**/ext/**/Makefile"], :verbose => $verbose
    rm_f FileList["lib/tooling/**/Makefile"], :verbose => $verbose
    rm_f FileList["lib/ext/dl/*.func"], :verbose => $verbose
  end

  task :melbourne_build_clean do
    if BUILD_CONFIG[:stagingdir]
      rm_rf FileList["#{BUILD_CONFIG[:stagingdir]}/**/melbourne/build"], :verbose => true
    end
  end
end

def rbx_build
  if BUILD_CONFIG[:stagingdir]
    "#{BUILD_CONFIG[:stagingdir]}#{BUILD_CONFIG[:bindir]}/#{BUILD_CONFIG[:program_name]}"
  else
    "#{BUILD_CONFIG[:sourcedir]}/vm/vm"
  end
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

  include18_dir = "#{BUILD_CONFIG[:sourcedir]}/vm/capi/18/include"
  include19_dir = "#{BUILD_CONFIG[:sourcedir]}/vm/capi/19/include"

  unless File.directory? BUILD_CONFIG[:runtimedir]
    if opts[:env] == "-X18"
      ENV["CFLAGS"] = "-I#{include18_dir}"
    else
      ENV["CFLAGS"] = "-I#{include19_dir}"
    end
  end

  ENV["RBXOPT"] = opts[:env]

  unless opts[:deps] and opts[:deps].all? { |n| File.exists? n }
    sh("#{rbx_build} extconf.rb #{redirect}", &fail_block)
  end

  sh("make #{redirect}", &fail_block)

  ENV.delete("RBXOPT")
end

def compile_ext(name, opts={})
  names = name.split ":"
  name = names.last
  ext_dir = opts[:dir] || File.join("#{libprefixdir}/ext", names)

  if t = opts[:task]
    ext_task_name = "build:#{t}"
    names << t
  else
    ext_task_name = "build"
  end

  opts[:env] ||= "-X18"

  task_name = names.join "_"

  namespace :extensions do
    desc "Build #{name.capitalize} extension #{opts[:doc]}"
    task task_name do
      ext_helper = File.expand_path "../ext_helper.rb", __FILE__
      dep_grapher = File.expand_path "../dependency_grapher.rb", __FILE__
      build_config = File.expand_path "../../config.rb", __FILE__
      Dir.chdir ext_dir do
        if File.exists? "Rakefile"
          ENV["BUILD_VERSION"] = BUILD_CONFIG[:language_version]

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

compile_ext "melbourne", :task => "build",
                         :doc => "for bootstrapping"

compile_ext "melbourne", :task => "rbx",
                         :doc => "for Rubinius"

compile_ext "digest", :deps => ["Makefile", "extconf.rb"],
                      :dir => "#{libprefixdir}/digest/ext"

compile_ext "digest:md5", :deps => ["Makefile", "extconf.rb"],
                          :dir => "#{libprefixdir}/digest/ext/md5"
compile_ext "digest:rmd160", :deps => ["Makefile", "extconf.rb"],
                             :dir => "#{libprefixdir}/digest/ext/rmd160"
compile_ext "digest:sha1", :deps => ["Makefile", "extconf.rb"],
                           :dir => "#{libprefixdir}/digest/ext/sha1"
compile_ext "digest:sha2", :deps => ["Makefile", "extconf.rb"],
                           :dir => "#{libprefixdir}/digest/ext/sha2"
compile_ext "digest:bubblebabble", :deps => ["Makefile", "extconf.rb"],
                                   :dir => "#{libprefixdir}/digest/ext/bubblebabble"

if BUILD_CONFIG[:language_version] == "18"
  compile_ext "18/bigdecimal", :dir => "#{libprefixdir}/18/bigdecimal/ext"

  compile_ext "18/syck", :dir => "#{libprefixdir}/18/syck/ext"
  compile_ext "18/nkf", :dir => "#{libprefixdir}/18/nkf/ext"

  if BUILD_CONFIG[:readline] == :c_readline
    compile_ext "18/readline", :dir => "#{libprefixdir}/18/readline/ext",
             :deps => ["Makefile", "extconf.rb"]
  end

  # rbx must be able to run to build these because they use
  # extconf.rb, so they must be after melbourne for Rubinius.
  compile_ext "18/openssl", :deps => ["Makefile", "extconf.h"],
                            :dir => "#{libprefixdir}/18/openssl/ext"

  compile_ext "18/dl", :deps => ["Makefile", "dlconfig.h"],
                       :dir => "#{libprefixdir}/18/dl/ext"

  compile_ext "18/pty", :deps => ["Makefile"],
                        :dir => "#{libprefixdir}/18/pty/ext"
  compile_ext "18/zlib", :deps => ["Makefile", "extconf.rb"],
                         :dir => "#{libprefixdir}/18/zlib/ext"
  compile_ext "18/iconv", :deps => ["Makefile", "extconf.rb"],
                          :dir => "#{libprefixdir}/18/iconv/ext"

  compile_ext "18/dbm", :deps => ["Makefile"],
                   :dir => "#{libprefixdir}/18/dbm/ext",
                   :ignore_fail => true
  compile_ext "18/gdbm", :deps => ["Makefile"],
                   :dir => "#{libprefixdir}/18/gdbm/ext",
                   :ignore_fail => true
  compile_ext "18/sdbm", :deps => ["Makefile"],
                   :dir => "#{libprefixdir}/18/sdbm/ext"
end

if BUILD_CONFIG[:language_version] == "19"
  compile_ext "19/bigdecimal",
      :dir => "#{libprefixdir}/19/bigdecimal/ext",
      :deps => ["Makefile", "extconf.rb"]
  compile_ext "19/nkf",
      :dir => "#{libprefixdir}/19/nkf/ext",
      :deps => ["Makefile", "extconf.rb"]
  if BUILD_CONFIG[:readline] == :c_readline
    compile_ext "19/readline",
        :dir => "#{libprefixdir}/19/readline/ext",
        :deps => ["Makefile", "extconf.rb"]
  end

  compile_ext "19/psych",
      :dir => "#{libprefixdir}/19/psych/ext",
      :deps => ["Makefile"]

  compile_ext "19/syck",
      :dir => "#{libprefixdir}/19/syck/ext",
      :deps => ["Makefile"]

  compile_ext "json/parser",
      :dir => "#{libprefixdir}/19/json/ext/parser",
      :deps => ["Makefile", "extconf.rb"]
  compile_ext "json/generator",
      :dir => "#{libprefixdir}/19/json/ext/generator",
      :deps => ["Makefile", "extconf.rb"]

  compile_ext "19/openssl",
      :dir => "#{libprefixdir}/19/openssl/ext",
      :deps => ["Makefile", "extconf.h"]
  compile_ext "19/pty",
      :dir => "#{libprefixdir}/19/pty/ext",
      :deps => ["Makefile"]
  compile_ext "19/zlib",
      :dir => "#{libprefixdir}/19/zlib/ext",
      :deps => ["Makefile", "extconf.rb"]
  compile_ext "19/iconv",
      :dir => "#{libprefixdir}/19/iconv/ext",
      :deps => ["Makefile", "extconf.rb"]

  compile_ext "19/dbm", :deps => ["Makefile"],
                   :dir => "#{libprefixdir}/19/dbm/ext",
                   :ignore_fail => true
  compile_ext "19/gdbm", :deps => ["Makefile"],
                   :dir => "#{libprefixdir}/19/gdbm/ext",
                   :ignore_fail => true
  compile_ext "19/sdbm", :deps => ["Makefile"],
                   :dir => "#{libprefixdir}/19/sdbm/ext"
  compile_ext "19/io/console", :deps => ["Makefile"],
                   :dir => "#{libprefixdir}/19/io/console/ext"

end

if BUILD_CONFIG[:language_version] == "20"
  compile_ext "20/bigdecimal",
      :dir => "#{libprefixdir}/20/bigdecimal/ext",
      :deps => ["Makefile", "extconf.rb"]
  compile_ext "20/nkf",
      :dir => "#{libprefixdir}/20/nkf/ext",
      :deps => ["Makefile", "extconf.rb"]
  if BUILD_CONFIG[:readline] == :c_readline
    compile_ext "20/readline",
        :dir => "#{libprefixdir}/20/readline/ext",
        :deps => ["Makefile", "extconf.rb"]
  end

  compile_ext "20/psych",
      :dir => "#{libprefixdir}/20/psych/ext",
      :deps => ["Makefile"]

  compile_ext "20/syck",
      :dir => "#{libprefixdir}/20/syck/ext",
      :deps => ["Makefile"]

  compile_ext "json/parser",
      :dir => "#{libprefixdir}/20/json/ext/parser",
      :deps => ["Makefile", "extconf.rb"]
  compile_ext "json/generator",
      :dir => "#{libprefixdir}/20/json/ext/generator",
      :deps => ["Makefile", "extconf.rb"]

  compile_ext "20/openssl",
      :dir => "#{libprefixdir}/20/openssl/ext",
      :deps => ["Makefile", "extconf.h"]
  compile_ext "20/pty",
      :dir => "#{libprefixdir}/20/pty/ext",
      :deps => ["Makefile"]
  compile_ext "20/zlib",
      :dir => "#{libprefixdir}/20/zlib/ext",
      :deps => ["Makefile", "extconf.rb"]
  compile_ext "20/iconv",
      :dir => "#{libprefixdir}/20/iconv/ext",
      :deps => ["Makefile", "extconf.rb"]


  compile_ext "20/dbm", :deps => ["Makefile"],
                   :dir => "#{libprefixdir}/20/dbm/ext",
                   :ignore_fail => true
  compile_ext "20/gdbm", :deps => ["Makefile"],
                   :dir => "#{libprefixdir}/20/gdbm/ext",
                   :ignore_fail => true
  compile_ext "20/sdbm", :deps => ["Makefile"],
                   :dir => "#{libprefixdir}/20/sdbm/ext"

  compile_ext "20/io/console", :deps => ["Makefile"],
                   :dir => "#{libprefixdir}/20/io/console/ext"
end

compile_ext "profiler", :dir => "#{libprefixdir}/tooling/profiler",
                        :deps => ["Makefile"]

compile_ext "coverage", :dir => "#{libprefixdir}/tooling/coverage/ext",
                        :deps => ["Makefile"]
