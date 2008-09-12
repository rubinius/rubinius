# -*- ruby -*-

def spec_ruby
  "#{RUBINIUS_BASE}/spec/ruby"
end

def spec_target
  target = ENV['SPEC_TARGET'] || 'rubinius'
  ENV['RBX_RUNTIME'] = 'runtime'
  if target == 'rubinius' then
    system %(vm/vm -e 'puts "rubinius build: \#{Rubinius::BUILDREV}"'
)
  end
  target
end

Rsync_options = "-avP --delete --exclude '*svn*' --exclude '*swp' --exclude '*rbc' --exclude '*.rej' --exclude '*.orig'"

def rsync(left, right)
  sh "rsync #{Rsync_options} #{left} #{right}"
end

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

  desc "Initialize #{spec_ruby} with a rubyspec clone"
  task :init do
    unless is_git_dir spec_ruby
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

  desc "Commit changes to rubyspec sources"
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
  task :sync do
    dir = ENV['DIR'] || "spec/ruby"

    unless is_git_dir(dir)
      raise "#{dir} isn't a rubyspec checkout. Use spec:init to get it."
    end

    rsync dir + "/*", "spec/frozen"

    version = Dir.chdir(dir) { `git log --pretty=oneline -1`[0..7] }
    sh "git add spec/frozen/"
    sh "git commit -m 'Updated RubySpec source to #{version}.' spec/frozen"
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

  desc "Run all rubyspecs under MatzRuby"
  task :check do
    sh "bin/mspec -t ruby #{spec_ruby}"
  end

  desc "Run continuous integration examples"
  task :ci => :build do
    sh "bin/mspec ci -t #{spec_target}"
  end

  desc "Run continuous integration examples including stdlib"
  task :full => :build do
    clear_compiler
    sh "bin/mspec ci -t #{spec_target} -B full.mspec"
  end

  desc "Run continuous integration examples including stdlib on multiple processors"
  task :multi => :build do
    clear_compiler
    sh "bin/mspec ci -j -t #{spec_target} -B full.mspec"
  end

  spec_targets = %w(core language library parser rubinius)
  # Build a spec:<task_name> for each group of Rubinius specs
  spec_targets.each do |group|
    desc "Run #{group} examples"
    task group do
      sh "bin/mspec spec/#{group}"
    end
  end

  desc "Run compiler examples"
  task :compiler do
    sh "bin/mspec -tr spec/compiler"
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
