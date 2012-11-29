# -*- ruby -*-

require 'rakelib/git'

namespace :winpthreads do
  desc "Synchronize ./vendor/winpthreads with a current clone of winpthreads"
  task :sync do
    unless src_dir = ENV['DIR']
      raise "Use DIR= to specify a clone of winpthreads"
    end
    dest_dir = "vendor/winpthreads"

    unless is_git_project src_dir, "winpthreads.git"
      raise "#{src_dir} is not a winpthreads clone. Clone from 'git://github.com/brixen/winpthreads.git'"
    end

    rm_rf dest_dir
    rsync "#{src_dir}/*", dest_dir

    version = Dir.chdir(src_dir) { `git log --pretty=oneline -1`[0..7] }
    sh "git add -f #{dest_dir}"
    sh "git commit -m 'Updated winpthreads to #{version}.' #{dest_dir}"
  end
end
