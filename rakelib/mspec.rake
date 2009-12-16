# -*- ruby -*-

require 'rakelib/git'

namespace :mspec do
  desc "Synchronize ./mspec with a current clone of MSpec"
  task :sync do
    unless dir = ENV['DIR']
      raise "Use DIR= to specify a clone of mspec"
    end

    unless is_git_project dir, "mspec.git"
      raise "#{dir} is not an mspec clone. Clone from 'git://github.com/rubyspec/mspec.git'"
    end

    rsync dir + "/*", "mspec"

    version = Dir.chdir(ENV['DIR']) { `git log --pretty=oneline -1`[0..7] }
    sh "git add mspec/"
    sh "git commit -m 'Updated MSpec source to #{version}.' mspec"
  end
end
