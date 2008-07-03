#!/usr/bin/env ruby

# See README for instructions.

require 'time'
require 'fileutils'
require 'net/http'
require 'yaml'

require 'rubygems'

$d = $i = $r = $t = $v = false

# evan thinks writing this is better than calling with full path in
# crontab... um. ick. stupid linux's env
while flag = ARGV.shift
  case flag
  when "-i" then # turn on incremental builds
    $i = true
  when "-d" then # change the directory we run in
    $d = ARGV.shift
  when "-r" then # what ruby impl to run against (rbx, mri, etc)
    $r = ARGV.shift.downcase.to_sym
  when "-t" then # test mode
    $t = true
  when "-v" then # verbose mode
    $v = true
  end
end

$VERBOSE = true

abort "need -r (mri|rbx|...) option" unless $r

# tweakables
BASE_DIR     = $d || "/tmp/ci"
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
  warn "dir = #{Dir.pwd}" if $v
  raise "cmd #{cmd.inspect} failed with #{$?}" if fatal unless system cmd
end

def git_log_hashes
  Dir.chdir HEAD_DIR do
    `git log --pretty=format:"%H %ae"`.split(/\n+/).map {|s| s.split }
  end
end

def git_fetch dir
  Dir.chdir dir do
    cmd "git fetch"
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
  arg = "1.8"
  log = File.join Dir.pwd, "#{hash}.log"

  unless File.directory? dir then
    cmd "git clone -q -l #{HEAD_DIR} #{dir}"
  else
    git_fetch dir
    Dir.chdir dir do
      cmd "git reset --hard #{hash}"
    end
  end

  # NOTE: I really really really don't want this conditional here...
  #       CI should _NOT_ have implementation specific knowledge in
  #       it, but I don't have a choice because rubinius isn't up to
  #       snuff with the rest of the impls, so it can't run against
  #       the raw rubyspecs (without being horribly red). This is in
  #       part because rubyspecs is trying to not have rubinius
  #       specific code in it (tho, it does in the form of
  #       guards)... I is a trade-off, and the smallest diff lies
  #       here:

  case $r
  when :mri then
    # change nothing... yay
  when :rbx then
    dir = File.expand_path "~/.multiruby/install/rubinius"
    arg = "ci spec/frozen"
  else
    raise "unknown implementation: #{$r}"
  end

  Dir.chdir dir do
    cmd "multiruby -v -S mspec #{arg} > #{log} 2>&1 < /dev/null", false
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
      warn res.body if $v
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
  cmd "multiruby_setup update", false if full
  git_fetch HEAD_DIR
end

# end
###################################################

_run if $0 == __FILE__
