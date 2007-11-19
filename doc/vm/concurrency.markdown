## Concurrency

Rubinius provides a number of concurrency techniques, significantly expanding upon that available in standard Ruby. As such, use of these additional primitives is non-standard.

_Note:_ Currently, Rubinius achieves concurrency through the use of green-threads. Native thread support is not implemented at present, although it is planned for the 2.0 release of Rubinius, and Shotgun has been constructed with thread-safety in mind.

### Tasks

Tasks are the foundation of green threads in Rubinius. See [here](http://blog.fallingsnow.net/2007/04/05/task-class-in-svn/) for an overview of Rubinius tasks.

### Threads

### Channels

Channels provide a simple mechanism for coordinating the work of two or more threads. A channel resembles a queue, in that producer thread(s) can write to the channel, while consumer thread(s) read from it. If there is no data currently in the channel, consumer threads will suspend until data is available.

### Actors

The `Actor` class provides an implementation of the _Actor_ concurrency pattern. Actor instances are threads that can be given work to do via message objects. Actor instances process messages according to filters, deciding what to do based on the type of message to be processed (where comparison of message types uses the same comparison logic as __case__, namely `===`).

Unlike Channels, the read end of an Actor is fixed (to the thread owned by the Actor), and the Actor can prioritise handling of messages, and even postpone processing of messages for which no filter has yet been defined.

    Actor.receive do |filter|
      filter.when SomeMessageClass do |message|
        # handle message
      end
      filter.when OtherMessageClass do |message|
        # handle message
      end
    end
