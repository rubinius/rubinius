# Wooter an html report :) --matt

# get info from my hacky comparison tools
info = eval `ruby -W0 #{File.dirname(__FILE__) + '/compare_info.rb'}`
info.each do |key, value|
  self.class.send(:define_method, key) { value }
end

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
total_classes = total_classes()
classes_done = complete_classes.length
classes_pct = classes_done.to_f / total_classes

total_class_methods = mri_class_methods_total
class_methods_left = class_methods_left()
class_methods_done = total_class_methods - class_methods_left
class_methods_pct = 1.0 - class_methods_left.to_f / total_class_methods

total_instance_methods = mri_instance_methods_total
instance_methods_left = instance_methods_left()
instance_methods_done = total_instance_methods - instance_methods_left
instance_methods_pct = 1.0 - instance_methods_left.to_f / total_instance_methods

# read and run template
require 'erb'
template = File.dirname(__FILE__) + '/templates/completeness.rhtml'
template = ERB.new( IO.read(template) )
template.run

