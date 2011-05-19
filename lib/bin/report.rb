# Shamelessly imported directly from defunkt's gist tool.

require 'open-uri'
require 'net/https'

GistSubmissionError = Class.new(StandardError)

module Gist
  extend self

  @@gist_url = 'https://gist.github.com/%s.txt'

  def read(gist_id)
    return help if gist_id == '-h' || gist_id.nil? || gist_id[/help/]
    open(@@gist_url % gist_id).read
  end

  def write(content, private_gist, name=nil)
    url = URI.parse('https://gist.github.com/gists')

    http = Net::HTTP.new(url.host, url.port)
    http.use_ssl = true
    http.verify_mode = OpenSSL::SSL::VERIFY_NONE

    req = Net::HTTP::Post.new(url.path)
    req.form_data = data(name, nil, content, private_gist)

    res = http.start {|h| h.request(req) }

    unless res.kind_of?(Net::HTTPRedirection)
      raise GistSubmissionError, %Q{Could not submit gist (#{res.code} #{res.message}). Make sure you've set github.user and github.token in your Git config, or submit the crash report located at '#{ENV['HOME']}/.rubinius_last_error' manually.}
    end

    copy res['Location']
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

  def data(name, ext, content, private_gist)
    return {
      'file_ext[gistfile1]'      => ext,
      'file_name[gistfile1]'     => name,
      'file_contents[gistfile1]' => content
    }.merge(private_gist ? { 'private' => 'on' } : {}).merge(auth)
  end

  def auth
    user  = `git config --global github.user`.strip
    token = `git config --global github.token`.strip

    user.empty? ? {} : { :login => user, :token => token }
  end
end

### Find the report and submit it.

unless path = ARGV.shift
  path = "#{ENV['HOME']}/.rubinius_last_error"
end

unless File.exists?(path)
  puts "Unable to find last error report."
  puts "Please specify the file path to the report."
  exit 1
end

data = File.read(path)

unless data.split("\n")[0].index("#rbxcrashreport")
  puts "This file does not appear to be a Rubinius crash report"
  exit 1
end

puts "Submitting the crash report..."

url = Gist.write(data, true, "crashreport.txt")

puts "Report submitted at: #{url}"
puts "Please open a Rubinius issue and provide this url."
