require 'date'
require 'time'

$dates=(Date.parse($first_date)..(Date.today-1)).map { |d| d.to_s }
$git_dir = File.join $stats_dir, "proj"
$revisions = nil

def git_revisions
  unless $revisions then
    $revisions, all, min = {}, {}, Time.parse($first_date)
    
    `git log --pretty=format:"%ci: %H"`.each do |line|
      time, hash = line.split(/: /)
      raise "line = #{line}" unless time and hash
      time = Time.parse(time).utc
      next if time < min
      all[time] = hash
    end

    all.sort.each do |time, hash|
      key = time.strftime("%Y-%m-%d")
      next if $revisions.has_key? key
      $revisions[key] = hash
      last = hash
    end

    backfill = nil
    $dates.each do |date|
      $revisions[date] ||= backfill
      backfill = $revisions[date]
    end
  end

  $revisions
end

def git_task type, &block
  task type => $dates.map { |date| File.join $stats_dir, "#{date}.#{type}" }
  task :collect => type

  rule ".#{type}" do |t|
    file = t.name
    date = file[/\d\d\d\d-\d\d-\d\d/]
    hash = git_revisions[date]
    File.open file, 'w' do |f|
      Dir.chdir $git_dir do
        sh "git checkout -q #{hash}"
        block[f, date]
      end
    end
  end
end

