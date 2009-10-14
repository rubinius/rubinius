require 'yaml'

Run = Struct.new(:runtime, :times, :median)

yaml = ARGV.shift

files = YAML.load File.read(yaml)

runtimes = files[0]['runs'].keys.sort

file_names = files.map { |h| h['file'] }
runtime_data = Hash.new { |h,k| h[k] = [] }

files.each do |file|
  file['runs'].each do |name, data|
    runtime_data[name] << (data['median'])
  end
end

def theme
  blue = '#6886B4'
  yellow = '#FDD84E'
  green = '#72AE6E'
  red = '#D1695E'
  purple = '#8A6EAF'
  orange = '#EFAA43'
  white = 'white'
  colors = [yellow, blue, green, red, purple, orange, white]

  {
    :colors => @colors,
    :marker_color => 'white',
    :font_color => 'white',
    :background_colors => '#4a465a'
  }

end

require 'rubygems'
require 'gruff'

require "#{File.dirname(__FILE__)}/gruff_bar_better.rb"

g = Gruff::BetterBar.new("800x1000")
g.font = "/Users/evan/Library/Fonts/DejaVuSans.ttf"
g.title = "Tier 0"

runtimes.each do |runtime|
  g.data(runtime, runtime_data[runtime])
end

labels = {}
file_names.each_with_index { |n,i| labels[i] = n }
g.labels = labels
g.legend_font_size = 10
g.marker_font_size = 10
# g.left_margin = 0

g.sort = false

g.theme = theme()

output = ARGV.shift
puts "Writing to #{output}"
g.write output

exit

g = Gruff::SideBar.new("800x1000")
g.hide_title = true

runtimes = ["macruby", "ruby19", "jruby --server --fast", "rbx"]

runtimes.each do |runtime|
  g.data(runtime, runtime_data[runtime])
end

labels = {}
file_names.each_with_index { |n,i| labels[i] = n }
g.labels = labels
g.legend_font_size = 10
g.marker_font_size = 10
# g.left_margin = 0

g.sort = false


output = ARGV.shift
puts "Writing to #{output}"
g.write output

=begin

g = Gruff::SideBar.new
g.hide_title = true

data = []
runtimes.each do |runtime|
  g.data runtime, Float(runtime_data[runtime].inject { |a,o| a + o })
end

# data << Float(runtime_data[runtime].inject { |a,o| a + o }) / 1000
g.data "<null>", 0

# g.data("seconds", data)

labels = {}
runtimes.each_with_index { |n,i| labels[i] = n }
g.labels = { 0 => "seconds" }

p labels
p data

g.legend_font_size = 12
g.marker_font_size = 12

g.sort = false

output = ARGV.shift
puts "Writing to #{output}"
g.write output

=end
