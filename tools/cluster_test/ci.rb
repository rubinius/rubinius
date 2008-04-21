#!/usr/bin/env ruby -ws

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
# 2) check out HEAD from git if necessary
# 3) head git atom feed and get date
# 4) sleep and repeat 3 if date unchanged
# 5) if first time or date has changed, get atom feed
# 6) get hashes and usernames newer than recorded
# 7) build latest hash PER USER
# 8) record latesh hash

require 'fileutils'
require 'net/http'
require 'yaml'

$v ||= false
$i ||= false # use -i to turn on incremental builds

# tweakables, preferably through cmdline:
BASE_DIR  = File.expand_path(ARGV.shift || "/tmp/ci")
GIT_REPO  = ARGV.shift || 'git@git.rubini.us:code'
CGI_URI   = (ARGV.shift ||
             GIT_REPO.sub(/git@git\.(.*):\w+$/,
                          'http://ci.\1/cgi-bin/ci_submit.cgi'))

# don't modify these:
HEAD_DIR  = File.join(BASE_DIR, "HEAD")
BUILD_DIR = File.join(BASE_DIR, "builds")
GIT_HASH  = GIT_REPO.sub(/git@/, 'git://').sub(/:(\w+)$/, '/\1')
HASH_PATH = File.join(BASE_DIR, 'latest_hash.txt')

def cmd cmd
  puts "cmd = #{cmd}"
  raise "cmd #{cmd.inspect} failed with #{$?}" unless system cmd
end

def build_dir_structures
  FileUtils::mkdir_p BUILD_DIR unless File.directory? BUILD_DIR

  Dir.chdir BASE_DIR do
    cmd "git clone -n #{GIT_REPO} HEAD"
  end unless File.directory? HEAD_DIR
end

def git_latest_hash
  `git ls-remote -h #{GIT_HASH} refs/heads/master`.split.first
end

def git_pull
  Dir.chdir HEAD_DIR do
    cmd "git pull"
  end
end

def hashes_since latest_hash
  logs = Dir.chdir HEAD_DIR do
    `git log --pretty=format:"%H %ae"`.split(/\n+/).map {|s| s.split }
  end

  warn "found #{logs.size} hashes" if $v

  latest_hash = logs[1].first if latest_hash.nil?
  build_hashes, latest_author = [], nil

  logs.each do |hash, author|
    break if hash == latest_hash
    build_hashes << hash unless latest_author == author
    latest_author = author
  end

  build_hashes.reverse
end

def build hash
  warn "building #{hash}" if $v
  dir = $i ? "incremental" : hash
  cmd "git clone -q -l #{HEAD_DIR} #{dir}" unless File.directory? dir
  Dir.chdir dir do
    cmd "git reset --hard #{hash}"
    system "rake -t spec &> ../#{hash}.log"
  end
ensure
  FileUtils::rm_rf hash # not dir, hash... incremental stays as a result
end

def submit hash
  warn "submitting #{hash}" if $v
  data = {
    :platform    => RUBY_PLATFORM,
    :incremental => $i,
    :hash        => hash,
    :log         => File.read("#{hash}.log"),
  }

  res = Net::HTTP.post_form(URI.parse(CGI_URI), "data" => YAML.dump(data))

  puts res.body
ensure
  # TODO: File.unlink hash
end

def write_file path, str
  File.open(path, 'w') do |f|
    f.write str
  end
end

def run
  build_dir_structures

  Dir.chdir BASE_DIR do
    loop do
      previous_hash = File.read(HASH_PATH).chomp rescue nil
      latest_hash = git_latest_hash

      need_build = (latest_hash &&
                    (previous_hash.nil? ||
                     previous_hash != latest_hash))

      if need_build then
        warn "needs building #{previous_hash} vs #{latest_hash}" if $v
        git_pull

        hashes = hashes_since(previous_hash)

        warn hashes.inspect if $v

        hashes.each do |hash|
          Dir.chdir BUILD_DIR do
            build hash
            submit hash
          end
          write_file HASH_PATH, hash
        end
      end

      sleep 120
    end
  end
end

run if $0 == __FILE__
