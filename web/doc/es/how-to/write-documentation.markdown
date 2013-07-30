---
layout: doc_es
title: Cómo - escribir documentación
previous: Escribir una entrada en el blog
previous_url: how-to/write-a-blog-post
next: Traducir documentación
next_url: how-to/translate-documentation
---

La documentación de Rubinius está integrada con el sitio web y el blog.
Utiliza Jekyll al igual que los otros componentes.

Para empezar, asegúrate de tener las gemas `kramdown` y `jekyll` instaladas.

     rbx gem install kramdown jekyll

Los fuentes de la documentación se encuentran en el directorio `web/doc`. Hay
subdirectorios por cada idioma para el cual la documentación ha sido traducida
(por ejemplo `en`, `es`, etc.)

Hay una Tabla de Contenido para cada traducción (por ejemplo
`web/doc/es/index.markdown`). El resto de la documentación consiste en
archivos individuales que tienen atributos YAML para especificar cómo se
están conectados los documentos. Esencialmente, la documentación puede ser
vista como una lista doblemente enlazada de documentos, con cada documento
apuntando al anterior y al siguiente. El documento Tabla de Contenido
muestra la estructura completa.

Los atributos YAML en un documento se ven como como los siguientes:

    ---
    layout: doc_es
    title: How-To - Write Documentation
    previous: Write a Blog Post
    previous_url: how-to/write-a-blog-post
    next: Translate Documentation
    next_url: how-to/translate-documentation
    ---

El _layout_ especifica a Jekyll que diagramación utilizar cuando se formatee
el documento. El _layout_ debe ser `doc_LANG`, donde _LANG_ es el código
ISO-639-2 para el idioma.

El _title_ especifica el título del documento que se muestra en la parte
superior de la página.

Los atributos _previous_ y _previous\_url_ dan el título y el vínculo al
documento anterior. Del mismo modo, los atributos _next_ y _next\_url_ dan
el título y el vínculo para el próximo documento. Éstos se utilizan para
mejorar la navegación de la documentación, limitando la cantidad de trabajo
necesario para reordenar partes de ésta.

Tanto los fuentes de la documentación como los archivos generados por Jekyll
son subidos al repositorio de Rubinius. Cuando las personas clonan el
repositorio de Rubinius, pueden ejecutar `rake docs` para ver la documentación
antes de compilar Rubinius o si tienen problemas en la compilación.


### Editando documentación ya existente

Un primer esbozo de la documentación ya ha sido creado. Hay muchos temas que
simplemente necesitan que se escriba documentación para ellos.

Para agregar documentación a un tema existente o para corregir documentación
existente:

1. Abre el archivo para el tema en `web/doc/LANG`.
1. Agrega o mejora la documentación.
1. Para ver tus modificaciones mientras trabajas en ellas, ejecuta
   `rbx -S jekyll serve --watch`
1. Una vez que finalices de agregar o editar documentación, haz commit de tus
   cambios a los archivos fuente.
1. Ejecuta `rbx -S jekyll build` en el directorio `web/` para forzar la
   actualización de todos los archivos generados en `web/_site`.
1. Haz commit de los archivos generados. Si haces un cambio pequeño, puedes
   hacer commit de los archivos generados junto con los fuentes de la
   documentación. Si haces muchos cambios, haz commit de los archivos
   generados en forma separada para hacer más fácil la revisión de los
   cambios.


### Agregando nueva documentación

Para agregar documentación para la cuál no existe un tema:

1. Crea un nuevo archivo con extensión .markdown bajo `web/doc/LANG`.
1. Edita los atributos para enlazar el nuevo archivo en los archivos ya
   existentes. Esto requerirá editar los atributos _previous_ y _next_ de los
   archivos ya existentes para insertar el nuevo archivo, así como también
   agregar una entrada en `index.markdown`.
1. Para ver tus modificaciones mientras trabajas en ellas, ejecuta
   `rbx -S jekyll serve --watch`
1. Edita el nuevo archivo utilizando sintaxis Markdown.
1. Una vez que finalices de agregar o editar documentación, haz commit de tus
   cambios a los archivos fuente.
1. Ejecuta `rbx -S jekyll build` en el directorio `web/` para forzar la
   actualización de todos los archivos generados en `web/_site`.
1. Haz commit de los archivos generados. Si haces un cambio pequeño, puedes
   hacer commit de los archivos generados junto con los fuentes de la
   documentación. Si haces muchos cambios, haz commit de los archivos
   generados en forma separada para hacer más fácil la revisión de los
   cambios.
