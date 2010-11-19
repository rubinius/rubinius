#!/usr/bin/env ruby

# Options handling
require 'optparse'
require 'ostruct'

now            = Time.now
NOW            = "#{now.year}-#{'%02d' % now.month}-#{'%02d' % now.day}-" +
                 "#{'%02d' % now.hour}-#{'%02d' % now.min}-#{'%02d' % now.sec}"

OPTIONS = OpenStruct.new( :size   => 1024,
                          :path   => './' )

options = OptionParser.new do |opts|
  opts.on('-c', '--chart [FILENAME]', 'Output passing test times to SVG file at --path') do |c|
    OPTIONS.chart = c || "#{NOW}-contributors.svg"
  end

  opts.on('-p', '--path PATH', 'Path prefix for --chart, --diff-chart, and --file') do |p|
    OPTIONS.path = p
  end

  opts.on('-s', '--size PIXELS', 'Size, in pixels, of --chart and/or --diff-chart') do |s|
    OPTIONS.size = s
  end
end

options.parse!

if OPTIONS.help then
  puts options.summarize
  exit
end

class Contributor
  include Comparable 

  attr_accessor :name,:lines

  def initialize(name,lines)
    @name = name
    @lines = lines
  end

  def <=>(other) 
    @lines <=> other.lines
  end 
end

contributors = []

while l = gets
  lines,name = l.match(/^\s*(\d+) (.+)/)[1,2]
  contributors << Contributor.new(name,lines.to_i)
end

contributors.sort!
contributors.reverse!

contributors.each do |c|
  puts "#{c.name}, #{c.lines}"
end

if OPTIONS.chart then
  require 'rubygems'
  require 'scruffy'

  graph = Scruffy::Graph.new
  
  graph << Scruffy::Layers::Line.new( :title  => 'Rubinius LOC by contributor',
                                      :points => contributors.collect { |c| c.lines } )

  graph.point_markers = contributors.collect { |c| c.name }

  puts "Rendering chart to #{OPTIONS.chart}"

  graph.render( :width => OPTIONS.size, :to => OPTIONS.path + OPTIONS.chart )
end
