#!/usr/bin/env ruby

##
# Build dir structure:
#
# root/
#   current   - file with latest built hash
#   HEAD/...
#   builds/hash/...
#
# Strategy:
#
# 1) start and build necessary dir structure
# 2) head CI trigger URL to get timestamp
# 3) sleep and repeat 3 if date unchanged
# 4) if first time or timestamp has changed, run multiruby_update
# 5) git pull on rubyspec dir
# 6) run multiruby on the rubyspecs
# 7) split results from multiruby output
# 8) submit each result separately with rubyspec hash & ruby impl id (-v)
#
# Requirements:
#
# 1) sudo gem install ZenTest
# 2) multiruby_setup update_rubygems
# 3) multiruby_setup help and run spec(s) for your test target, e.g.:
#    multiruby_setup mri:svn:branches - for all MRI active branches
# 4) multiruby -S gem install mspec

require 'time'
require 'fileutils'
require 'net/http'
require 'yaml'

require 'rubygems'
require 'mspec'

$i = $r = $t = $u = $v = false

# evan thinks writing this is better than calling with full path in
# crontab... um. ick. stupid linux's env
while flag = ARGV.shift
  case flag
  when "-i" then # turn on incremental builds
    $i = true
  when "-r" then
    $r = ARGV.shift.downcase.to_sym
  when "-t" then
    $t = true
  when "-u" then
    $u = true
  when "-v" then
    $v = true
  end
end

$VERBOSE = true

abort "need -r (mri|rbx|...) option" unless $r

# tweakables
BASE_DIR     = "/tmp/ci"
SPEC_REPO    = 'git://github.com/rubyspec/rubyspec.git'
CGI_URI      = 'http://ci.rubini.us/cgi-bin/ci_submit.cgi'
TRIGGER_HOST = 'ci.rubini.us'

# don't modify these:
HEAD_DIR     = File.join(BASE_DIR, "HEAD")
TEST_DIR     = File.join(BASE_DIR, "builds")
TIME_PATH    = File.join(BASE_DIR, "latest_#{"incremental_" if $i}hash.txt")

# ARGH
ENV["RBX"]   = "rbx.colorize_backtraces=no"

############################################################
# Core Method

def _run
  build_dir_structures

  Dir.chdir HEAD_DIR do
    warn "rolling back 5 commits for testing"
    cmd "git reset --hard HEAD~5"
    hash, _ = git_log_hashes[10]
    update_build hash
    File.utime 0, 0, TIME_PATH
  end if $t

  Dir.chdir BASE_DIR do
    loop do
      update_scm

      if there_is_shit_to_do? then
        update_scm :full

        new_hashes.each do |hash|
          update_build hash

          Dir.chdir TEST_DIR do
            run_tests hash
            submit hash
          end
        end
      end

      sleep($t ? 15 : 120)
    end
  end
end

############################################################
# Utility Methods

def build_dir_structures
  FileUtils::mkdir_p TEST_DIR unless File.directory? TEST_DIR

  Dir.chdir BASE_DIR do
    cmd "git clone -n #{SPEC_REPO} HEAD"
    hash, _ = git_log_hashes.first
    update_build hash
  end unless File.directory? HEAD_DIR
end

def cmd cmd, fatal = true
  warn "cmd = #{cmd}"
  raise "cmd #{cmd.inspect} failed with #{$?}" if fatal unless system cmd
end

def git_log_hashes
  Dir.chdir HEAD_DIR do
    `git log --pretty=format:"%H %ae"`.split(/\n+/).map {|s| s.split }
  end
end

def git_pull dir
  Dir.chdir dir do
    cmd "git pull"
  end
end

def head_time path
  response = nil
  Net::HTTP.start(TRIGGER_HOST, 80) {|http|
    response = http.head path
  }

  Time.parse(response["last-modified"]) rescue Time.now
end

def new_hashes
  logs      = git_log_hashes
  curr_hash = File.read(TIME_PATH).chomp || logs.first.first

  build_hashes, latest_author = [], nil
  logs.each do |hash, author|
    break if hash == curr_hash
    build_hashes << hash unless latest_author == author
    latest_author = author
  end

  build_hashes.reverse
end

def not_implemented_yet
  abort "NIY: #{caller.first}"
end

def run_tests hash
  warn "building #{hash}" if $v
  dir = $i ? "incremental" : hash
  unless File.directory? dir then
    cmd "git clone -q -l #{HEAD_DIR} #{dir}"
  else
    git_pull dir
    Dir.chdir dir do
      cmd "git reset --hard #{hash}"
    end
  end

  Dir.chdir dir do
    # TODO: stupid rbx needs to go against frozen, not 1.8
    # TODO: way to totally botch things up with complexity
    cmd "multiruby -v -S mspec 1.8 > ../#{hash}.log 2>&1 < /dev/null", false
  end
ensure
  FileUtils::rm_rf hash # not dir, hash... incremental stays as a result
end

def scm_time
  head_time "/trigger/#{$r}.yaml"
end

def spec_time
  head_time "/trigger/spec.yaml"
end

def submit hash
  target = $r

  full_log = File.read("#{hash}.log")

  full_log.scan(/^VERSION =.*?RESULT = \d+/m).each do |log|
    warn "submitting #{hash} to #{CGI_URI}" if $v
    data = {
      :platform    => Gem::Platform.local.to_s,
      :incremental => $i,
      :hash        => hash,
      :target      => target,
      :log         => log,
    }

    unless $t then
      res = Net::HTTP.post_form(URI.parse(CGI_URI), "data" => YAML.dump(data))
      warn res.body
    else
      warn YAML.dump(data)
    end
  end
ensure
  File.unlink hash rescue nil
end

def there_is_shit_to_do?
  prev_time   = File.mtime(TIME_PATH)
  newest_time = [scm_time, spec_time].max

  prev_time < newest_time
end

def update_build hash
  File.open(TIME_PATH, 'w') do |f|
    f.puts hash
  end
end

def update_scm full = false
  cmd "multiruby_setup update", false if full if $u
  git_pull HEAD_DIR
end

# end
###################################################

_run if $0 == __FILE__
