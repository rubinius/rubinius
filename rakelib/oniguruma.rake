# -*- ruby -*-

require 'rakelib/git'

namespace :oniguruma do
  desc "Synchronize ./vendor/oniguruma with a current clone of rubinius/oniguruma"
  task :sync do
    unless dir = ENV['DIR']
      raise "Use DIR= to specify a clone of oniguruma"
    end

    unless is_git_project dir, "oniguruma.git"
      raise "#{dir} is not an oniguruma clone. Clone from 'git://github.com/rubinius/oniguruma.git'"
    end

    src_dir = "vendor/oniguruma"
    rm_rf src_dir
    rsync "#{dir}/*", src_dir

    version = Dir.chdir(dir) { `git log --pretty=oneline -1`[0..7] }
    sh "git add #{src_dir}"
    sh "git commit -m 'Updated vendor/oniguruma to #{version}.' #{src_dir}"
  end
end
