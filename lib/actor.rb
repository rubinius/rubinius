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
  class << self
    alias_method :private_new, :new
    private :private_new

    @@registered = Channel.new
    @@registered << {}
  
    # Get the currently executing Actor
    def current
      Thread.current[:__current_actor__] ||= private_new(current_mailbox)
    end

    def current_mailbox
      Thread.current[:__current_mailbox__] ||= Mailbox.new
    end
    private :current_mailbox

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
          current.handle_error(:exit_clean)
        rescue => e
          current.handle_error(e)
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
      registered = @@registered.receive
      begin
        registered[name]
      ensure
        @@registered << registered
      end
    end

    # Register an Actor locally as a named service
    def register(name, actor)
      unless actor.is_a?(Actor)
        raise ArgumentError, "only actors may be registered"
      end

      registered = @@registered.receive
      begin
        registered[name] = actor
      ensure
        @@registered << registered
      end
    end

    # Unregister the named service
    def unregister(name)
      registered = @@registered.receive
      begin
        registered.delete(name)
      ensure
        @@registered << registered
      end
    end
  end

  def initialize(mailbox)
    @mailbox = mailbox
    @lock = Channel.new
    @links = []
    @trap_exit = true
    @lock << nil
  end

  def send(value)
    @mailbox.send value
    self
  end
  alias_method :<<, :send
 
  # Notify this actor that it's now linked to the given one
  def notify_link(actor)
    raise ArgumentError, "can only link to Actors" unless actor.is_a? Actor
    
    # Ignore circular links
    return true if actor == self
    
    @lock.receive
    begin
      # Ignore duplicate links
      return true if @links.include? actor
    
      @links << actor
    ensure
      @lock << nil
    end
    true
  end
  
  # Notify this actor that it's now unlinked from the given one
  def notify_unlink(actor)
    @lock.receive
    begin
      @links.delete(actor)
    ensure
      @lock << nil
    end
    true
  end
  
  # Notify this actor that one of the Actors it's linked to has exited
  def notify_exited(actor, reason)
    event = [:exit, actor, reason]

    if @trap_exit
      send event
    elsif ex
      # Need to raise the error in the context of the actor thread itself,
      # or else kill the actor thread and notify its linked actors.
      #raise ex
    end
  end

  # Notify all the linked actors that this actor has exited with an
  # error
  def handle_error(reason)
    links = nil
    @lock.receive
    begin
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
  
  #NOTE: For now we only support trap_exit mode so it is always true
  def trap_exit=(value)
    raise ArgumentError, "must be true or false" unless value == true or value == false
    #@trap_exit = value
  end
  
  # Is the Actor trapping exit?
  def trap_exit?
    @trap_exit
  end
end
