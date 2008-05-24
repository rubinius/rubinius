# -*- ruby -*-

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
    sh "git commit -m 'Updated MSpec submodule to #{version}.' mspec"
  end
end
