---
layout: doc_pl
title: Analiza pamięci
previous: Profiler
previous_url: tools/profiler
next: Jak
next_url: how-to
review: true
---

Rubinius udostępnia interfejs do wykonywania zrzutu pamięci (sterty -
heap) do pliku w celu analizy. Istnieje kilka narzędzi umożliwiających
wykrycie wycieków pamięci, dużych kolekcji obiektów oraz potencjalnych
problemów z kodem mogących prowadzić do problemów z pamięcią.

## Przykładowy program

Poniższy program będzie służył jako przykład do analizy wycieku
pamięci w kodzie Ruby jak również w kodzie korzystającym z podsystemu FFI.

Kod został sztucznie spreparowany jednak posłuży jako dobry przykład
problemów z jakimi możemy się spotkać pisząc rzeczywiste programy.

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

Ten program cieknie jak z durszlaka :). Sprawdźmy dlaczego:

## Zapisanie sterty

Rubinius umożliwia dostęp do swojej maszyny wirtualnej poprzez
interfejs agenta. Agent otwiera gniazdo sieciowe i odpowiada na
polecenia wydawane przez program w konsoli. Agenta uruchamiamy wraz z
programem w poniższy sposób.

    rbx -Xagent.start <program>

Korzystając z powyższego polecenia uruchomimy zarówno program jak i agenta.

    rbx -Xagent.start leak.rb

Teraz możemy nawiązać połączenie z agentem za pomocą konsoli rbx (rbx
console). Polecenia wywoływane w konsoli są przekazywane do agenta
który działa w tym samym procesie co maszyna wirtualna. Teraz wydamy
kilka poleceń które służą do zrzutu pamięci do późniejszej analizy.

Podczas startu agent tworzy plik w katalogu
$TMPDIR/rubinius-agent.\<pid\> zawierający kilka przydatnych
informacji z których korzysta konsola rbx. Kiedy zamkniemy agenta ten
plik zostaje przez niego usunięty. W pewnych przypadkach
(np. segfault) ten plik może nie zostać usunięty i konieczne będzie
jego ręczne usunięcie.

    $ rbx console
    VM: rbx -Xagent.start leak.rb tcp://127.0.0.1:5549 1024 100000000
    Connecting to VM on port 60544
    Connected to localhost:60544, host type: x86_64-apple-darwin10.5.0
    console> set system.memory.dump heap.dump
    console> exit

Polecenie którym wykonujemy zrzut pamięci to `set system.memory.dump <filename>`. Plik z zapisem stanu pamięci jest tworzony w katalogu w
którym działa program z uruchomionym agentem.

## Analiza sterty

Plik ze stertą zostaje zapisany w udokumentowanym formacie. Do tej
pory znane nam są 2 narzędzia, które potrafią zinterpretować ten
format. Są to projekty odrębne do projektu Rubinius.

Jednym z tych narzędzi jest heap_dump [strona domowa projektu](https://github.com/evanphx/heap_dump).

Heap dump czyta plik ze stertą i wyświetla kilka przydatnych
informacji w 3 kolumnach. Informacje te odnoszą się do liczby obiektów
widocznych na stercie, ich klas oraz sumy bajtów zajętych przez
wszystkie instancje tego obiektu.

Jeśli uruchomimy to narzędzie przekazując mu plik ze stertą z naszego
programu `leak.rb` uzyskamy kilka wskazówek odnośnie tego, gdzie
znajduje się wyciek pamięci.

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

Na pierwszy rzut oka nic nie wydaję się być podejrzane. Możemy jednak
zauważyć kilka rzeczy.

1. Najwięcej pamięci (20MB) zajmują obiekty klasy
`Rubinius::CompactLookupTable`. Jest to klasa której nasz kod
bezpośrednio nie wywołuje. Dowiadujemy się więc o pewnych wewnętrznych
strukturach Rubiniusa. Jest to ciekawe lecz nie pomaga nam w diagnozie
problemu.

2. Klasa `ZMQ::Message` w linii 3 jest pierwszą klasą, do której
nasz kod bezpośrednio się odwołuje. Istnieje 168987 instancji tej
klasy więc to jest zapewne nasz wyciek.

Czasami aby lepiej zdiagnozować wyciek nie wystarczy jeden zrzut
sterty. W takim wypadku powinniśmy wykonać kilka zrzutów w różnym
czasie działania programu i pozwolić programowi "heap dump" wykonać
analizę różnic. Analiza różnic pokazuje które obiekty zmieniły się na
stercie *przed* i *po*.

    $ rbx -I /path/to/heap_dump/lib /path/to/heap_dump/bin/histo.rb heap.dump heap2.dump
    203110   Rubinius::CompactLookupTable 25998080
    203110                   ZMQ::Message 9749280
    203110                    LibZMQ::Msg 8124400
    203110             FFI::MemoryPointer 8124400

Wynik wyraźnie pokazuje źródło znacznego wzrostu zużycia
pamięci. Sterta zyskała 203110 nowych obiektów klasy `ZMQ::Message`
pomiędzy pierwszym i drugim zrzutem.

Badając nasz kod zauważamy 2 podejrzane linie:

    messages << message
    ...
    puts "received #{messages.size} messages in #{elapsed_usecs / 1_000_000} seconds"

Z pewnością przechowywanie każdej wiadomości nie jest konieczne aby na
końcu uzyskać liczbę wszystkich wiadomości. Zmiana kodu tak aby
korzystał z prostej zmiennej zliczającej wiadomości powinno naprawić wyciek.

## Zaawansowane narzędzia - tylko OSX

Po naszych modyfikacjach (użycie zmiennej zliczającej) odśmiecacz (GC)
zajmuje się poprawnie usunięciem wszystkich instancji
`ZMQ::Message`. Jednak nasz program wciąż cieknie. Wykonanie 2 zrzutów
nie pomaga.

    $ rbx -I /path/to/heap_dump/lib /path/to/heap_dump/bin/histo.rb heap3.dump heap4.dump
      -4                          Array -224
     -90                 Digest::SHA256 -4320
     -90          Rubinius::MethodTable -4320
     -90                   Digest::SHA2 -3600
     -90          Rubinius::LookupTable -4320
     -90                          Class -10080
    -184                Rubinius::Tuple -29192

Powyższa analiza róznicowa pokazuje, że kilka struktur skurczyło się
pomiędzy zrzutami a nie powiększyło. Najwyraźniej wyciek nie jest już
w kodzie Ruby ponieważ maszyna wirtualna nie jest w stanie wskazać co
zajmuje tą pamięć.

Na szczęście jeśli mamy dostęp do systemu Mac OS X mamy również dostęp
do programu o nazwie `leaks` który pomoże nam w wykryciu
problemu. Dodatkowo czytając manual do `malloc` (man malloc) dowiemy
się o ustawieniu zmiennej środowiskowej która umożliwi programowi
`leaks` dostęp do aktualnego stanu stosu w miejscu każdego wycieku.

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

Wynik wykonania programu `leaks` pokazuje, że w momencie zrzutu
mieliśmy prawie 172000 wyciekających obiektów. Obraz stosu pokazuje,
że wyciek nastąpił podczas wywołania `zmq_msg_init_size` co nabierze
sensu dopiero gdy zagłebimy się w szczegóły implementacji klasy
`ZMQ::Message``. W tym miejscu przydaje się znajomość szegółów naszego
systemu; bez tej znajomości byłoby trudno namierzyć ten problem.

Okazuje się, że `ZMQ::Message` alokuje pamięć przy pomocy `malloc`
która to pamięć nie jest kontrolowania przez odśmiecacz
Rubiniusa. Pamięć ta musi zostać ręcznie zwolniona.

Zmiana kodu aby wywołał `ZMQ::Message#close` poprawia ten wyciek.
