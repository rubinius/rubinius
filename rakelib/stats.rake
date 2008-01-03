# -*- ruby -*-

require 'find'

namespace :stats do
  $first_date = "2007-11-01"
  $stats_dir = "stats"

  require 'rakelib/git_task'

  task :collect => [$git_dir]

  directory $stats_dir

  task $git_dir => $stats_dir do
    unless test ?d, $git_dir then
      parent = Dir.pwd
      Dir.chdir $stats_dir do
        sh "git clone -q #{parent} proj"
      end
    end
  end

  git_task "size" do |f, date|
    stats = Hash.new 0
    Find.find '.' do |file|
      next unless test ?f, file
      stats[File.extname(file)] += 1
    end
    f.puts stats.inspect
  end

  git_task "flog" do |f, date|
    f.puts `flog -c kernel 2>&1`
  end
end
