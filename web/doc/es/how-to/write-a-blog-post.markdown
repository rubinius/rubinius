---
layout: doc_es
title: Cómo - Escribir un post en el Blog
previous: Fix a Failing Spec
previous_url: how-to/fix-a-failing-spec
next: Write Documentation
next_url: how-to/write-documentation
---

El [blog de Rubinius][1] utiliza [Jekyll][2] y está integrado con el sitio web y
la documentación. Animamos y apreciamos los posts de evaluación de las
experiencias en el uso o el desarrollo de Rubinius.

El formato preferido para los posts en el blog es Markdown. Sin embargo, si usted
tiene alguna necesidad especial en el formato, el post puede ser escrito en HTML
directamente.

Clone el repositorio Rubinius, que es dónde está el website y los posts del blog.

    git clone https://github.com/rubinius/rubinius.git

Para empezar, asegúrese de tener las gemas `kramdown` y `jekyll` instaladas.

     rbx gem install kramdown jekyll

Ahora vaya al directorio de los posts

    cd rubinius/web/_posts

Cree un archivo desde la consola usando el siguiente formato
de nombre de archivo `YYYY-MM-DD-perma-link.markdown`.

    touch "%(date +"%Y-%m-%d")-perma-link-title.markdown"

Escriba un post brillante.

    cat /dev/random > <<the file post>> # :-p

Ejecute jekyll para compilar su website, inlcuyendo su post.

    cd ../web/
    rbx -S jekyll build

Cree un commit para todos los cambios en el directorio `web/`.

    cd ../
    git add web/
    git commit -m "Wrote a blog post on ....."

Envie un patch, pull request, o si ud. tiene derecho de commit, pushee
el commit a la rama master.

Díganos que ahí hay un nuevo post. Quizá tengamos un poco de feedback
para ud. antes de publicarlo.

[1]: /blog "Rubinius' Blog"
[2]: https://github.com/mojombo/jekyll "Mojombo's Jekyll"
