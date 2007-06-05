begin
  require 'spec/rake/spectask'
rescue LoadError
  raise <<-EOM
  Unable to load spec/rake/spectask. RSpec is a requirement to build Rubinius.
  Please install RSpec before building (http://rspec.rubyforge.org).
EOM
end

ROOT = File.expand_path(File.join(File.dirname(__FILE__),'..'))
def from_root_dir
  dir = Dir.pwd
  Dir.chdir(ROOT)
  yield
  Dir.chdir(dir)
end

namespace :spec do
  namespace :setup do
    task :subtend do
      Dir[File.join(ROOT,"spec/subtend/**/Rakefile")].each do |rakefile|
        sh "rake -f #{rakefile}"
      end
    end
  end
  
  spec_targets = %w(compiler core incompatible language library parser rubinius)
  # Build a spec:<task_name> for each group of Rubinius specs
  spec_targets.each do |group|
    desc "Run '#{group}' specifications"
    task group do
      from_root_dir do
        sh "bin/specrunner spec/#{group}"
      end
    end
  end

  desc "Run 'subtend' specifications"
  task :subtend => "spec:setup:subtend" do
    from_root_dir do
      sh "bin/specrunner spec/subtend"
    end
  end

  # Specdiffs to make it easier to see what your changes have affected :)
  desc 'Run specs and produce a diff against current base'
  task :diff => 'diff:run'

  namespace :diff do
    desc 'Run specs and produce a diff against current base'
    task :run do
      from_root_dir do
        system 'bin/specrunner spec > /tmp/rbs_specdiff' 
        system 'diff -u spec/diffs/base.txt /tmp/rbs_specdiff'
        system 'rm /tmp/rbs_specdiff'
      end
    end

    desc 'Replace the base spec file with a new one'
    task :replace do
      from_root_dir do
        system 'bin/specrunner spec > spec/diffs/base.txt' 
      end
    end
  end
end 

#task :report => 'report:all' # default
namespace :report do
  #desc "Build all reports"
  #task :all => [:completeness, :rspec_errors]

  #desc "Build completeness report"
  #task :completeness do
  #  run_report(:completeness)
  #end

  desc "Build rspec errors report"
  task :rspec_errors do
    run_report(:rspec_errors)
  end

  desc "Build self-hosting errors report"
  task :self_hosting_errors do
    run_report(:self_hosting_errors)
  end

  def run_report(name)
    report = "#{ROOT}/bin/reports/#{name}.rb"
    dest   = "#{ROOT}/reports/#{name}.html"
    File.open("#{ROOT}/reports/#{name}.html", 'w') do |f|
      f.write `RUBYOPT='' ruby #{report}`
    end
    puts "ruby #{report} > #{dest}"
  end
end

