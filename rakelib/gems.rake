def bootstrap_rubinius(cmd)
  ENV["RBX_BOOTSTRAP_LOAD_PATH"] = "1"
  sh cmd
ensure
  ENV.delete "RBX_BOOTSTRAP_LOAD_PATH"
end

namespace :gems do
  desc 'Install the pre-installed gems'
  task :install do
    ENV['GEM_HOME'] = ENV['GEM_PATH'] = nil
    cmd = "#{BUILD_CONFIG[:build_exe]} #{BUILD_CONFIG[:sourcedir]}/rakelib/preinstall_gems.rb"

    Dir.chdir "vendor/cache" do
      bootstrap_rubinius cmd
    end
  end

  task :melbourne do
    prefix = "#{BUILD_CONFIG[:build_prefix]}#{BUILD_CONFIG[:runtimedir]}"
    path = Dir["#{prefix}/gems/rubinius-melbourne-*/ext/rubinius/melbourne"].first
    Dir.chdir path do
      begin
        ENV["RBX_RUN_COMPILED"] = "1"
        ENV["RBX_PREFIX_PATH"] = BUILD_CONFIG[:build_prefix]
        sh "#{BUILD_CONFIG[:build_exe]} ./extconf.rbc"
        sh "#{BUILD_CONFIG[:build_make]} && #{BUILD_CONFIG[:build_make]} install"
      ensure
        ENV.delete "RBX_RUN_COMPILED"
        ENV.delete "RBX_PREFIX_PATH"
      end
    end
  end

  task :extensions do
    gems_dir = "#{BUILD_CONFIG[:build_prefix]}#{BUILD_CONFIG[:runtimedir]}/gems"
    build_gem_extconf = "#{BUILD_CONFIG[:build_exe]} extconf.rb"

    # Build the gems needed to run mkmf.rb
    names = FileList["#{gems_dir}/rubysl-{etc,fcntl}*/**/extconf.rb"]
    names.each do |name|
      Dir.chdir File.dirname(name) do
        bootstrap_rubinius build_gem_extconf
      end
    end

    # Build the rest of the gems
    names = FileList["#{gems_dir}/**/extconf.rb"].exclude(
      %r[.*/rubinius-melbourne-.*],
      %r[.*/rubysl-(etc|fcntl)-.*]
    )

    re = %r[(.*)/(ext|lib)/[^/]+/(.*extconf.rb)]
    names.each do |name|
      next unless m = re.match(name)

      ext_name = "#{File.basename(File.dirname(name))}.#{$dlext}"
      dest_path = File.join m[1], "lib", File.dirname(m[3])
      next if File.exists? File.join(dest_path, ext_name)

      Dir.chdir File.dirname(name) do
        puts "Building #{m[1]}..."

        bootstrap_rubinius build_gem_extconf unless File.exists? "Makefile"
        sh "#{BUILD_CONFIG[:build_make]}", :verbose => $verbose

        if File.exists? ext_name
          FileUtils.mkdir_p dest_path
          FileUtils.cp ext_name, dest_path
        end
      end
    end
  end
end
