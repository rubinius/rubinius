# -*- ruby -*-
require 'find'

task :todo do
  excludes = Hash.new 0
  Find.find "spec/tags" do |path|
    next unless File.file? path
    next unless File.size? path
    dir = File.dirname(path).sub(/^\../, '')

    excludes[dir] += File.readlines(path).size
  end

  total = 0
  excludes.sort_by { |_, v| -v }.each do |dir, count|
    puts "%3d: %s" % [count, dir]
    total += count
  end
  puts "%3d: TOTAL" % total
end
