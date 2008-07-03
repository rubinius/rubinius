#!/usr/bin/env ruby -w

# See README for instructions.

require 'cgi'
require 'yaml'
require 'fileutils'
require 'open3' # FIX: no clue why this is needed

# tweak this as needed:
BASE_DIR = File.expand_path "/tmp/ci"
SAFE_CAT = "/usr/local/bin/safecat"

# don't tweak this
DATA_DIR = File.join BASE_DIR, "data"
TEMP_DIR = File.join BASE_DIR, "data", "temp"

FileUtils::mkdir_p TEMP_DIR unless File.directory? TEMP_DIR

cgi = CGI.new

data = cgi.params["data"].first

cgi.out do
  begin
    YAML.load(data) # let it blow up here and now

    Open3.popen3("#{SAFE_CAT} #{TEMP_DIR} #{DATA_DIR}") do |inn, out, err|
      inn.write data
    end

    "<p>thanks #{$?.exitstatus}<p>"
  rescue => e
    "<p>error: #{e.class} #{e.message}"
  end
end
puts
