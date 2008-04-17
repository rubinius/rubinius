# actor.rb - implementation of the actor model
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

require 'mailbox'

class Actor
  class DeadActorError < RuntimeError
    attr_reader :actor
    attr_reader :reason
    def initialize(actor, reason)
      @actor = actor
      @reason = reason
    end
  end

  class << self
    alias_method :private_new, :new
    private :private_new

    @@registered_lock = Channel.new
    @@registered = {}
    @@registered_lock << nil
  
    # Get the currently executing Actor
    def current
      Thread.current[:__current_actor__] ||= private_new(current_mailbox)
    end

    def current_mailbox
      Thread.current[:__current_mailbox__] ||= Mailbox.new
    end
    private :current_mailbox

    def check_interrupt
      current_mailbox.check_interrupt
      self
    end

    # Receive a message from the current Actor's mailbox
    def receive(&block)
      current_mailbox.receive(&block)
    end

    # Spawn a new Actor that will run in its own thread
    def spawn(*args, &block)
      raise ArgumentError, "no block given" unless block

      channel = Channel.new
      Thread.new do
        channel << current

        begin
          block.call *args
          current._shutdown(:exit_clean)
        rescue => e
          current._shutdown(e)
        end
      end
      channel.receive
    end
    alias_method :new, :spawn

    # Spawn an Actor and immediately link it to the current one
    def spawn_link(*args, &block)
      current = Actor.current
      link_complete = Channel.new
      spawn do
        begin
          Actor.link(current)
        ensure
          link_complete << Actor.current
        end
        block.call *args
      end
      link_complete.receive
    end
    
    # Link the current Actor to another one
    def link(actor)
      current = Actor.current
      actor.notify_link current
      current.notify_link actor
      self
    end
    
    # Unlink the current Actor from another one
    def unlink(actor)
      current = Actor.current
      current.notify_unlink actor
      actor.notify_unlink current
      self
    end

    # Lookup a locally named service
    def lookup(name)
      raise ArgumentError, "name must be a symbol" unless Symbol === name
      @@registered_lock.receive
      begin
        @@registered[name]
      ensure
        @@registered_lock << nil
      end
    end

    # Register an Actor locally as a named service
    def register(name, actor)
      raise ArgumentError, "name must be a symbol" unless Symbol === name
      unless actor.is_a?(Actor)
        raise ArgumentError, "only actors may be registered"
      end

      @@registered_lock.receive
      begin
        @@registered[name] = actor
      ensure
        @@registered_lock << nil
      end
    end

    # Unregister the named service
    def unregister(name)
      raise ArgumentError, "name must be a symbol" unless Symbol === name
      @@registered_lock.receive
      begin
        @@registered.delete(name)
      ensure
        @@registered_lock << nil
      end
    end
  end

  def initialize(mailbox)
    @mailbox = mailbox
    @lock = Channel.new
    @links = []
    @alive = true
    @exit_reason = nil
    @trap_exit = false
    @lock << nil
  end

  def send(value)
    @mailbox.send value
    self
  end
  alias_method :<<, :send
 
  # Notify this actor that it's now linked to the given one
  def notify_link(actor)
    # Ignore circular links
    return true if actor == self
    
    @lock.receive
    begin
      raise DeadActorError.new(self, @exit_reason) unless @alive

      # Ignore duplicate links
      return true if @links.include? actor
    
      @links << actor
    ensure
      @lock << nil
    end
    self
  end
  
  # Notify this actor that it's now unlinked from the given one
  def notify_unlink(actor)
    @lock.receive
    begin
      @links.delete(actor)
    ensure
      @lock << nil
    end
    self
  end
  
  # Notify this actor that one of the Actors it's linked to has exited
  def notify_exited(actor, reason)
    @lock.receive
    begin
      @links.delete(actor)
      if @trap_exit
        send [:exit, actor, reason]
      elsif reason != :exit_clean
        @mailbox.interrupt DeadActorError.new(actor, reason)
      end
    ensure
      @lock << nil
    end
    self
  end

  def _shutdown(reason)
    links = nil
    @lock.receive
    begin
      @alive = false
      @exit_reason = reason
      links, @links = @links, []
    ensure
      @lock << nil
    end
    links.each do |actor|
      begin
        actor.notify_exited(self, reason)
      rescue Exception
      end
    end
  end
  
  # Actors trapping exit do not die when an error occurs in an Actor they
  # are linked to.  Instead the exit message is sent to their regular
  # mailbox in the form [:exit, actor, reason].  This allows certain
  # Actors to supervise sets of others and restart them in the event
  # of an error.
  
  def trap_exit=(value)
    @lock.receive
    begin
      @trap_exit = !!value
    ensure
      @lock << nil
    end
  end
  
  # Is the Actor trapping exit?
  def trap_exit?
    @lock.receive
    begin
      @trap_exit
    ensure
      @lock << nil
    end
  end
end
