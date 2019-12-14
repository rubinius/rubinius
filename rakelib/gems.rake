namespace :gems do
  desc 'Install the pre-installed gems'
  task :install do
    begin
      clean_environment

      destdir = ENV["DESTDIR"]
      ENV.delete "DESTDIR"

      ENV["RBX_PREFIX_PATH"] = BUILD_CONFIG[:builddir]
      ENV["RBX_GEMS_PATH"] = BUILD_CONFIG[:builddir]

      Dir.chdir BUILD_CONFIG[:gems_cache] do
        sh "#{BUILD_CONFIG[:build_exe]} #{BUILD_CONFIG[:scriptdir]}/preinstall_gems.rb", :verbose => $verbose
      end

      cp "#{BUILD_CONFIG[:stdlibdir]}/bin/irb",
        "#{BUILD_CONFIG[:builddir]}#{BUILD_CONFIG[:gemsdir]}/bin/", :verbose => $verbose
    ensure
      ENV["DESTDIR"] = destdir

      ENV.delete "RBX_PREFIX_PATH"
      ENV.delete "RBX_GEMS_PATH"
    end
  end
end
