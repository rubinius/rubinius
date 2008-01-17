# case.rb - classes which abuse Ruby's case matching for pattern matching
#
# Copyright 2008  MenTaLguY <mental@rydia.net>
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

module Case

# Like regular Struct, except that Struct#=== performs a structural match
class Struct < ::Struct
  def ===(other)
    return false unless self.class == other.class
    zip(other) { |a, b| return false unless a === b }
    true
  end
end

# Pattern-matching disjunction
class Any
  class << self
    alias_method :[], :new
  end

  def initialize(*options)
    @options = options
  end

  def options
    @options.dup
  end

  def ===(obj)
    @options.any? { |e| e === obj }
  end
end

class Tuple < ::Tuple
  def ===(other)
    return false unless ::Tuple === other and size == other.size
    to_a.zip(other) { |a, b| return false unless a === b }
    true 
  end
end

class Array < ::Array
  def ===(other)
    return false unless ::Array === other and size == other.size
    zip(other) { |a, b| return false unless a === b }
    true
  end
end

class Predicate
  def initialize(&predicate)
    @predicate = predicate
  end

  def ===(other)
    @predicate.call other
  end
end

# Shorthand for Case::Array.[]
def self.[](*args)
  Array[*args]
end

end
