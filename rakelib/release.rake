RBX_VERSION = BUILD_CONFIG[:version]
require 'time'

namespace :release do
  desc "Print out information about the HEAD"
  task :info do
    if ENV['FILE']
      fd = File.open ENV['FILE'], "w"
    else
      fd = STDOUT
    end

    ver = ENV['VERSION'] || RBX_VERSION

    ol = `git log --pretty=oneline release-#{ver}..HEAD`.split("\n")
    count = ol.size

    now = Time.now
    date = now.strftime("%b %d, %Y")

    prev_date = `git log -n 1 --pretty=format:"%aD" release-#{ver}`

    prev = Time.parse prev_date.strip

    now_day = now.strftime("%j").to_i
    prev_day = prev.strftime("%j").to_i

    if now.year > prev.year
      days_since = (366 - prev_day) + now_day
    else
      days_since = now_day - prev_day
    end

    issues = []
    ol.each do |line|
      if /([Ff]ixes|[Cc]loses)\s#(\d+)/.match(line)
        issues << line
      end
    end

    fd.puts "---"
    fd.puts "layout: release"
    fd.puts "class: release"
    fd.puts "---"
    fd.puts
    fd.puts "## Overview"
    fd.puts
    fd.puts "The Rubinius team is proud to announce the availability of VERSION."
    fd.puts
    fd.puts "VERSION is a bugfix release for #{ver}. It contains things."
    fd.puts
    fd.puts

    fd.puts "## About"
    fd.puts
    fd.puts <<-ABOUT
Rubinius is a high performance environment for running code written in the
Ruby programming language. It provides a number of virtual machine features
found in other popular language runtimes such as generational garbage
collection, Just-In-Time compilation, and compatibility with existing C APIs.
    ABOUT

    fd.puts
    fd.puts "## Summary"
    fd.puts
    fd.puts "#### Stats\n\n"
    fd.puts "* **Release Date:** #{date}"
    fd.puts "* **Commits:** #{count}"
    fd.puts "* **Days since last release:** #{days_since}"
    fd.puts "* **Issues fixed:** #{issues.size} since #{ver}"

    authors = `git shortlog -s release-#{ver}..HEAD`.split("\n")
    authors.map! { |x| x.strip.split(/\s+/, 2) }
    authors.sort! { |a,b| a[0].to_i <=> b[0].to_i }

    fd.puts
    fd.puts "#### Authors\n\n"

    authors.reverse_each do |a|
      fd.puts "* #{a[1]}"
    end

    fd.puts
    fd.puts "##### Closed Issues\n\n"

    issues.each do |i|
      hash, message = i.split(" ", 2)

      message.gsub!(/([Ff]ixes|[Cc]loses)\s#(\d+)(\.)?/) { "Fixes ##{$2}" }
      message.gsub!("_", "\\_")

      fd.puts "* #{hash[0,7]} #{message}"
    end

    fd.puts
    fd.puts "##### Bug Fixes\n\n"
    ol.each do |line|
      unless issues.include?(line)
        message = line.split(" ", 2)[1]
        message.gsub!("_", "\\_")
        fd.puts "* #{message}"
      end
    end

    fd.close unless fd == STDOUT
  end

  desc "Create the tarball for the release"
  task :tar do
    stamp = Time.now.strftime("%Y%m%d")
    @rbx_tar = "rubinius-#{RBX_VERSION}-#{stamp}.tar.gz"
    sh "git archive --format=tar --prefix=rubinius-#{RBX_VERSION}/ HEAD | gzip > #{@rbx_tar}"
  end

  desc "Tag HEAD with the current release"
  task :tag do
    sh "git tag release-#{RBX_VERSION} HEAD"
  end

  desc "Create a release"
  task :all do
    # A simple sanity check, so that RBX_VERSION is updated first.
    unless RBX_VERSION == ENV['VERSION']
      p [RBX_VERSION, ENV['VERSION']]
      raise "Please set VERSION to the same as RBX_VERSION to be released"
    end

    STDOUT.sync = true
    print "Did you update @config_version in configure? [y/n]"
    unless STDIN.gets.strip == "y"
      puts "Well go change it then!"
      exit
    end

    puts "* Making tar.gz..."
    Rake::Task['release:tar'].invoke

    # Do this separately, so that the testing can fail and we leave the directory
    # where it is.
    if osx = ENV['OSX']
      puts "* Cleaning and building OS X package..."
      sh "cd release-test/rubinius-#{RBX_VERSION}; rake -q clean; RELEASE=1 ./configure --prefix=/usr/local/rubinius/#{RBX_VERSION}; rake -q package:osx && mv rubinius-#{RBX_VERSION}.pkg ../.."
      pref = "rubinius-#{RBX_VERSION}-#{osx}"
      sh "mv rubinius-#{RBX_VERSION}.pkg #{pref}.pkg"
      sh "zip -r #{pref}.pkg.zip #{pref}.pkg"
    end
    sh "rm -rf release-test"

    # Comes last, means everything went fine.
    puts "* Tagging..."
    Rake::Task['release:tag'].invoke

    puts "* DONE!"
  end

  desc "Create an OS X package from a release tarball"
  task :osx do
    unless file = ENV['FILE']
      raise "Please set FILE to the tar.gz for the release"
    end

    unless osx = ENV['OSX']
      raise "Please set OSX to the version of OS X this is for"
    end

    unless ver = ENV['VERSION']
      ver = RBX_VERSION
    end

    puts "* Building OS X package..."
    sh "rm -rf osx-build; mkdir osx-build"

    Dir.chdir "osx-build" do
      sh "tar zxf ../#{file}"

      unless File.directory?("rubinius-#{ver}")
        raise "tar file didn't contain the proper release directory"
      end

      sh "cd rubinius-#{ver}; RELEASE=1 ./configure --prefix=/usr/local/rubinius/#{ver}; rake -q package:osx && mv rubinius-#{ver}.pkg ../.."
    end

    pref = "rubinius-#{ver}-#{osx}"
    sh "mv rubinius-#{ver}.pkg #{pref}.pkg"
    sh "zip -r #{pref}.pkg.zip #{pref}.pkg"
  end
end
