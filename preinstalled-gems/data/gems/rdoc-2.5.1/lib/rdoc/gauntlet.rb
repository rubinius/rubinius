require 'rdoc/rdoc'
require 'gauntlet'
require 'fileutils'

##
# Allows for testing of RDoc against every gem

class RDoc::Gauntlet < Gauntlet

  def run name
    next if self.data[name]

    ri_dir = File.expand_path "~/.gauntlet/data/ri/#{name}"
    FileUtils.rm_rf ri_dir if File.exist? ri_dir

    yaml = File.read 'gemspec'
    spec = Gem::Specification.from_yaml yaml

    args = %W[--ri --op #{ri_dir}]
    args.push(*spec.rdoc_options)
    args << spec.require_paths
    args << spec.extra_rdoc_files
    args = args.flatten.map { |a| a.to_s }
    args.delete '--quiet'

    puts "#{name} - rdoc #{args.join ' '}"

    self.dirty = true
    r = RDoc::RDoc.new

    begin
      r.document args
      self.data[name] = true
      puts 'passed'
    rescue StandardError, RDoc::Error => e
      puts "failed - (#{e.class}) #{e.message}"
      self.data[name] = false
    end
  rescue Gem::Exception
    puts "bad gem #{name}"
  ensure
    puts
  end

end

RDoc::Gauntlet.new.run_the_gauntlet if $0 == __FILE__

