# -*- ruby -*-

require 'rakelib/git'

namespace :rubyspec do
  desc "Initialize spec/ruby with a rubyspec clone"
  task :init do
    if File.exists? spec_ruby
      unless is_git_project spec_ruby, "rubyspec.git"
        raise "#{spec_ruby} is not a rubyspec clone. Please remove before running this task."
      end
    else
      sh "git clone git://github.com/rubyspec/rubyspec.git #{spec_ruby}"
    end
  end

  desc "Update rubyspec"
  task :update => :init do
    puts "\nUpdating rubyspec repository..."
    Dir.chdir spec_ruby do
      git_update
    end
  end

  desc "Report changes to the rubyspec sources"
  task :status do
    Dir.chdir spec_ruby do
      system "git status"
    end
  end

  desc "Commit changes to the rubyspec sources"
  task :commit do
    puts "\nCommitting changes to rubyspec sources..."
    Dir.chdir spec_ruby do
      sh "git commit -a"
    end
  end

  desc "Push changes to the rubyspec repository"
  task :push => :update do
    puts "\nPushing changes to the rubyspec repository..."
    Dir.chdir spec_ruby do
      git_push
    end
  end

  desc "Synchronize spec/frozen with a current checkout"
  task :sync => :update do
    dir = ENV['DIR'] || spec_ruby

    rm_rf "spec/frozen"
    rsync dir + "/*", "spec/frozen"

    version = Dir.chdir(dir) { `git log --pretty=oneline -1`[0..7] }
    sh "git add spec/frozen/"
    sh "git commit -m 'Updated CI frozen specs to RubySpec #{version}.' spec/frozen"
  end

  desc "Switch to the rubyspec commiter URL"
  task :committer do
    Dir.chdir spec_ruby do
      sh "git config remote.origin.url git@github.com:rubyspec/rubyspec.git"
    end
    puts "\nYou're now accessing rubyspec via the committer URL."
  end

  desc "Switch to the rubyspec anonymous URL"
  task :anon do
    Dir.chdir spec_ruby do
      sh "git config remote.origin.url git://github.com/rubyspec/rubyspec.git"
    end
    puts "\nYou're now accessing rubyspec via the anonymous URL."
  end
end
