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
require File.dirname(__FILE__) + '/rspec_autotest'

class RubiniusAutotest < RspecAutotest
  def initialize # :nodoc:
    super
    @exceptions = %r%^\./(?:coverage|db|doc|log|public|script|vendor)%
  end

  def tests_for_file(filename)
    case filename
    when %r%^spec/primitives/.*_spec.rb%
      [filename]
    when %r%^shotgun/lib/primitives.rb%
      build_shotgun
      Dir['spec/primitives/*_spec.rb']
    else
      @output.puts "Dunno! #{filename}" if $TESTING
      []
    end
  end

  private
  def build_shotgun
    puts 'Building Shotgun'
    `rake build:shotgun` # will only output to screen if stderr
  end

  def compile_kernel
    # code here..
  end
end

RubiniusAutotest.run
