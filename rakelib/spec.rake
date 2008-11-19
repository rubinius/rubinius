# -*- ruby -*-

def spec_target
  target = ENV['SPEC_TARGET'] || 'rubinius'
  ENV['RBX_RUNTIME'] = 'runtime'
  if target == 'rubinius' then
    system %(bin/rbx -e 'puts "rubinius build: \#{Rubinius::BUILDREV}"')
  end
  target
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

  desc "Run all 'known good' specs with SydneyParser"
  task :sydney do
    warn "*** Running with SydneyParser enabled ***"
    ENV['SYDNEY'] = '1'

    sh "bin/mspec ci -t #{spec_target}"
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

  desc "Run continuous integration examples including stdlib with SydneyParser"
  task :full_sydney => :build do
    warn "*** Running with SydneyParser enabled ***"
    ENV['SYDNEY'] = '1'

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
    files = Dir["spec/compiler/*_spec.rb"].reject { |s| s =~ /new_compiler/ }

    sh "bin/mspec -tr #{files.join(' ')}"
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
