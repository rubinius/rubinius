# -*- ruby -*-

require 'rakelib/git'

namespace :rapa do
  desc "Synchronize ./projects/rapa with external repository"
  task :sync do
    unless dir = ENV['DIR']
      raise "Use DIR= to specify a clone of rapa"
    end

    unless is_git_project dir, "rapa.git"
      raise "#{dir} is not an rapa clone. Clone from 'git://github.com/brixen/rapa.git'"
    end

    rsync dir + "/*", "projects/rapa"

    version = Dir.chdir(ENV['DIR']) { `git log --pretty=oneline -1`[0..7] }
    sh "git add projects/rapa/"
    sh "git commit -m 'Updated Rapa source to #{version}.' projects/rapa"
  end

  desc "Build the Ragel parsers for Array#pack, String#unpack"
  task :build do
    project_dir = File.expand_path("../../projects/rapa", __FILE__)
    vm_builtin  = File.expand_path("../../vm/builtin", __FILE__)

    Dir.chdir project_dir do
      sh "rake build:rbx DIR=#{vm_builtin}"
    end
  end
end
