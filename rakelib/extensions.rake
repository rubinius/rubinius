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

compile_ext "profiler", :dir => "#{libprefixdir}/tooling/profiler",
                        :deps => ["Makefile"]

compile_ext "coverage", :dir => "#{libprefixdir}/tooling/coverage/ext",
                        :deps => ["Makefile"]
