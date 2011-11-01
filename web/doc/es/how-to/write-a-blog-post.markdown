---
layout: doc_es
title: Cómo - escribir un post en el Blog
previous: Fix a Failing Spec
previous_url: how-to/fix-a-failing-spec
next: Write Documentation
next_url: how-to/write-documentation
translated: true
---

El blog Rubinius utiliza Jekyll y está integrado con el sitio web y
documentación. Animamos y apreciar los mensajes de evaluación sobre su blog
experiencias en el uso o el desarrollo de Rubinius.

Para empezar, asegúrese de tener las gemas `kramdown` y `jekyll` instaladas.

     rbx gem install kramdown jekyll

El formato preferido para los post en el blog es Markdown. Sin embargo, si usted
tiene especial necesidades del formato, el mensaje puede estar escrito en HTML
directamente.

1. Cree un archivo en `web/_posts/` con el formato
    `YYYY-MM-DD-perma-link.markdown` para el nombre del archivo.
1. Escriba el mensaje.
1. En el `web/`, ejecute `rbx -S jekyll`.
1. Crear un commit de todos los cambios en el `web/` de la guía.
1. Enviar un parche, o si usted tiene los derechos de comprometerse, impulsar
   el compromiso de dominar.
1. Nos dicen que hay una nueva entrada de blog. Es posible que haya alguna
   información para usted antes de publicar.
