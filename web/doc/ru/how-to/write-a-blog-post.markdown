---
layout: doc_ru
title: How-To - Написание сообщения в блог
previous: Написание бенчмарков
previous_url: how-to/write-benchmarks
next: Написание документации
next_url: how-to/write-documentation
---

[Блог Rubinius][1] использует [Jekyll][2] и интегрирован с веб-сайтом и
документацией. Мы ценим и всячески поощряем посты в блог, в которых
Вы рассказываете о своем опыте использования или разработки Rubinius.

Markdown является предпочтительным форматом для сообщений блога. Однако, если
вам необходимо особое форматирование, пост может быть написан сразу на HTML.

1. Клонируйте репозиторий Rubinius-а: сайт и блог находятся внутри:

       git clone https://github.com/rubinius/rubinius.git

1. Перед началом работы убедитесь, что у Вас установлены гемы `kramdown` и
   `jekyll`:

        rbx gem install jekyll kramdown

1. Зайдите в директорию постов:

        cd web/_posts

1. Создайте файл, используя для его имени следующий формат:
   `YYYY-MM-DD-perma-link.markdown`.

        touch "%(date +"%Y-%m-%d")-perma-link-title.markdown"

1. Напишите Ваше сногсшибательное сообщение:

        cat /dev/random > <<the file post>> # :-p

1. Запустите Jekyll, чтобы скомпилировать сайт с учетом Вашего поста:

        cd ../web/
        rbx -S jekyll build

1. Сделайте коммит со всеми изменениями в директории `web/`:

        cd ../
        git add web/
        git commit -m "Wrote a blog post on ....."

1. Отправьте патч, пулл-реквест или, если у вас есть право коммита в главный
   репозиторий, опубликуйте изменения в нем в ветви <<master>>.

1. Сообщите нам о новом посте: возможно перед публикацией мы захотим с Вами
   дополнительно пообщаться.

[1]: /blog "Rubinius' Blog"
[2]: https://github.com/mojombo/jekyll "Mojombo's Jekyll"
