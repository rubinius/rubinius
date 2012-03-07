---
layout: doc_ru
title: Анализ памяти
previous: Профайлер
previous_url: tools/profiler
next: How-To
next_url: how-to
review: true
---

Rubinius обеспечивает интерфейс для выгрузки рабочей кучи (current heap) в
файл для оффлайн-анализа. Существует еще несколько вспомогательных проектов
для анализа дампа кучи с целью выявления утечек, проблем с большими
коллекциями и просто кодом, могущих привести к напряженным ситуациям с памятью
во время выполнения программы.

## Программа-образец

Нижеследующий код (синтаксически безупречный) станет образцом для
нашего экскурса в процесс обнаружения точек утечки памяти в Ruby-коде и в
коде, используемом в FFI-подсистеме. Пример немного искуственен, но в нем
проиллюстрировано сразу несколько проблем, характерных для реальной жизни.

    require 'rubygems'
    require 'ffi-rzmq'
    
    if ARGV.length < 3
      puts "usage: ruby leak.rb <connect-to> <message-size> <roundtrip-count>"
      exit
    end
    
    link = ARGV[0]
    message_size = ARGV[1].to_i
    roundtrip_count = ARGV[2].to_i
    
    ctx = ZMQ::Context.new
    request_socket = ctx.socket ZMQ::REQ
    reply_socket = ctx.socket ZMQ::REP
    
    request_socket.connect link
    reply_socket.bind link
    
    poller = ZMQ::Poller.new
    poller.register_readable request_socket
    poller.register_readable reply_socket
    
    start_time = Time.now
    
    message = ZMQ::Message.new("a" * message_size)
    request_socket.send message, ZMQ::NOBLOCK
    i = roundtrip_count
    messages = []
    
    until i.zero?
      i -= 1
    
      poller.poll_nonblock
    
      poller.readables.each do |socket|
        message = ZMQ::Message.new
        socket.recv message, ZMQ::NOBLOCK
        messages << message
        socket.send ZMQ::Message.new(message.copy_out_string), ZMQ::NOBLOCK
      end
    end
    
    elapsed_usecs = (Time.now.to_f - start_time.to_f) * 1_000_000
    latency = elapsed_usecs / roundtrip_count / 2
    
    puts "mean latency: %.3f [us]" % latency
    puts "received #{messages.size} messages in #{elapsed_usecs / 1_000_000} seconds"

О! Программа жрет память как свинья --- помои! Давайте разберемся, почему.

## Выгрузка дампа кучи

Rubinius дает доступ к виртуальной машине (далее --- VM) посредством агент-интерфейса (agent
interface). Агент открывает сетевой сокет и отвечает на команды консольной
программы, вместе с которой его и следует запускать:

    rbx -Xagent.start <script name>

Пример: запуск исследуемой программы с подключенным агентом:

    rbx -Xagent.start leak.rb

Коннектимся к агенту rbx-консолью. Получаем интерактивный сеанс с агентом,
<<крутящимся>> внутри VM. Команды направляются непосредственно агенту. В нашем
примере мы сохраняем дамп кучи для последующего оффлайн-анализа.

При запуске агент пишет создает файл `$TMPDIR/rubinius-agent.<pid>`,
содержащий кое-какие важные данные для rbx-консоли. При выходе агент
автоматически зачищает этот файл и удаляет его. В некоторых аварийных случаях
файл может остаться неудаленным, тогда его необходимо удалить вручную.

    $ rbx console
    VM: rbx -Xagent.start leak.rb tcp://127.0.0.1:5549 1024 100000000
    Connecting to VM on port 60544
    Connected to localhost:60544, host type: x86_64-apple-darwin10.5.0
    console> set system.memory.dump heap.dump
    console> exit

Интересующая нас команда --- `set system.memory.dump <filename>`. Файл дампа
пишется в текущую рабочую директорию программы, подключившей агент.

## Анализ дампа

Дамп кучи записан в формате, который сам по себе хорошо документирован. На
нынешний момент есть два инструмента, которые этот формат читают и
интерпретируют --- два отдельных от Rubinius проекта.

Программа `heap_dump` берется [на страничке соответствующего проекта](https://github.com/evanphx/heap_dump).

Утилита читает дамп и выводит полезную информацию тремя колонками: количество
видимых в куче объектов, класс этих объектов, и суммарное число байт, занятое
всеми экземплярами объекта.

Напустив утилитку на дамп, выгруженный из нашей `leak.rb`, мы получаем тонкий
намек на местоположение интересующей нас утечки (вывод отредактирован, чтобы
он поместился на странице):

    $ rbx -I /path/to/heap_dump/lib /path/to/heap_dump/bin/histo.rb heap.dump 
        169350   Rubinius::CompactLookupTable 21676800
        168983             FFI::MemoryPointer 6759320
        168978                   ZMQ::Message 8110944
        168978                    LibZMQ::Msg 6759120
         27901                Rubinius::Tuple 6361528
         15615                         String 1124280
         13527            Rubinius::ByteArray 882560
          3010                          Array 168560
           825                    Hash::Entry 46200
           787       Rubinius::AccessVariable 62960
            87                           Time 4872
            41                           Hash 3280
            12                   FFI::Pointer 480
             2                    ZMQ::Socket 96

В принципе, в нашем примере нет чего-либо очень уж экстраординарного, однако
некоторые странности заметны:

1. Наибольшее место заняли объекты `Rubinius::CompactLookupTable`, класса,
   который написанный нами код напрямую не инстанциирует --- около 20
   мегабайт. То есть, некоторые внутренности Rubinius отображаются в анализе
   дампа. Факт интересный, но в выявлении нашей утечки он не поможет.

2. Класс `ZMQ::Message` на третьей строчке --- первый из тех, экземпляры
   которого наш код непосредственно создает. Этих экземпляров набралось аж 170
   тысяч, так что это и может быть местом утечки.

Иногда разового снапшота для нахождения утечки недостаточно. В этом случае
надо сделать несколько снимков кучи в разное время и выполнить при помощи
`heap_dump` *diff*-анализ. *diff* показывает изменения в куче между
состояниями *до* и *после*:

    $ rbx -I /path/to/heap_dump/lib /path/to/heap_dump/bin/histo.rb heap.dump heap2.dump
    203110   Rubinius::CompactLookupTable 25998080
    203110                   ZMQ::Message 9749280
    203110                    LibZMQ::Msg 8124400
    203110             FFI::MemoryPointer 8124400

Здесь *diff* ясно показывает источник <<раздутой>> памяти. Между первым и
вторым снимками накопилось 200 тысяч экземпляров `ZMQ::Message`, что и
объясняет <<распухание>>.

Проверка кода выводит на две строчки, претендующие на роль подозреваемых:

    messages << message
    ...
    puts "received #{messages.size} messages in #{elapsed_usecs / 1_000_000} seconds"

Очевидно, что нет необходимости сохранять каждую мессажку только для того,
чтобы в конце концов их подсчитать. Рефакторинг с простым введением
переменной-счетчика прикончит обнаруженную утечку.

## Продвинутый инструмент, только для OSX

После переделки кода под счетчик и сдачи сборщику мусора всех прав на
`ZMQ::Message` программа продолжает жрать память. Два снимка с последующим
анализом не дают даже намека на то, почему:

    $ rbx -I /path/to/heap_dump/lib /path/to/heap_dump/bin/histo.rb heap3.dump heap4.dump
      -4                          Array -224
     -90                 Digest::SHA256 -4320
     -90          Rubinius::MethodTable -4320
     -90                   Digest::SHA2 -3600
     -90          Rubinius::LookupTable -4320
     -90                          Class -10080
    -184                Rubinius::Tuple -29192

`diff` показывает, что, в сущности, некоторые структуры за измеряемый
промежуток даже <<похудели>>. Очевидно, утечку теперь следует искать не в
Ruby-части, поскольку VM самостоятельно не может указать, что именно
потребляет все больше памяти.

По счастью, на Mac OS X есть чудесная утилита, именуемая `leaks` --- она-то
нам и поможет. К тому же man-страничка `malloc` содержит сведения, как
выставить переменную окружения, снабжающую `leaks` дополнительной информацией
о трассировке вызовов на момент каждой утечки.

    $ MallocStackLogging=1 rbx leak.rb tcp://127.0.0.1:5549 1024 10000000 &
    $ leaks 36700 > leak.out
    $ vi leak.out
    leaks Report Version:  2.0
    Process:         rbx [36700]
    Path:            /Volumes/calvin/Users/cremes/.rvm/rubies/rbx-head/bin/rbx
    Load Address:    0x100000000
    Identifier:      rbx
    Version:         ??? (???)
    Code Type:       X86-64 (Native)
    Parent Process:  bash [997]
    
    Date/Time:       2010-12-22 11:34:35.225 -0600
    OS Version:      Mac OS X 10.6.5 (10H574)
    Report Version:  6
    
    Process 36700: 274490 nodes malloced for 294357 KB
    Process 36700: 171502 leaks for 263427072 total leaked bytes.
    Leak: 0x101bb2400  size=1536  zone: DefaultMallocZone_0x100dea000
            0x01bb2428 0x00000001 0x00000400 0x00000000     ($..............
            0x00000000 0x00000000 0x00000000 0x00000000     ................
            0x00000000 0x00000000 0x61616161 0x61616161     ........aaaaaaaa
            0x61616161 0x61616161 0x61616161 0x61616161     aaaaaaaaaaaaaaaa
            0x61616161 0x61616161 0x61616161 0x61616161     aaaaaaaaaaaaaaaa
            0x61616161 0x61616161 0x61616161 0x61616161     aaaaaaaaaaaaaaaa
            0x61616161 0x61616161 0x61616161 0x61616161     aaaaaaaaaaaaaaaa
            0x61616161 0x61616161 0x61616161 0x61616161     aaaaaaaaaaaaaaaa
            ...
            Call stack: [thread 0x102f81000]: | thread_start | _pthread_start | 
            thread_routine | zmq::kqueue_t::loop() | zmq::zmq_engine_t::in_event() | 
            zmq::decoder_t::eight_byte_size_ready() | zmq_msg_init_size | malloc | 
            malloc_zone_malloc

Вывод утилиты показывает, что на момент фиксации у нас оказалось около 172
тысяч <<утекших>> объектов. Стек вызовов говорит о том, что утечка происходит
во время вызова `zmq_msg_init_size`. Все это лишено смысла до тех пор, пока мы
не заглянем в реализацию `ZMQ::Message`. В этот момент становится критичным
знание системы: не зная, где происходит данный конкретный вызов, отследить
проблему в разы сложнее.

Оказывается, класс `ZMQ::Message` аллоцирует `malloc`-ом куски памяти,
невидимые сборщику мусора Rubinius-а. Их придется освобождать вручную.

Вписывание в код вызова `ZMQ::Message#close` решает проблему утечки
окончательно.
