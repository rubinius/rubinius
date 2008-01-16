# -*- ruby -*-

require 'find'

namespace :stats do
  $first_date = ENV['DATE'] || "2007-11-01"
  $stats_dir = "stats"

  require 'rakelib/git_task'

  desc "Collect statistics for the project"
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

  git_task "spec" do |f, date|
    # all these extra convolutions are to try to protect us from
    # stupid specs that kill the parent process.
    trap 'HUP', 'IGNORE'

    IO.popen "(rake clean && rake && ./bin/ci -C -f s) 2>&1" do |p|
      Process.setpgid(0, 0)
      p.each_line do |l|
        f.puts l
      end
    end
  end

  # This allows the dates to interleave, causing less churn
  Rake::Task["collect"].prerequisites.sort!
  Rake::Task["collect"].prerequisites.reverse!
end
