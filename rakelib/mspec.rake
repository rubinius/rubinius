# -*- ruby -*-

# TODO remove this when the compiler is
# working in the C++ VM
namespace :build do
  desc "Temp task to build mspec with MRI"
  task :mspec do
    files = Dir["mspec/**/*.rb"]
    files.concat %w(lib/pp.rb
                    lib/prettyprint.rb
                    lib/time.rb
                    lib/rbconfig.rb
                    lib/fileutils.rb
                    lib/yaml.rb
                   )
    files.each do |path|
      compile_ruby path, "#{path}c", true
    end
  end
end

namespace :mspec do
  desc "Synchronize mspec with another checkout"
  task :sync do
    unless dir = ENV['DIR']
      raise "Use DIR= to specify a checkout of mspec"
    end

    unless File.directory?(dir)
      raise "#{dir} isn't an mspec checkout. Use 'cd ~/git; git clone git://github.com/brixen/mspec.git'"
    end

    rsync dir + "/*", "mspec"

    version = Dir.chdir(ENV['DIR']) { `git log --pretty=oneline -1`[0..7] }
    sh "git add mspec/"
    sh "git commit -m 'Updated MSpec source to #{version}.' mspec"
  end
end
