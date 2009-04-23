# mailbox.rb - mailbox class supporting actor implementation
#
# Copyright 2007  MenTaLguY <mental@rydia.net>
#
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without 
# modification, are permitted provided that the following conditions are met:
# 
# * Redistributions of source code must retain the above copyright notice,
#   thi slist of conditions and the following disclaimer.
# * Redistributions in binary form must reproduce the above copyright notice
#   this list of conditions and the following disclaimer in the documentatio
#   and/or other materials provided with the distribution.
# * Neither the name of the Evan Phoenix nor the names of its contributors 
#   may be used to endorse or promote products derived from this software 
#   without specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

class Mailbox
  def initialize
    @channel = Rubinius::Channel.new
    @skipped = []
  end

  def send(value)
    @channel.send value
    self
  end
  alias_method :<<, :send

  def receive
    if block_given?
      filter = Filter.new
      yield filter

      value = nil
      action = nil

      found_at = nil
      @skipped.each_with_index do |obj, index|
        action = filter.action_for obj
        if action
          value = obj
          found_at = index
          break
        end
      end
      @skipped.delete_at found_at if found_at

      until action
        value = @channel.receive
        action = filter.action_for value
        @skipped.push value unless action
      end

      action.call value
    else
      unless @skipped.empty?
        @skipped.shift
      else
        @channel.receive
      end
    end
  end

  class Filter
    def initialize
      @pairs = []
    end

    def when(pattern, &action)
      raise ArgumentError, "no block given" unless action
      @pairs.push [pattern, action]
    end

    def action_for(value)
      pair = @pairs.find { |pattern, action| pattern === value }
      pair ? pair[1] : nil
    end
  end
end
