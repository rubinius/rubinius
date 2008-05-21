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

  end
end
