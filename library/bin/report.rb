# Shamelessly imported directly from defunkt's gist tool.

require 'open-uri'
require 'net/https'
require 'json'

GistSubmissionError = Class.new(StandardError)

module Gist
  extend self

  def write(content, private_gist, name, report_file)
    url = URI.parse('https://api.github.com/gists')

    http = Net::HTTP.new(url.host, url.port)
    http.use_ssl = true
    http.verify_mode = OpenSSL::SSL::VERIFY_NONE

    req = Net::HTTP::Post.new(url.path)
    req.body = data(name, content, private_gist).to_json

    token = gh_personal_token
    unless token.empty?
      req.basic_auth token, 'x-oauth-basic'
    end

    res = http.start {|h| h.request(req) }

    unless res.code == '201' # Created
      raise GistSubmissionError, %Q{Could not submit gist (#{res.code} #{res.message}). Make sure you've set github.user and github.token in your Git config, or submit the crash report located at '#{report_file}' manually.}
    end

    gist = JSON.parse res.body
    copy gist['html_url']
  end

  def help
    "USE:\n  " + File.read(__FILE__).match(/USE:(.+?)=end/m)[1].strip
  end

private
  def copy(content)
    case RUBY_PLATFORM
    when /darwin/
      return content if `which pbcopy`.strip == ''
      IO.popen('pbcopy', 'r+') { |clip| clip.puts content }
    when /linux/
      return content if `which xclip  2> /dev/null`.strip == ''
      IO.popen('xclip', 'r+') { |clip| clip.puts content }
    end

    content
  end

  def data(file_name, content, private_gist)
    return {
      'public' => !private_gist,
      'files' => {
        file_name => { 'content' => content }
      }
    }
  end

  # For GitHub API v3 you can create your
  # own personal access tokens to allow scripts
  # like this to authenticate with your user.
  def gh_personal_token
    `git config github.token`.strip
  end
end

### Find the report and submit it.

unless report_file = ARGV.shift
  # We search both locations that may contain crash reports and use the most
  # recent one we find. This enables submitting crash reports for older
  # versions of Rubinius.
  files = []

  path = "#{ENV['HOME']}/.rbx"
  if File.directory? path
    files.concat Dir["#{path}/rubinius_last_error*"]
  end

  files.concat Dir["#{ENV['HOME']}/.rubinius_last_error*"]

  report_file = files.sort { |a, b| File.mtime(a) <=> File.mtime(b) }.last
end

unless File.exist?(report_file)
  puts "Unable to find last error report."
  puts "Please specify the file path to the report."
  exit 1
end

data = File.read(report_file)

unless data.split("\n")[0].index("#rbxcrashreport")
  puts "This file does not appear to be a Rubinius crash report"
  exit 1
end

puts "Submitting the crash report..."

url = Gist.write(data, true, "crashreport.txt", report_file)

puts "Report submitted at: #{url}"
puts "Please open a Rubinius issue and provide this url."
