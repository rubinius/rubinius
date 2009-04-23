require 'thread'
require 'actor'

# VMActors represent Actors running in another VM.  They are duck-typed to Actors
# and can be used interchangably with them.  They're useful for "scatter/gather"
# styles of programming, where a given dataset is partitioned among a number of
# VMs (each running in a native thread) so that respective portions of the data
# set are processed concurrently.  They can also be used for simple load balancing
# between multiple copies of the same service running in different VMs.
class VMActor
  @@actors = {}
  @@actors_lock = Mutex.new

  class << self
    # Store the specified actor in the local Actor registry
    def register(actor)
      @@actors_lock.synchronize { @@actors[actor.object_id] = actor }
    end

    # Process an incoming inter-VM actor message
    def dispatch(command, *operands)
      case command
      when :spawn then spawn(*operands)
      when :message then process_message(*operands)
      end
    end

    # Serialize an outgoing message.  Why?  Doesn't Marshal already handle this?
    # Well, not really.  One of the definining characteristics of Actors local
    # ones behave identically to remote ones, be it on a different CPU on the
    # same machine or a different machine entirely, located elsewhere on the
    # network.
    # Among other things, this means that we should be able to include Actor
    # handles in messages, regardless of if the handle is to a local or remote
    # Actor.  A potential use case is synchronous communication, which requires
    # we include the "return address" of an Actor in a message.
    #
    # In order to accomplish this, we need to translate any Actor handles in the
    # outgoing message into a form that will be instantiated into a VMActor on
    # the other side.
    #
    # FIXME: This implementation is naive, and only examines Arrays and Tuples as
    # potential containers.  Obviously it's possible to transmit Actor objects in
    # other ways (e.g. in a Hash, or an instance variable in an arbitrary object)
    # which this implementation will miss.  A better approach is needed.
    def serialize_message(value)
      case value
      when Array then value.map { |child| serialize_message(child) }
      when Rubinius::Tuple then Rubinius::Tuple[*serialize_message(value.to_a)]
      when Actor
        VMActor.register value
        Rubinius::Tuple[:VMActor, Rubinius::VM_ID, value.object_id]
      else value
      end
    end

    # Process an incoming message, translating VMActor Tuples into VMActor objects
    def unserialize_message(value)
      case value
      when Rubinius::Tuple
        if value.first == :VMActor
          _, vm_id, actor_id = value
          VMActor.new(vm_id, actor_id)
        else
          Rubinius::Tuple[*value.map { |child| unserialize_message(child) }]
        end
      else value
      end
    end

    #######
    private
    #######

    # Create a new VMActor, performing either a faux-apply of a Class/Method/Argument
    # combination or an evaluation of a string to create Actors on remote nodes.
    # This is called directly by the remote VM itself.  In order to create new VMActors
    # use VMActor::Container#spawn_actor instead.
    def spawn(container, remote_actor, *operands)
      actor = case operands.first
      when String
        command = operands.first
        Actor.spawn { eval command }
      when Symbol
        klass, meth, *args = operands
        klass = Object.const_lookup klass

        Actor.spawn { klass.send(meth, *args) }
      else
        raise ArgumentError, "first argument must be a Symbol (for application) or String (for evaluation)"
      end

      register(actor)

      Rubinius::VM.send_message(container, [
        :actor,
        :message,
        remote_actor,
        Rubinius::Tuple[:spawn_reply, actor.object_id]
      ])
    rescue Exception => ex
      Rubinius::VM.send_message(container, [
        :actor,
        :message,
        remote_actor,
        Rubinius::Tuple[:spawn_error, "#{ex.class}: #{[ex, *ex.backtrace].join("\n\t")}"]
      ])
    end

    # Process a message sent from a remote Actor and direct it to the appropriate
    # local one.
    def process_message(actor_id, message)
      actor = @@actors_lock.synchronize { @@actors[actor_id] }
      #puts "sending #{unserialize_message(message).inspect} to #{actor.inspect}"
      return unless actor
      actor << unserialize_message(message)
    end
  end

  def initialize(container_id, actor_id)
    @container_id, @actor_id = container_id, actor_id
  end

  # Send a message to a VMActor
  def <<(value)
    Rubinius::VM.send_message(@container_id, [:actor, :message, @actor_id, VMActor.serialize_message(value)])
  end

  alias_method :send, :<<

  #######
  private
  #######

  # VMActor::Container wraps Rubinius's native multi-VM support.  The newly
  # created VM is automatically configured for supporting remote Actors, but
  # can still be given arbitrary command line arguments.
  class Container
    @@responder = nil

    class << self
      # Start the message responder thread for a parent VM
      def init_message_responder
        @@responder ||= Thread.new(&method(:process_messages))
      end

      # Start message processing in a child VM
      def child_message_responder(parent_vm = nil, actor = nil)
        # Notify the parent VM that the current one is ready
        Rubinius::VM.send_message(parent_vm, [
          :actor,
          :message,
          actor,
          Rubinius::Tuple[:ready, Rubinius::VM_ID]
        ])

        process_messages
      end

      # Receive from the VM message queue
      # Actor events get sent to VMActor.dispatch
      def process_messages
        Rubinius::VM.each_message do |command, *operands|
          VMActor.dispatch(*operands) if command == :actor
        end
      end
    end

    # Create a new VMActor.  Accepts command line arguments to pass to the new VM
    def initialize(*args)
      Container.init_message_responder
      VMActor.register Actor.current

      # Ensure any -r's passed happen before we execute the message processor
      args += [
        "-rvmactor",
        "-e", "VMActor::Container.child_message_responder(#{Rubinius::VM_ID},#{Actor.current.object_id})",
      ]

      @vm = Rubinius::VM.spawn(*args)

      Actor.receive do |filter|
        # Wait for new VM to become ready
        filter.when(Rubinius::Tuple[:ready, @vm.id]) {}
      end
    end

    # Create a new Actor on a remote VM and obtain a handle to it.  Accepts arguments
    # specifying what code to execute in the context of the new Actor:
    #
    # == "Apply" style
    #
    # The first approach accepts a class name and method name (as symbols) and an
    # arbitrary number of arguments (or none).  The remote Actor will invoke the
    # given method of the given class with the given arguments and return a handle
    # to the VMActor the method is running in the context of:
    #
    #  container = VMActor::Container.new
    #  actor = container.spawn_actor :Kernel, :puts, 'hi'
    #
    # == "Eval" style
    #
    # The second approach accepts a string to evaluate in the context of the newly
    # created actor:
    #
    #  container = VMActor::Container.new
    #  actor = container.spawn_actor "puts 'hi'"
    #
    def spawn_actor(*args)
      VMActor.register Actor.current

      Rubinius::VM.send_message(@vm.id, [
        :actor,
        :spawn,
        Rubinius::VM_ID,
        Actor.current.object_id,
        *args
      ])

      Actor.receive do |filter|
        filter.when(Rubinius::Tuple[:spawn_reply, Object]) do |message|
          _, actor_id = message
          return VMActor.new(@vm.id, actor_id)
        end

        filter.when(Rubinius::Tuple[:spawn_error, Object]) do |message|
          _, ex = message
          raise ex
        end
      end
    end
  end
end

