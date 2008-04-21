#!/usr/bin/env ruby -w

require 'yaml'
require 'fileutils'
require 'rubygems'
require 'tagz'

BASE_DIR = File.expand_path(ARGV.shift || "/tmp/ci")
HTML_DIR = File.expand_path(ARGV.shift || File.join(BASE_DIR, "html"))
DATA_DIR = File.join BASE_DIR, "data"
CI_DIR = File.join HTML_DIR, "ci"

GIT_URL = "http://git.rubini.us/?p=code;a=commit;h="

ABBR = {
  "x86-freebsd-6" => "f",
  "x86-linux"     => "l",
  "x86_64-linux"  => "l6",
  "x86-darwin-9"  => "d",
}

class HashHash < Hash
  def initialize
    super { |h,k| h[k] = HashHash.new }
  end
end

FileUtils.rm_rf CI_DIR
FileUtils.mkdir_p CI_DIR

all_data = HashHash.new

flat_data = Dir[File.join(DATA_DIR, "*")].select { |f| File.file? f }.map { |f|
  h = YAML.load(File.read(f))
  h[:id] = File.basename(f)
  h[:time] = File.mtime f

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
          [:id, :time, :incremental, :hash, :platform].each do |key|
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

    File.open File.join(HTML_DIR, "ci", "#{h[:id]}.html"), 'w' do |out|
      out.write html
    end
  end

  h
}

platforms = flat_data.map { |h| h[:platform] }.uniq
hashes    = flat_data.map { |h| h[:hash] }.uniq

flat_data.each do |data|
  all_data[data[:incremental]][data[:hash]][data[:platform]] = data
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
          th_ ABBR[platform]
        end

        th_ "&nbsp;"

        platforms.each do |platform|
          th_ ABBR[platform]
        end
      end

      hashes.each do |hash|
        tr_ do

          th_ do
            a_(hash[0..7], :href => "#{GIT_URL}#{hash}")
          end

          times = (all_data[true][hash].map { |_,run| run[:time] } +
                   all_data[false][hash].map { |_,run| run[:time] })

          th_ "#{times.max.strftime("%m-%d %H:%M")}"

          build_row all_data[true][hash], platforms
          td_ "&nbsp;"
          build_row all_data[false][hash], platforms
        end
      end
    end

    table_ do
      tr_ { th_ "Legend", :colspan => 2 }
      ABBR.each do |k,v|
        tr_ { th_ v; td_ k }
      end
    end
  end
end


File.open File.join(HTML_DIR, "index.html"), "w" do |f|
  f.puts html
end
