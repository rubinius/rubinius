---
layout: doc_ru
title: Параллелизм
previous: FFI
previous_url: systems/ffi
next: Ввод/Вывод
next_url: systems/io
---

Для обеспечения параллелизма Rubinius поддерживает те же базовые конструкции,
что и MRI Ruby: `Treads` и `Fibers`. Создан также и новый API, для реализации
**акторов**: `Actors`. Акторы обеспечивают параллелизм без участия мутексов и
блокировок в передаче статуса между тредами.

Акторы выполняются параллельно, но не имеют общего статуса. Вместо этого они
передают другим акторам сообщения. В качестве примера создадим два актора
при помощи `Actor.spawn`: `ping` и `pong`. Пусть они обмениваются сообщениями
до тех пор, пока совместно не произведут инкремент переменной до 1000:

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

Обратите внимание: акторы принимают сообщения посредством `Actor.receive`.
Этот вызов блокирует актор до тех пор, пока сообщение не придет в его
<<почтовый ящик>>. Передавая `Actor.receive` блок и используя фильтрацию
сообщений, можно учитывать в алгоритме тип сообщения:

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

Фильтры применяют к сообщениям метод `===`, поэтому `when()` можно
передавать, например, регулярные выражения, классы или процедуры --- в
зависимости от конкретной ситуации.

Акторы могут приобретать отношения <<предок-потомок>> посредством
`Actor.spawn_link`. Если потомок по какой-то причине <<умирает>>,
актор-родитель может получить об этом извещение, если перед запуском потомка
переменная `Actor.trap_exit` была установлена в `true`. Создадим
актор-супервизор, управляющий некоторой очередью при помощи 10
акторов-работников, выполняющих непосредственный процессинг очереди. Если
работник умирает (т.е. инициирует исключение), супервизор получает сообщение
`Actor::DeadActorError`.

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

Приведенный пример --- фрагмент кода из
[<<girl_friday>>](http://github.com/mperham/girl_friday). Если у Вас возникли
вопросы, поизучайте этот проект.
