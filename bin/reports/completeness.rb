# Wooter an html report :) --matt

# get info from my refactored comparison tools
info = Marshal.load(`ruby -W0 #{File.dirname(__FILE__) + '/compare.rb'}`)

# helpers (probably need to make this whole thing way less messy)
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

# instance vars... (also .. less messy) lol
title = 'Rubinius Completeness Report'
generated = Time.now

unimplemented_constants = info[:unimplemented_constants]
incomplete_constants = info[:incomplete_constants]
completed_constants = info[:completed_constants]
constants_length = info[:constants_length]
completed_constants_length = info[:completed_constants_length]
methods_length = info[:methods_length]
completed_methods_length = info[:completed_methods_length]
instance_methods_length = info[:instance_methods_length]
completed_instance_methods_length = info[:completed_instance_methods_length]

constants_pct = completed_constants_length.to_f / constants_length
methods_pct = completed_methods_length.to_f / methods_length
instance_methods_pct = completed_instance_methods_length.to_f / instance_methods_length

# read and run template
require 'erb'
template = File.dirname(__FILE__) + '/templates/completeness.rhtml'
template = ERB.new( IO.read(template) )
template.run

