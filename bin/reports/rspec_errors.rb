results = Marshal.load(`ruby #{File.dirname(__FILE__) + '/rspec_error_info.rb'}`)

require 'cgi'
def h(html)
  CGI.escapeHTML(html)
end

def percent_bar(pct)
  width = (pct * 200).round
  rest = 200 - width
  percent = ((pct * 100.0).round).to_s + '%'
  <<HTML
<table style="height: 25px; width: 200px" cellpadding="0" cellspacing="0">
  <tr>
    <td style="width: #{width}px; text-align: right; color: #000000; background: #00f000">
      #{pct >= 0.5 ? percent + '&nbsp;&nbsp;' : ''}</td>
    <td style="width: #{rest}px; text-align: left; color: #ffffff; background: #e00000">
      #{pct < 0.5 ? '&nbsp;&nbsp;' + percent : ''}</td>
  </tr>
</table>
HTML
end

title = 'Make RSpec work on Rubinius Report'
generated = Time.now

all_files = results
working_files = results.select {|file, error| error.empty? }
nonworking_files = all_files - working_files

all_files_len = all_files.length
working_files_len = working_files.length

working_files_pct = working_files_len.to_f / all_files_len

# read and run template
require 'erb'
template = File.dirname(__FILE__) + '/templates/rspec_errors.rhtml'
template = ERB.new( IO.read(template) )
template.run

