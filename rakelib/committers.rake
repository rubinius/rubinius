# shell command for quarterly list of committers
def quarterly_committers(start_month, year=Time.now.year)
  "git log --since='#{start_month}/1/#{year}' --until='#{start_month + 2}/31/#{year}' | git shortlog -n -s -e"
end

def future?(start_month, year=Time.now.year)
  require "date"

  if Date.parse("#{start_month}/1/#{year}") > Date.today
    puts
    puts "ERROR: That's the future!"
    puts
    true
  elsif year < Time.now.year
    false
  elsif Date.parse("#{start_month + 2}/28/#{year}") > Date.today
    puts
    puts "WARNING: That's the current quarter."
    puts
  end
end

namespace :committers do
  desc "Prints list of committers from first calendar quarter of this year"
  task :q1 do
    unless future?(1)
      sh quarterly_committers(1)
    end
  end

  desc "Prints list of committers from second calendar quarter of this year"
  task :q2 do
    unless future?(4)
      sh quarterly_committers(4)
    end
  end

  desc "Prints list of committers from third calendar quarter of this year"
  task :q3 do
    unless future?(7)
      sh quarterly_committers(7)
    end
  end

  desc "Prints list of committers from fourth calendar quarter of this year"
  task :q4 do
    unless future?(10)
      sh quarterly_committers(10)
    end
  end

  namespace :q4 do
    desc "Prints list of committers from fourth calendar quarter of LAST year"
    task :last_year do
      unless future?(10, Time.now.year - 1)
        sh quarterly_committers(10, Time.now.year - 1)
      end
    end
  end
end
