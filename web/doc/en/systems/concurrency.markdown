---
layout: doc_en
title: Concurrency
previous: FFI
previous_url: systems/ffi
next: IO
next_url: systems/io
---

Rubinius supports the same basic concurrency constructs as Ruby, Threads and Fibers, along with a new API: Actors.  Actors provide concurrency without any mutexes or locking inherent when sharing state between Threads.

Actors execute concurrently but don't share state; instead they pass messages to other actors.  Here we create two actors using `Actor.spawn`, ping and pong, who will send messages back and forth until they have collaboratively incremented a variable to 1000:

    require 'actor'
    pong = nil
    ping = Actor.spawn do
      loop do
        count = Actor.receive
        break puts(count) if count > 1000
        pong << (count + 1)
      end
    end
    pong = Actor.spawn do
      loop do
        count = Actor.receive
        break puts(count) if count > 1000
        ping << (count + 1)
      end
    end
    ping << 1
    sleep 1

Note that actors receive messages by using `Actor.receive`.  This will block the actor until a message arrives in its own mailbox.  You can process different types of messages by passing a block into `Actor.receive` and using message filtering:

    Actor.receive do |filter|
      filter.when(Ready) do |who|
        # SNIP
      end
      filter.when(Work) do |work|
        ready_workers.pop << work
      end
      filter.when(Actor::DeadActorError) do |exit|
        print "Actor exited with message: #{exit.reason}\n"
        ready_workers << Actor.spawn_link(&work_loop)
      end
    end

Message filtering uses === on the message so you can pass in a regexp, class or proc to `when()`, based on your needs.

Actors can also have a parent/child relationship using `Actor.spawn_link`; if the child dies for some reason, the parent actor can be notified by setting `Actor.trap_exit` to true before spawning the child.  Here we create a supervisor actor who manages a work queue with 10 worker actors to do queue processing.  The supervisor will receive an `Actor::DeadActorError` message if a worker dies (e.g. raises an exception).

    require 'actor'

    Ready = Struct.new(:this)
    Work = Struct.new(:msg)

    @supervisor = Actor.spawn do
      supervisor = Actor.current
      work_loop = Proc.new do
        loop do
          work = Actor.receive
          puts("Processing: #{work.msg}")
          supervisor << Ready[Actor.current]
        end
      end

      Actor.trap_exit = true
      ready_workers = []
      10.times do |x|
        # start 10 worker actors
        ready_workers << Actor.spawn_link(&work_loop)
      end
      loop do
        Actor.receive do |f|
          f.when(Ready) do |who|
            # SNIP
          end
          f.when(Work) do |work|
            ready_workers.pop << work
          end
          f.when(Actor::DeadActorError) do |exit|
            print "Actor exited with message: #{exit.reason}\n"
            ready_workers << Actor.spawn_link(&work_loop)
          end
        end
      end
    end

    10.times do |idx|
      @supervisor << Work[idx]
    end
    sleep 1

This example is a snippet from [girl_friday](http://github.com/mperham/girl_friday).  You can review that codebase for more detail if you have questions.