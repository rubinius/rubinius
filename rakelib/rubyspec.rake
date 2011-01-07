# -*- ruby -*-

require 'rakelib/git'

namespace :rubyspec do
  desc "Synchronize ./spec/ruby with a current clone of RubySpec"
  task :sync do
    unless dir = ENV['DIR']
      raise "Use DIR= to specify a clone of rubyspec"
    end

    unless is_git_project dir, "rubyspec.git"
      raise "#{dir} is not a rubyspec clone. Clone from 'git://github.com/rubyspec/rubyspec.git'"
    end

    rm_rf "spec/ruby"
    rm_rf "#{dir}/optional/capi/ext/rubyspec_version.h"
    rsync "#{dir}/*", "spec/ruby"
    cp "#{dir}/.gitignore", "spec/ruby"

    version = Dir.chdir(dir) { `git log --pretty=oneline -1`[0..7] }
    sh "git add spec/ruby/"
    sh "git commit -m 'Updated CI specs to RubySpec #{version}.' spec/ruby"
  end
end
