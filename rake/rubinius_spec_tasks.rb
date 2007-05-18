begin
  require 'spec/rake/spectask'
rescue LoadError
  raise <<-EOM
  Unable to load spec/rake/spectask. RSpec is a requirement to build Rubinius.
  Please install RSpec before building (http://rspec.rubyforge.org).
EOM
end

require File.join(File.dirname(__FILE__),'group_coverage_report')
require File.join(File.dirname(__FILE__),'group_spec_task')
require File.join(File.dirname(__FILE__),'specific_group_spec_task')

ROOT = File.expand_path(File.join(File.dirname(__FILE__),'..'))
def from_root_dir
  dir = Dir.pwd
  Dir.chdir(ROOT)
  yield
  Dir.chdir(dir)
end

namespace :spec do
  desc "Run all specs and tests."
  task :all do
    Rake::Task['spec:only'].invoke  rescue got_error = true
    Rake::Task['test:all'].invoke   rescue got_error = true

    raise "Spec or test failures." if got_error
  end
  
  desc "Setup code-cache directory"
  task :setup => 'setup:code_cache'
  
  namespace :setup do
    desc "Setup code-cache directory"
    task :code_cache do
      from_root_dir do
        Dir.mkdir "code-cache" unless File.exists?("code-cache")
        FileUtils.rm Dir.glob("code-cache/*")
      end
    end
    
    task :subtend do
      Dir[File.join(ROOT,"spec/subtend/**/Rakefile")].each do |rakefile|
        sh "rake -f #{rakefile}"
      end
    end
  end
  
  desc "Run only specs but not any tests."
  spec_targets = %w(language shotgun library core targets primitives)

  # Convenience method to run a single spec test
  spec_targets.each do |group|
    spec_files = Dir[ File.join(ROOT,"spec/#{group}/*_spec.rb") ]
    GroupSpecTask.new(group)
    namespace group do
      spec_files.each do |file|
        SpecificGroupSpecTask.new(File.basename(file, '_spec.rb'), :core)
      end
    end
  end

  task :only => spec_targets.collect! { |g| 'spec:' << g }

  # experimental -- need to adjust exclusions depending on what your testing
  namespace :coverage do
    desc "Generate a coverage report for the library specs."
    GroupCoverageReport.new(:library)

    desc "Generate a coverage report for the core specs."
    GroupCoverageReport.new(:core)
  end
  
  # New runner
  desc 'Run all specs'
  task :new do
    from_root_dir do
      system 'bin/specrunner spec' 
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

task :report => 'report:all' # default
namespace :report do
  desc "Build all reports"
  task :all => [:completeness, :rspec_errors]

  desc "Build completeness report"
  task :completeness do
    run_report(:completeness)
  end

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

