#!/usr/bin/env ruby
#--
# $Idaemons: /home/cvs/rb/generator.rb,v 1.8 2001/10/03 08:54:32 knu Exp $
# $RoughId: generator.rb,v 1.10 2003/10/14 19:36:58 knu Exp $
# $Id: generator.rb 15954 2008-04-10 10:52:50Z knu $
#++
#
# = generator.rb: convert an internal iterator to an external one
#
# Copyright (c) 2001,2003 Akinori MUSHA <knu@iDaemons.org>
#
# All rights reserved.  You can redistribute and/or modify it under
# the same terms as Ruby.
#
# == Overview
#
# This library provides the Generator class, which converts an
# internal iterator (i.e. an Enumerable object) to an external
# iterator.  In that form, you can roll many iterators independently.
#
# The SyncEnumerator class, which is implemented using Generator,
# makes it easy to roll many Enumerable objects synchronously.
#
# See the respective classes for examples of usage.


#
# Generator converts an internal iterator (i.e. an Enumerable object)
# to an external iterator.
#
# Note that it is not very fast since it is implemented using
# continuations, which are currently slow.
#
# == Example
#
#   require 'generator'
#
#   # Generator from an Enumerable object
#   g = Generator.new(['A', 'B', 'C', 'Z'])
#
#   while g.next?
#     puts g.next
#   end
#
#   # Generator from a block
#   g = Generator.new { |g|
#     for i in 'A'..'C'
#       g.yield i
#     end
#
#     g.yield 'Z'
#   }
#
#   # The same result as above
#   while g.next?
#     puts g.next
#   end
#   
class Generator
  include Enumerable

  # Creates a new generator either from an Enumerable object or from a
  # block.
  #
  # In the former, block is ignored even if given.
  #
  # In the latter, the given block is called with the generator
  # itself, and expected to call the +yield+ method for each element.
  def initialize(enum=nil, &block)
    if enum
      @block = proc do |g|
        enum.each { |x| g.yield x }
      end
    else
      @block = block
    end

    @index = 0
    @queue = []

    @done = false

    @fiber = Rubinius::Fiber.new do
      @block.call(self)
      @done = true
      @fiber = nil
    end

    self
  end

  # Yields an element to the generator.
  def yield(value)
    # Running inside @fiber
    @queue << value
    Rubinius::Fiber.yield

    self
  end

  # Returns true if the generator has reached the end.
  def end?
    return true if @done
    return false unless @queue.empty?

    # Turn the loop over and see if we hit the end
    @fiber.resume

    return @done
  end

  # Returns true if the generator has not reached the end yet.
  def next?
    !end?
  end

  # Returns the current index (position) counting from zero.
  def index
    @index
  end

  # Returns the current index (position) counting from zero.
  def pos
    @index
  end

  # Returns the element at the current position and moves forward.
  def next
    raise EOFError, "end of iteration hit" if end?

    @fiber.resume if @queue.empty?

    # We've driven the fiber ahead and least once now, so we should
    # have values if there are values to be had

    unless @queue.empty?
      @index += 1
      return @queue.shift
    end

    raise EOFError, "end of iteration hit"
  end

  # Returns the element at the current position.
  def current
    if @queue.empty?
      raise EOFError, "no more elements available"
    end

    @queue.first
  end

  # Rewinds the generator.
  def rewind
    initialize(nil, &@block) if @index.nonzero?

    self
  end

  # Rewinds the generator and enumerates the elements.
  def each
    rewind

    until end?
      yield self.next
    end

    self
  end
end

#
# SyncEnumerator creates an Enumerable object from multiple Enumerable
# objects and enumerates them synchronously.
#
# == Example
#
#   require 'generator'
#
#   s = SyncEnumerator.new([1,2,3], ['a', 'b', 'c'])
#
#   # Yields [1, 'a'], [2, 'b'], and [3,'c']
#   s.each { |row| puts row.join(', ') }
#
class SyncEnumerator
  include Enumerable

  # Creates a new SyncEnumerator which enumerates rows of given
  # Enumerable objects.
  def initialize(*enums)
    @gens = enums.map { |e| Generator.new(e) }
  end

  # Returns the number of enumerated Enumerable objects, i.e. the size
  # of each row.
  def size
    @gens.size
  end

  # Returns the number of enumerated Enumerable objects, i.e. the size
  # of each row.
  def length
    @gens.length
  end

  # Returns true if the given nth Enumerable object has reached the
  # end.  If no argument is given, returns true if any of the
  # Enumerable objects has reached the end.
  def end?(i = nil)
    if i.nil?
      @gens.detect { |g| g.end? } ? true : false
    else
      @gens[i].end?
    end
  end

  # Enumerates rows of the Enumerable objects.
  def each
    @gens.each { |g| g.rewind }

    while true
      count = 0

      ret = @gens.map { |g|
        if g.end?
          count += 1
          nil
        else
          g.next
        end
      }

      break if count == @gens.size

      yield ret
    end

    self
  end
end

