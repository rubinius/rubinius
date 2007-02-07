# (c) Copyright 2006-2007 Nick Sieger <nicksieger@gmail.com>
# 
# Permission is hereby granted, free of charge, to any person
# obtaining a copy of this software and associated documentation files
# (the "Software"), to deal in the Software without restriction,
# including without limitation the rights to use, copy, modify, merge,
# publish, distribute, sublicense, and/or sell copies of the Software,
# and to permit persons to whom the Software is furnished to do so,
# subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
# BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
# ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
# CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

require 'rubygems'
gem 'ZenTest'
require 'autotest'

class RspecAutotest < Autotest
  attr_accessor :spec_command
  def initialize # :nodoc:
    @spec_command = "spec --diff unified"
    super
    @exceptions = %r%^\./(?:coverage|doc)%
  end

  def tests_for_file(filename)
    case filename
    when /^lib\/.*\.rb$/ then
      impl = File.basename(filename).gsub('_', '_?').sub(/\.rb$/, '')
      @files.keys.select do |k|
        k =~ %r%^spec/.*#{impl}_spec\.rb$%
      end
    when %r%^spec/spec_helper.rb% then
      @files.keys.select do |f|
        f =~ %r%^spec/.*_spec\.rb$%
      end
    when /^spec\/.*_spec\.rb$/ then
      [filename]
    when /#{Regexp.quote(File.basename(__FILE__))}/
      # Don't respond to changes to this file
      []
    else
      @output.puts "Dunno! #{filename}" if $TESTING
      []
    end
  end

  def handle_results(results)
    failed = results.scan(/^\d+\)\n(?:\e\[\d*m)?(?:.*?Error in )?'([^\n]*)'(?: FAILED)?(?:\e\[\d*m)?\n(.*?)\n\n/m)
    @files_to_test = consolidate_failures failed
    unless @files_to_test.empty? then
      hook :red
    else
      hook :green
    end unless $TESTING
    @tainted = true unless @files_to_test.empty?
  end

  def consolidate_failures(failed)
    filters = Hash.new { |h,k| h[k] = [] }
    failed.each do |spec, failed_trace|
      @files.keys.select{|f| f =~ /spec\//}.each do |f|
        if failed_trace =~ Regexp.new(f)
          filters[f] << spec 
          break
        end
      end
    end
    return filters
  end

  def make_test_cmd(files_to_test)
    cmds = []
    full, partial = files_to_test.partition { |k,v| v.empty? }

    unless full.empty? then
      classes = full.map {|k,v| k}.flatten.join(' ')
      cmds << "#{spec_command} #{classes}"
    end

    partial.each do |klass, methods|
      methods.each { |meth| cmds << "#{spec_command} -s #{meth.inspect} #{klass}" }
    end

    return cmds.join('; ')
  end
end

