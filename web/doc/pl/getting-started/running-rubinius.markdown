---
layout: doc_pl
title: Uruchomienie Rubiniusa
previous: Kompilacja
previous_url: getting-started/building
next: Rozwiązywanie problemów
next_url: getting-started/troubleshooting
---

Gdy zakończyłeś etap kompilacji (oraz opcjonalnie instalacji)
Rubiniusa, możesz  sprawdzić czy działa on poprawnie wpisując:

    rbx -v

Rubinius działa bardzo podobnie do Rubiego z linii poleceń. Na przykład:

    rbx -e 'puts "Hello!"'

Aby uruchomić plik z kodem Rubiego 'code.rb':

    rbx code.rb

Aby uruchomić IRB (interaktywną konsolę):

    rbx

Jeśli dodałeś katalog bin do zmiennej PATH Rubinius będzie wywoływany
gdy wpiszesz którąś z poniższych komend (zupełnie tak jak w MRI): `ruby`, `rake`,
`gem`, `irb`, `ri`, and `rdoc`.

Możesz dodać katalog bin Rubiniusa do zmiennej środowiskowej PATH
tylko jeśli planujesz używać Rubiniusa do uruchamiania
programów. Dzięki temu nie będzie on kolidował ze zwykłą wersją
Rubiego (MRI), którą możesz mieć zainstalowaną w systemie w momencie
kiedy nie będziesz chcieć używać Rubiniusa.
