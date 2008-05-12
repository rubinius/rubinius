# -*- ruby -*-

desc "Run all 'known good' specs (task alias for spec:ci)"
task :spec => 'spec:ci'

namespace :spec do
  namespace :setup do
    # Setup for 'Subtend' specs. No need to call this yourself.
    task :subtend do
      Dir["spec/subtend/**/Rakefile"].each do |rakefile|
        sh "rake -f #{rakefile}"
      end
    end
  end

  desc "Initialize git submodules for mspec and rubyspec"
  task :init => 'mspec:init' do
    unless File.exist? "spec/frozen/1.8"
      puts "Initializing CI rubyspecs submodule..."
      rm_rf "spec/frozen"
      sh "git submodule init spec/frozen"
      sh "git submodule update spec/frozen"
    end
  end

  desc "Clone a committer version of the rubyspecs"
  task :clone do
    unless File.exist? "spec/ruby/.git"
      rm_rf "spec/ruby" if File.exist? "spec/ruby"
      sh "git clone git://github.com/brixen/rubyspec.git spec/ruby"
    end
  end

  desc "Update submodule sources for mspec and rubyspec"
  task :update => %w[init mspec:update clone] do
    sh "git submodule update spec/frozen"

    puts "Updating rubyspec repository..."
    Dir.chdir "spec/ruby" do
      git_update
    end
  end

  task :pull => :update

  desc "Push changes to the rubyspec repository"
  task :push => :update do
    puts "Pushing changes to the rubyspec repository..."
    Dir.chdir "spec/ruby" do
      git_push
    end
  end

  desc "Switch to the rubyspec commiter URL"
  task :committer do
    Dir.chdir "spec/ruby" do
      sh "git config remote.origin.url git@github.com:brixen/rubyspec.git"
    end
    puts "\nYou're now accessing rubyspec via the committer URL."
  end

  desc "Switch to the rubyspec anonymous URL"
  task :anon do
    Dir.chdir "spec/ruby" do
      sh "git config remote.origin.url git://github.com/brixen/rubyspec.git"
    end
    puts "\nYou're now accessing rubyspec via the anonymous URL."
  end

  desc "Run continuous integration examples"
  task :ci => :build do
    clear_compiler
    sh "bin/mspec ci -t #{spec_target}"
  end

  desc "Run continuous integration examples including stdlib"
  task :full => :build do
    clear_compiler
    sh "bin/mspec ci -t #{spec_target} -B full.mspec"
  end

  spec_targets = %w(compiler core language library parser rubinius)
  # Build a spec:<task_name> for each group of Rubinius specs
  spec_targets.each do |group|
    desc "Run #{group} examples"
    task group do
      sh "bin/mspec spec/#{group}"
    end
  end

  desc "Run subtend (Rubinius C API) examples"
  task :subtend => "spec:setup:subtend" do
    sh "bin/mspec spec/rubinius/subtend"
  end

  # Specdiffs to make it easier to see what your changes have affected :)
  desc 'Run specs and produce a diff against current base'
  task :diff => 'diff:run'

  namespace :diff do
    desc 'Run specs and produce a diff against current base'
    task :run do
      system 'bin/mspec -f ci -o spec/reports/specdiff.txt spec'
      system 'diff -u spec/reports/base.txt spec/reports/specdiff.txt'
      system 'rm spec/reports/specdiff.txt'
    end

    desc 'Replace the base spec file with a new one'
    task :replace do
      system 'bin/mspec -f ci -o spec/reports/base.txt spec'
    end
  end

  desc "Run the rspec specs for mspec"
  task :mspec do
    # Use the rspec spec runner (see mspec/README; gem install rspec)
    sh 'spec ./mspec/spec'
  end

  task :r2r do
    puts ARGV.inspect
  end
end
