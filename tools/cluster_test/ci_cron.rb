#!/usr/bin/env ruby -w

require 'yaml'
require 'fileutils'
require 'rubygems'
require 'tagz'

BASE_DIR  = File.expand_path(ARGV.shift || "/tmp/ci")
HTML_DIR  = File.expand_path(ARGV.shift || File.join(BASE_DIR, "html"))

CI_DIR    = File.join HTML_DIR, "ci"
DATA_DIR  = File.join BASE_DIR, "data"
MRI_TRUNK = 'http://svn.ruby-lang.org/repos/ruby/trunk'
RBX_REPO  = 'git://git.rubini.us/code'
SPEC_REPO = 'git://github.com/rubyspec/rubyspec.git'
TEMP_DIR  = File.join BASE_DIR, "tmp"
TRIG_DIR  = File.join HTML_DIR, "trigger"

GIT_URL = "http://git.rubini.us/?p=code;a=commit;h="

STALE   = 14
DISPLAY = 50

class HashHash < Hash
  def initialize
    super { |h,k| h[k] = HashHash.new }
  end
end

############################################################
# Main Runner:

def _run
  FileUtils.rm_rf CI_DIR
  FileUtils.mkdir_p [CI_DIR, TEMP_DIR, TRIG_DIR]
  Dir.chdir BASE_DIR

  archive_stale_data STALE

  all_data  = HashHash.new
  flat_data = process_individual_results
  platforms = flat_data.map { |h| h[:platform] }.uniq
  hashes    = flat_data.map { |h| h[:hash]     }.uniq

  flat_data.each do |data|
    all_data[data[:incremental]][data[:hash]][data[:platform]] = data
  end

  hash_times = hashes.map { |hash|
    # incrementals and fall back to fulls if the sky is falling
    (all_data[true ][hash].map { |_,run| run[:submitted] }.max ||
     all_data[false][hash].map { |_,run| run[:submitted] }.max)
  }

  hashes = Hash[*hashes.zip(hash_times).flatten]

  write_index platforms, hashes, all_data

  update_triggers
end

############################################################
# Helper Methods

def abbreviate_platform(arch)
  plat = Gem::Platform.new(arch)
  cpu, os, _ = plat.to_a
  o, c = os[0..0], cpu[0..0] rescue "?"

  c = "i" if c == "x"
  x = "6" if cpu =~ /64/

  "#{o}#{c}#{x}"
end

def archive_stale_data days
  seconds_per_day = 86400

  results = Hash.new { |h,k| h[k] = [] }

  Dir["data/*"].each do |f|
    next unless File.file? f
    h = YAML.load(File.read(f))
    log = h.delete :log
    mtime = File.mtime f

    next if (Time.now - mtime) < days * seconds_per_day

    h[:submitted] = mtime
    h[:time]      = log[/^Finished in (.*) seconds/, 1].to_f
    h[:result]    = log[/^\d+ files.*/]

    results[h[:submitted].strftime("%Y-%m")] << h
    File.unlink f
  end

  results.each do |date, data|
    next if data.empty?
    path = "html/index.#{date}.yaml"
    if File.exist? path then
      old_data = YAML.load_file path
      data = old_data + data
    end
    File.open path, 'w' do |f|
      YAML.dump data, f
    end
  end
end

def build_row runs, platforms
  platforms.each do |platform|
    run = runs[platform]
    if run.has_key? :log then
      status = run[:status]
      case status
      when :passed then
        td_ "&nbsp;", :class => :p
      when :died then
        td_ :class => :d do
          a_("doa", :href => "ci/#{run[:id]}.html")
        end
      when String then
        td_ :class => :f do
          a_(status, :href => "ci/#{run[:id]}.html")
        end
      end
    else
      td_ "&nbsp;", :class => :m
    end
  end
end

def git_diff repo, n
  old = YAML.load_file(File.join(TRIG_DIR, "#{n}.yaml"))[:revision] rescue nil
  new = `git ls-remote -h #{repo} refs/heads/master`.split.first
  return old, new
end

def process_individual_results
  Dir[File.join(DATA_DIR, "*")].select { |f| File.file? f }.map { |f|
    h = YAML.load(File.read(f))
    h[:id] = File.basename(f)
    h[:submitted] = File.mtime f

    log = h[:log]
    h[:status] = if log =~ /(\d+) failures?, (\d+) errors?$/ then
                   failures, errors = $1.to_i, $2.to_i
                   if failures + errors == 0 then
                     :passed
                   else
                     "#{failures}/#{errors}"
                   end
                 else
                   :died
                 end

    if h[:status] != :passed then
      html = Tagz do
        head_ do
          title_ "Build Result"
          style_ do
            tagz << "pre { white-space: pre-wrap;  }"
            tagz << ".x  { font-size: 0.7em; color: #999; }"
            tagz << ".b  { color: #009; }"
            tagz << "th  { text-align: right }"
          end
        end

        body_ do
          h1_ "Build Result"

          table_ do
            [:id, :submitted, :incremental, :hash, :platform].each do |key|
              tr_ do
                th_ key
                td_ h[key]
              end
            end
          end

          log = h[:log]
          log.gsub!(/^(\*\*.*not_needed.*)/, '<span class=x>\1</span>')
          log.gsub!(/^(\*\*.*)/, '<span class=x>\1</span>')
          log.gsub!(/^.*\.{10}.*/) { |l| l.scan(/.{78}/).join("\n") + "\n" }
          log.gsub!(/<.span>\n/, "\n</span>")
          pre_ log
        end
      end

      path = File.join(HTML_DIR, "ci", "#{h[:id]}.html")
      File.open path, 'w' do |out|
        out.write html
      end
      t = File.mtime f
      File.utime(t, t, path)
    end

    h
  }
end

def svn_diff repo, dir
  Dir.chdir TEMP_DIR do
    system "svn co -q #{repo} #{dir}" unless File.directory? dir

    Dir.chdir dir do
      old = `svnversion .`.chomp
      system "svn up -q"
      new = `svnversion .`.chomp
      return old.to_i, new.to_i
    end
  end
end

def update_mri_trigger
  old, new = svn_diff MRI_TRUNK, "mri_trunk"

  update_trigger "mri", :revision => new if old != new
end

def update_rbx_trigger
  old, new = git_diff RBX_REPO, "rbx"

  update_trigger "rbx", :revision => new if old != new || old.nil?
end

def update_rubyspec_trigger
  old, new = git_diff SPEC_REPO, "spec"

  update_trigger "spec", :revision => new if old != new || old.nil?
end

def update_trigger name, data = nil
  Dir.chdir HTML_DIR do
    File.open "trigger/#{name}.yaml", "w" do |f|
      YAML.dump data, f
    end
  end
end

def update_triggers
  update_rbx_trigger
  update_rubyspec_trigger
  update_mri_trigger
end

def write_index platforms, hashes, all_data
  html = Tagz do
    head_ do
      title_ "Rubinius CI Dashboard"
      style_ do
        tagz << "body { font-family: Optima, Times }"
        tagz << "a    { color: black; }"
        tagz << "h1,h3{ color: #339; text-align: center }"

        tagz << "table.data { border-spacing: 2 0 }"
        tagz << ".data tr   { border-bottom: 1px solid black }"
        tagz << ".data th   { text-align: right; font-family: monospace }"
        tagz << ".data td   { text-align: center; }"

        tagz << ".data #first th { text-align: center }"

        tagz << "td.p { background-color: #cfc }"
        tagz << "td.f { background-color: #fcc }"
        tagz << "td.d { background-color: #fcc }"
        tagz << "td.m { background-color: #ccc }"
      end
    end

    body_ do
      h1_ "Rubinius CI Dashboard"
      h3_ Time.now.strftime("%m-%d %H:%M")

      table_ :id => "data" do
        tr_ :id => "first" do
          th_ "&nbsp;"
          th_ "&nbsp;"
          th_ "Incrs", :colspan => platforms.size
          th_ "&nbsp;"
          th_ "Fulls", :colspan => platforms.size
        end

        tr_ :id => "first" do
          th_ "Hash"
          th_ "Time"

          platforms.each do |platform|
            th_ abbreviate_platform(platform)
          end

          th_ "&nbsp;"

          platforms.each do |platform|
            th_ abbreviate_platform(platform)
          end
        end

        hashes.sort_by {|_,t| -t.to_i }.first(DISPLAY).each do |hash, time|
          tr_ do
            th_ do
              a_(hash[0..7], :href => "#{GIT_URL}#{hash}")
            end

            th_ "#{time.strftime("%m-%d %H:%M")}"

            build_row all_data[true][hash], platforms
            td_ "&nbsp;"
            build_row all_data[false][hash], platforms
          end
        end
      end

      table_ do
        tr_ { th_ "Legend", :colspan => 2 }
        platforms.each do |plat|
          tr_ { th_ abbreviate_platform(plat); td_ plat }
        end
      end
    end
  end

  File.open File.join(HTML_DIR, "index.html"), "w" do |f|
    f.puts html
  end
end

# Done
############################################################

_run if $0 == __FILE__
